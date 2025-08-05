#include "mainwindow.h"
#include "passwordInputWindow.h"
#include "NewElementWindow.h"
#include "passwordGeneration.h"
#include <iostream>
#include <fstream>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QFileDialog>
#include <QFileInfo>
#include <QIcon>

void clearALayout(QLayout* layout);

MainWindow::MainWindow(QWidget *parentWidget):QMainWindow(parentWidget){

  MainCentralWidget = new QWidget;
  MainLayout = new QHBoxLayout(MainCentralWidget);

  RightLayout = new QVBoxLayout;
  TopRightLayout = new QHBoxLayout;
 
  LeftLayout = new QVBoxLayout;
  TopLeftLayout = new QHBoxLayout;

 
  //top left
  loadFileBtn = new QPushButton;
  CreateNewFile = new QPushButton;
  loadFileBtn->setObjectName("loadFileBtn");
  CreateNewFile->setObjectName("CreateNewFile");

  // bottom left
  LeftScrollArea = new QScrollArea;
  LeftScrollAreaWidgetHolder = new QWidget;
  LeftLayoutHolder = new QVBoxLayout;
  LeftWidgetHolder = new QWidget;
  LeftWidgetHolder->setObjectName("LeftWidgetHolder");
  LeftScrollArea->setObjectName("LeftScrollArea");

  // linking top left widgets
  TopLeftLayout->addWidget(loadFileBtn);
  TopLeftLayout->addStretch();
  TopLeftLayout->addWidget(CreateNewFile);
  LeftLayout->addLayout(TopLeftLayout);

  // linking bottom left widgets
  LeftLayout->addWidget(LeftScrollArea);
  LeftScrollArea->setWidget(LeftScrollAreaWidgetHolder);
  LeftScrollArea->setLayout(LeftLayoutHolder);

  // linking left layouts
  LeftWidgetHolder->setLayout(LeftLayout);
  MainLayout->addWidget(LeftWidgetHolder);

  //styling left elements
  loadFileBtn->setIcon(QPixmap("icons/loadFileIcon.png"));
  CreateNewFile->setIcon(QPixmap("icons/createFile.png"));

  loadFileBtn->setIconSize(QSize(24,24));
  CreateNewFile->setIconSize(QSize(24,24));

  LeftWidgetHolder->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  LeftScrollArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  loadFileBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  CreateNewFile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  LeftLayoutHolder->setAlignment(Qt::AlignTop);

  LeftScrollArea->setMaximumWidth(400);
  LeftScrollArea->setMinimumWidth(200);

  // connecting left elements to there functions 
  connect(loadFileBtn, &QPushButton::clicked, this, &MainWindow::getFile);
  connect(CreateNewFile, &QPushButton::clicked, this, &MainWindow::CreateNewPasswordFile);


  //top right
  addNewPasswordElement = new QPushButton;
  SearchBar = new QLineEdit;
  editContent = new QPushButton;
  addNewPasswordElement->setObjectName("addNewPasswordElement");
  SearchBar->setObjectName("SearchBar");
  editContent->setObjectName("editContent");

  // bottom right
  RigthScrollArea = new QScrollArea;
  widgetholder = new QWidget;
  RightEnternelHolderLayout = new QVBoxLayout;

  // linking Top right widgets
  TopRightLayout->addWidget(addNewPasswordElement);
  TopRightLayout->addWidget(SearchBar);
  TopRightLayout->addWidget(editContent);
  RightLayout->addLayout(TopRightLayout);

  // linking Bottom right widgets
  RightLayout->addWidget(RigthScrollArea);
  RigthScrollArea->setWidget(widgetholder);
  widgetholder->setLayout(RightEnternelHolderLayout);

  // linking right layouts
  MainLayout->addLayout(RightLayout);

  //styling right layout elements 
  addNewPasswordElement->setIcon(QPixmap("icons/add.png"));
  editContent->setIcon(QPixmap("icons/editIcon.png"));

  editContent->setIconSize(QSize(24,24));
  addNewPasswordElement->setIconSize(QSize(24,24));

  addNewPasswordElement->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  SearchBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  editContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  TopRightLayout->setContentsMargins(0,9,0,0);
  LeftLayoutHolder->setContentsMargins(0,10,0,0);

  // connecting right elements to there functions
  connect(SearchBar, &QLineEdit::textChanged, this, &MainWindow::EditLineChanged);
  connect(editContent,&QPushButton::clicked,this,&MainWindow::EnableEditContent);
  connect(addNewPasswordElement,&QPushButton::clicked,this,&MainWindow::NewPasswordElement);


  loadPasswordFile();
  for(DataFile* File:loadedFiles)
    createFileButton(File);

  setCentralWidget(MainCentralWidget);
}

