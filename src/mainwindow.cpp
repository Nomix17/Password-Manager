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

MainWindow::MainWindow(QWidget *parentWidget):QMainWindow(parentWidget){
  MainCentralWidget = new QWidget;
  MainLayout = new QVBoxLayout(MainCentralWidget);

  TopLayout = new QHBoxLayout;
  RightLayout = new QHBoxLayout;

  LeftLayout = new QVBoxLayout;
  MainContentLayout = new QVBoxLayout;

  LeftScrollArea = new QScrollArea;
  LeftWidgetHolder = new QWidget;
  LeftLayoutHolder = new QVBoxLayout;

  SearchBar = new QLineEdit;
  SearchBar->setObjectName("SearchBar");
  connect(SearchBar,&QLineEdit::textChanged, this, &MainWindow::EditLineChanged);

  LeftWidgetHolder->setObjectName("LeftWidgetHolder");
  LeftScrollArea->setWidgetResizable(true);
  LeftScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  LeftWidgetHolder->setLayout(LeftLayoutHolder);
  LeftScrollArea->setWidget(LeftWidgetHolder);
  LeftLayout->addWidget(LeftScrollArea);

  MainLayout->addLayout(TopLayout);
  MainLayout->addLayout(RightLayout);
  MainLayout->setAlignment(Qt::AlignTop);

  RightLayout->addLayout(LeftLayout);
  RightLayout->addLayout(MainContentLayout);
 
  LeftScrollArea->setFixedWidth(200);

  RightLayout->setStretchFactor(MainContentLayout, 1);
  RightLayout->setAlignment(Qt::AlignLeft);
  LeftLayoutHolder->setAlignment(Qt::AlignTop);

  loadFileBtn = new QPushButton;
  loadFileBtn->setObjectName("loadFileBtn");
  
  addNewPasswordElement = new QPushButton;
  addNewPasswordElement->setObjectName("addNewPasswordElement");

  editContent = new QPushButton;
  editContent->setObjectName("editContent");
  
  loadFileBtn->setIcon(QPixmap("icons/loadFileIcon.png"));
  editContent->setIcon(QPixmap("icons/editIcon.png"));
  addNewPasswordElement->setIcon(QPixmap("icons/add.png"));

  loadFileBtn->setIconSize(QSize(24,24));
  editContent->setIconSize(QSize(24,24));
  addNewPasswordElement->setIconSize(QSize(24,24));

  connect(loadFileBtn,&QPushButton::clicked,this,&MainWindow::getFile);
  connect(editContent,&QPushButton::clicked,this,&MainWindow::EnableEditContent);
  connect(addNewPasswordElement,&QPushButton::clicked,this,&MainWindow::NewPasswordElement);

  SearchBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  loadFileBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  editContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

  TopLayout->addWidget(loadFileBtn);
  TopLayout->addWidget(SearchBar);
  TopLayout->addWidget(addNewPasswordElement);
  TopLayout->addWidget(editContent);

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
  TopLayout->addWidget(cancelButton);
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
}

void MainWindow::createFileButton(DataFile* File){
  QPushButton *newFileButton = new QPushButton;
  newFileButton->setText(File->fileName);
  newFileButton->setObjectName("FileButtons");
  connect(newFileButton, &QPushButton::clicked, [this,File](){
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
  }
}

void MainWindow::savePasswordFile(DataFile* File){
  std::ofstream file("cache/save");
  if(file){
    file << File->fileName.toStdString() << "," << File->path.toStdString()<<"\n";
    file.close();
  }
}
void MainWindow::loadPasswordFile(){
  std::string line;
  std::ifstream file("cache/save");
  while(std::getline(file, line)){
    DataFile *newFile = new DataFile;
    newFile->fileName = QString::fromStdString(line.substr(0,line.find(",")));
    newFile->path = QString::fromStdString(line.substr(line.find(",")+1));
    loadedFiles.push_back(newFile);
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

void clearALayout(QLayout* layout);

void MainWindow::createFileContentElements(DataFile* File){
  clearALayout(MainContentLayout);
  editLineVector.clear();
  lineEditElementsHolders.clear();

  RigthScrollArea = new QScrollArea;
  QWidget* widgetholder = new QWidget;
  MainLayoutHolder = new QVBoxLayout;
  MainLayoutHolder->setAlignment(Qt::AlignTop);

  RigthScrollArea->setWidget(widgetholder);
  RigthScrollArea->setWidgetResizable(true);
  RigthScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  widgetholder->setLayout(MainLayoutHolder);
  MainContentLayout->addWidget(RigthScrollArea);
  widgetholder->setObjectName("CentralWidgetHolder");

  for(PasswordItem Item:File->FileContent){
    QWidget* holderwidget = new QWidget;
    QHBoxLayout* layoutholder = new QHBoxLayout;
    QVBoxLayout* ElementLayout = new QVBoxLayout;
    QVBoxLayout* FarRightLayout = new QVBoxLayout;

    QHBoxLayout* TopLayout = new QHBoxLayout;
    QHBoxLayout* RightLayout = new QHBoxLayout;

    QLabel* PasswordType = new QLabel;
    PasswordType->setObjectName("leftSideLabelsInPasswordElement");
    PasswordType->setText(Item.ServiceName);

    QLineEdit* UserName = new QLineEdit;
    UserName->setText(Item.userName);
    UserName->setReadOnly(true);

    QLabel* passwordLabel = new QLabel;
    passwordLabel->setObjectName("leftSideLabelsInPasswordElement");
    passwordLabel->setText("Password");

    QLineEdit* Password = new QLineEdit;
    Password->setText(Item.password);
    Password->setReadOnly(true);   

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
    TopLayout->addWidget(PasswordType);
    TopLayout->addWidget(UserName);
    
    RightLayout->addWidget(passwordLabel);
    RightLayout->addWidget(Password);

    ElementLayout->addLayout(TopLayout);
    ElementLayout->addLayout(RightLayout);

    FarRightLayout->addWidget(deleteButton);
    FarRightLayout->addWidget(NewRandomPassword);

    layoutholder->addLayout(ElementLayout);
    layoutholder->addLayout(FarRightLayout);

    holderwidget->setLayout(layoutholder);

    MainLayoutHolder->addWidget(holderwidget);

    editLineVector.push_back({PasswordType,UserName,Password});
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