void MainWindow::getFile(){
  QString FilePath = QFileDialog::getOpenFileName(this,tr("Open File"),"/home/",tr("GPG (*.gpg)"));
  if(!FilePath.isEmpty()){
    QFileInfo fi(FilePath);
    DataFile *newFileElement = new DataFile;
    newFileElement->path = FilePath;
    newFileElement->fileName = fi.fileName();
    loadedFiles.push_back(newFileElement);
    createFileButton(newFileElement);
    savePasswordFile(newFileElement);
  }
}
void MainWindow::CreateNewPasswordFile(){
  QString DirectoryPath = QFileDialog::getExistingDirectory(this,tr("Select Directory"),"/home/");
  if(DirectoryPath.isEmpty()) return;
  int numberOfFiles = 0;
  for(auto& file: std::filesystem::directory_iterator(DirectoryPath.toStdString())) numberOfFiles++;
  
  QLineEdit* FileNameEdit = new QLineEdit;
  FileNameEdit->setObjectName("FileNameEdit");
  FileNameEdit->setText(QString::fromStdString("Unamed "+std::to_string(numberOfFiles)+".gpg"));
  connect(FileNameEdit,&QLineEdit::returnPressed, [this,FileNameEdit,DirectoryPath](){
    QString FullPath = DirectoryPath+"/"+FileNameEdit->text();
    QFileInfo fi(FullPath);
    DataFile *newFileElement = new DataFile;
    newFileElement->path = FullPath;
    newFileElement->fileName = fi.fileName();
    loadedFiles.push_back(newFileElement);
    createFileButton(newFileElement);
    savePasswordFile(newFileElement);
    FileNameEdit->deleteLater();
    EnableEditContent();
    std::ofstream newFile(FullPath.toStdString());
    newFile.close();
  });

  LeftLayoutHolder->addWidget(FileNameEdit);
}


void MainWindow::EnableEditContent(){
  if(currentlyLoadedFile == nullptr) return;

  for(size_t i=0;i<editLineVector.size();i++){
    QLineEdit* UserNameElement = qobject_cast<QLineEdit*>(editLineVector[i][1]);
    QLineEdit* PassElement = qobject_cast<QLineEdit*>(editLineVector[i][2]);
    UserNameElement->setReadOnly(false);
    PassElement->setReadOnly(false);
  }
  editContent->setIcon(QPixmap("icons/save.png"));
  QPushButton* cancelButton = new QPushButton;
  cancelButton->setIcon(QPixmap("icons/cancel.png"));
  cancelButton->setIconSize(QSize(24,24));
  cancelButton->setObjectName("cancelButton");
  TopRightLayout->addWidget(cancelButton);
  disconnect(editContent,nullptr,nullptr,nullptr);
  connect(cancelButton,&QPushButton::clicked,this,&MainWindow::CancelContentEdit);
  connect(editContent,&QPushButton::clicked,this,&MainWindow::SaveEditedContent);
}

void MainWindow::SaveEditedContent(){
  std::stringstream sstr;
  for(size_t i=0;i<editLineVector.size();i++){
    QLabel* ServiceNameElement = qobject_cast<QLabel*>(editLineVector[i][0]);
    QLineEdit* UserNameElement = qobject_cast<QLineEdit*>(editLineVector[i][1]);
    QLineEdit* PassElement = qobject_cast<QLineEdit*>(editLineVector[i][2]);

    UserNameElement->setReadOnly(true);
    PassElement->setReadOnly(true);

    std::string Type = ServiceNameElement->text().toStdString();
    std::string NewUserName = UserNameElement->text().toStdString();
    std::string NewPassword = PassElement->text().toStdString();
    sstr<<Type<<":"<<NewUserName<<"\nPassword:"<<NewPassword<<"\n";
  }
  QString qFormatedString = QString::fromStdString(sstr.str());

  int status = -1;
  int counter = 0;
  while(status == -1){
    QString Password;
    if(counter == 0){
      Password = GetPasswordInput();
    }else{
      Password = GetPasswordInput("Wrong Password");
    }
    if(Password.isEmpty()) return;
    status = EncryptingViaPython(currentlyLoadedFile->path, Password, sstr.str());
    counter++;
  }

  std::vector <PasswordItem> FileContent = fetchContent(qFormatedString);
  currentlyLoadedFile->FileContent = FileContent;
  createFileContentElements(currentlyLoadedFile); 

  QPushButton* thisButton = qobject_cast<QPushButton*>(sender());
  thisButton->setIcon(QPixmap("icons/editIcon.png"));
  disconnect(thisButton,nullptr,nullptr,nullptr);
  connect(thisButton,&QPushButton::clicked,this,&MainWindow::EnableEditContent);
  QPushButton* cancelButton = this->findChild<QPushButton*>("cancelButton");
  delete cancelButton;
}

void MainWindow::CancelContentEdit(){
  QPushButton* cancelButton = this->findChild<QPushButton*>("cancelButton");
  QPushButton* editButton = this->findChild<QPushButton*>("editContent");
  if(editButton == nullptr || cancelButton == nullptr) return;

  createFileContentElements(currentlyLoadedFile);
  editButton->setIcon(QPixmap("icons/editIcon.png"));
  disconnect(editButton,nullptr,nullptr,nullptr);
  connect(editButton,&QPushButton::clicked,this,&MainWindow::EnableEditContent);
  delete cancelButton;
}

void MainWindow::NewPasswordElement(){
  if(currentlyLoadedFile == nullptr) return;
  NewElementWindow win;
  win.exec();

  if((win.ServiceName.isEmpty() && win.UserName.isEmpty() && win.Password.isEmpty())) return;
  PasswordItem newPasswordItem;
  newPasswordItem.ServiceName = win.ServiceName;
  newPasswordItem.userName = win.UserName;
  newPasswordItem.password = win.Password;
  currentlyLoadedFile->FileContent.push_back(newPasswordItem);
  createFileContentElements(currentlyLoadedFile);
  EnableEditContent();
}

void MainWindow::createFileButton(DataFile* File){
  QPushButton *newFileButton = new QPushButton;
  newFileButton->setText(File->fileName);
  newFileButton->setObjectName("FileButtons");
  connect(newFileButton, &QPushButton::clicked, [this,File](){
    clearALayout(RightEnternelHolderLayout);
    CancelContentEdit();
    loadFile(File); 
  });
  LeftLayoutHolder->addWidget(newFileButton);
}

void MainWindow::loadFile(DataFile* File){
  if(File->FileContent.size()){
    createFileContentElements(File); 
    return;
  }

  QString inputedPassword = GetPasswordInput();
  if(!inputedPassword.isEmpty()){
    QString DecryptionResults = DecryptingViaPython(File->path, inputedPassword);
    while(DecryptionResults == "Failed" || DecryptionResults == "Base64 Decode Failed" || DecryptionResults == "Decryption Failed"){
      inputedPassword = GetPasswordInput("Wrong Password");
      DecryptionResults = DecryptingViaPython(File->path, inputedPassword);
      if(inputedPassword.isEmpty()) return;
    }
    std::vector <PasswordItem> FileContent = fetchContent(DecryptionResults);
    File->FileContent = FileContent;
    createFileContentElements(File); 
    CancelContentEdit();
  }
}

void MainWindow::savePasswordFile(DataFile* File){
  std::ofstream file("cache/save", std::ios_base::app);
  if(file){
    file << File->fileName.toStdString() << "," << File->path.toStdString()<<"\n";
    file.close();
  }
}
void MainWindow::loadPasswordFile(){
  std::string line;
  std::ifstream file("cache/save");
  while(std::getline(file, line)){
    if(line.size()){
      DataFile *newFile = new DataFile;
      QString NewFilePath = QString::fromStdString(line.substr(line.find(",")+1));
      //check if the file exist
      std::ifstream file(NewFilePath.toStdString());
      if(file){ 
        QString NewFileName = QString::fromStdString(line.substr(0,line.find(",")));
        newFile->fileName = NewFileName;
        newFile->path = NewFilePath;
        loadedFiles.push_back(newFile);
      }
    }
  }
}

QString MainWindow::GetPasswordInput(QString responce){
  InputWindow inWin(nullptr,responce);
  inWin.exec();
  QString inputedPassword = inWin.Password;
  return inputedPassword ;
} 

int MainWindow::EncryptingViaPython(QString FilePath, QString Password, std::string newContent){
  const char* WritingCommand = "python3 encrypt.py encrypt";
  FILE* pipe = popen(WritingCommand, "w");
  if(!pipe){
    std::cerr<<"Failed to Open Write Pipe\n";
    return -1;
  }
  std::string filepathStd = FilePath.toStdString();
  std::string passwordStd = Password.toStdString();

  fwrite(filepathStd.c_str(),1, filepathStd.size(),pipe);
  fwrite("\n", 1, 1, pipe);

  fwrite(passwordStd.c_str(),1, passwordStd.size(),pipe);
  fwrite("\n", 1, 1, pipe);
  fwrite(newContent.c_str(),1, newContent.size(),pipe);
  fwrite("\n", 1, 1, pipe);

  fflush(pipe);
  pclose(pipe);

  const char* ReadingCommand = "python3 encrypt.py CheckState";
  FILE* readingPipe = popen(ReadingCommand,"r");
  if(!readingPipe){
    std::cerr<<"Failed to Open Read Pipe\n";
    return -1;
  }
  
  std::array <char, 128> buffer;
  std::string results;
  
  while(fgets(buffer.data(), buffer.size(), readingPipe) != nullptr){
    results += buffer.data();
  }
  pclose(readingPipe);
  std::cout<<results<<"\n";
  return (results == "Wrong Password" ? -1: 0);
}

QString MainWindow::DecryptingViaPython(QString FilePath, QString Password){
  const char* WritingCommand = "python3 decrypt.py write";
  FILE* pipe = popen(WritingCommand, "w");
  if(!pipe){
    std::cerr<<"Failed to Open Pipe\n";
    return QString("Failed");
  }
  std::string filePathStd =  FilePath.toStdString();
  std::string PasswordStd =  Password.toStdString();

  fwrite(filePathStd.c_str(),1,filePathStd.size(),pipe);
  fwrite("\n", 1, 1, pipe);
  fwrite(PasswordStd.c_str(),1,PasswordStd.size(),pipe);
  fwrite("\n", 1, 1, pipe);

  fflush(pipe);

  pclose(pipe);

  const char* ReadingCommand = "python3 decrypt.py read";
  std::array<char, 128> buffer;
  FILE* read_pipe = popen(ReadingCommand, "r");
  if(!read_pipe){
    std::cerr<<"Failed to Open a Read Pipe\n";
    return QString("Failed");
  }
  std::string results;

  while(fgets(buffer.data(), buffer.size(), read_pipe) != nullptr){
    results += buffer.data();
  }

  pclose(read_pipe);

  return QString::fromStdString(results);
}

std::vector <PasswordItem> MainWindow::fetchContent(QString FileOutput){
  std::vector <PasswordItem> FileContent;
  std::string FileContentStd = FileOutput.toStdString();
  std::stringstream ss(FileContentStd);
  std::string line;
  char splitChar = '\n';
  while(getline(ss, line, splitChar)){
    if(line.find(":") != std::string::npos){
      if(line.find("Password") == std::string::npos && line.find("password") == std::string::npos){
        std::string ServiceName = line.substr(0,line.find(":")); 
        std::string OriginalUserName = line.substr(line.find(":")+1);
        std::string UserName; 
        size_t start;
        for(start=0;start<OriginalUserName.size() && OriginalUserName[start] == ' ';start++);
        UserName = OriginalUserName.substr(start);

        PasswordItem newPasswordItem;
        newPasswordItem.ServiceName = QString::fromStdString(ServiceName);
        newPasswordItem.userName = QString::fromStdString(UserName);
        FileContent.push_back(newPasswordItem);
      }else{
        std::string password = line.substr(line.find(":")+1);
        FileContent[FileContent.size()-1].password = QString::fromStdString(password);
      }
    }
  }
  return FileContent;
}


void MainWindow::createFileContentElements(DataFile* File){
  clearALayout(RightEnternelHolderLayout);
  editLineVector.clear();
  lineEditElementsHolders.clear();
  RightEnternelHolderLayout->setAlignment(Qt::AlignTop);

  RigthScrollArea->setWidget(widgetholder);
  RigthScrollArea->setWidgetResizable(true);
  RigthScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
  widgetholder->setLayout(RightEnternelHolderLayout);
  RightLayout->addWidget(RigthScrollArea);
  widgetholder->setObjectName("CentralWidgetHolder");

  for(PasswordItem Item:File->FileContent){
    QWidget* holderwidget = new QWidget;
    QHBoxLayout* layoutholder = new QHBoxLayout;
    QVBoxLayout* ElementLayout = new QVBoxLayout;
    QVBoxLayout* FarRightLayout = new QVBoxLayout;

    QHBoxLayout* TopLayout = new QHBoxLayout;
    QHBoxLayout* BottomLayout = new QHBoxLayout;

    QLabel* serviceNameLabel = new QLabel;
    serviceNameLabel->setText(Item.ServiceName);
    serviceNameLabel->setObjectName("leftSideLabelsInPasswordElement");
    QLineEdit* UserName = new QLineEdit;
    UserName->setText(Item.userName);
    UserName->setReadOnly(true);

    QLabel* passwordLabel = new QLabel;
    passwordLabel->setObjectName("leftSideLabelsInPasswordElement");
    passwordLabel->setText("Password");
    QLineEdit* Password = new QLineEdit;
    Password->setText(Item.password);
    Password->setReadOnly(true);
    Password->setEchoMode(QLineEdit::Password);

    QPushButton* togglePasswordVisibility = new QPushButton;
    togglePasswordVisibility->setObjectName("togglePasswordVisibility");
    togglePasswordVisibility->setIcon(QPixmap("icons/display.png"));
    togglePasswordVisibility->setIconSize(QSize(20,20));
    connect(togglePasswordVisibility, &QPushButton::clicked, [this, Password,togglePasswordVisibility](){
      if(Password->echoMode() == QLineEdit::Password){
        Password->setEchoMode(QLineEdit::Normal);
        togglePasswordVisibility->setIcon(QPixmap("icons/hide.png"));
      }
      else{
        Password->setEchoMode(QLineEdit::Password);
        togglePasswordVisibility->setIcon(QPixmap("icons/display.png"));
      }
    });

    QPushButton* deleteButton = new QPushButton;
    deleteButton->setObjectName("deleteButton");
    deleteButton->setIcon(QPixmap("icons/trash.png"));
    deleteButton->setIconSize(QSize(20,20));
    connect(deleteButton, &QPushButton::clicked, [this,Item](){
      DeleteItem(Item);
      EnableEditContent();
    });
    
    QPushButton* NewRandomPassword = new QPushButton;
    NewRandomPassword->setObjectName("NewRandomPassword");
    NewRandomPassword->setIcon(QPixmap("icons/random.png")); 
    NewRandomPassword->setIconSize(QSize(20,20));
    connect(NewRandomPassword,&QPushButton::clicked,[this,Item](){
        RandomizePassword(Item);
    });
    TopLayout->addWidget(serviceNameLabel);
    TopLayout->addWidget(UserName);
    
    BottomLayout->addWidget(passwordLabel);
    BottomLayout->addWidget(Password);
    BottomLayout->addWidget(togglePasswordVisibility);

    ElementLayout->addLayout(TopLayout);
    ElementLayout->addLayout(BottomLayout);

    FarRightLayout->addWidget(deleteButton);
    FarRightLayout->addWidget(NewRandomPassword);

    layoutholder->addLayout(ElementLayout);
    layoutholder->addLayout(FarRightLayout);

    holderwidget->setLayout(layoutholder);

    RightEnternelHolderLayout->addWidget(holderwidget);

    editLineVector.push_back({serviceNameLabel,UserName,Password});
    lineEditElementsHolders.push_back(holderwidget);
  }
  currentlyLoadedFile = File;
}

void MainWindow::DeleteItem(const PasswordItem Item){
  for(size_t i=0;i<currentlyLoadedFile->FileContent.size();i++){
    if(Item.ServiceName == currentlyLoadedFile->FileContent[i].ServiceName && Item.userName == currentlyLoadedFile->FileContent[i].userName && Item.password == currentlyLoadedFile->FileContent[i].password){
      currentlyLoadedFile->FileContent.erase(currentlyLoadedFile->FileContent.begin()+i);
    }
  }
  createFileContentElements(currentlyLoadedFile);
}

void clearALayout(QLayout* layout){
  while(QLayoutItem* item = layout->takeAt(0)){
    if(QLayout* childLayout = item->layout()) clearALayout(childLayout);
    if(QWidget* childWidget = item->widget()) delete childWidget;

  delete item;
  }
}

void MainWindow::EditLineChanged(){
  QString text = SearchBar->text();
  for(size_t i=0;i<lineEditElementsHolders.size();i++){
    QWidget* element = lineEditElementsHolders[i];
    if(currentlyLoadedFile->FileContent[i].ServiceName.toLower().contains(text.toLower())) element->show();
    else element->hide();
  }
}

void MainWindow::RandomizePassword(PasswordItem Item){
  for(size_t i=0;i<currentlyLoadedFile->FileContent.size();i++){
    if(currentlyLoadedFile->FileContent[i].ServiceName == Item.ServiceName &&
        currentlyLoadedFile->FileContent[i].userName == Item.userName &&
        currentlyLoadedFile->FileContent[i].password == Item.password){

      std::string pass = GeneratePassword(); 
      QString formatedNewPassword = QString::fromStdString(pass);
      QLineEdit* PassElement = qobject_cast<QLineEdit*>(editLineVector[i][2]);
      PassElement->setText(formatedNewPassword);
      if(PassElement->isReadOnly()) EnableEditContent();
      break;
    }
  }
}

