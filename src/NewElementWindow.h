#include "passwordGeneration.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

class NewElementWindow: public QDialog{
  Q_OBJECT
  
  public:
  NewElementWindow(QWidget* parent=nullptr):QDialog(parent){
    this->setFixedSize(800,500);
    QVBoxLayout *MainLayout =  new QVBoxLayout;
    QHBoxLayout *passwordLayout = new QHBoxLayout;
    QHBoxLayout *ADDLayout = new QHBoxLayout;

    QLabel *ServiceNameLabel = new QLabel("Service Name:");
    QLabel *UserNameLabel = new QLabel("User Name: ");
    QLabel *PasswordLabel = new QLabel("Password: ");
    
    ServiceNameLabel->setObjectName("ServiceNameLabel");
    UserNameLabel->setObjectName("UserNameLabel");
    PasswordLabel->setObjectName("PasswordLabel");

    QLineEdit * ServiceNameEdit = new QLineEdit;
    QLineEdit * UserNameEdit = new QLineEdit;
    QPushButton *RandomizePass = new QPushButton;
    QLineEdit * PasswordEdit = new QLineEdit;
   
    QLabel *ServiceNameMissingLabel = new QLabel;
    QLabel *UserNameMissingLabel = new QLabel;
    QLabel *PasswordMissingLabel = new QLabel;

    RandomizePass->setObjectName("RandomizePass");
    RandomizePass->setIcon(QPixmap("icons/random.png")); 
    RandomizePass->setIconSize(QSize(20,20));

    ServiceNameMissingLabel->setObjectName("somethingMissingLabel");
    UserNameMissingLabel->setObjectName("somethingMissingLabel");
    PasswordMissingLabel->setObjectName("somethingMissingLabel");

    QPushButton* ADDButton = new QPushButton("ADD");
    ADDButton->setObjectName("ADDButton");
    ADDButton->setIconSize(QSize(20,20));
    ADDLayout->addWidget(ADDButton);
    ADDLayout->setAlignment(Qt::AlignHCenter);

    MainLayout->addWidget(ServiceNameLabel);
    MainLayout->addWidget(ServiceNameEdit);
    MainLayout->addWidget(ServiceNameMissingLabel);
    MainLayout->addSpacing(10);

    MainLayout->addWidget(UserNameLabel);
    MainLayout->addWidget(UserNameEdit);
    MainLayout->addWidget(UserNameMissingLabel);
    MainLayout->addSpacing(10);

    MainLayout->addWidget(PasswordLabel);
    passwordLayout->addWidget(PasswordEdit);
    passwordLayout->addWidget(RandomizePass);

    MainLayout->addLayout(passwordLayout);
    MainLayout->addWidget(PasswordMissingLabel);

    MainLayout->addStretch();
    MainLayout->addLayout(ADDLayout);
    MainLayout->addStretch();
   
    MainLayout->setContentsMargins(20,20,20,20);

    setLayout(MainLayout);
    
    connect(RandomizePass, &QPushButton::clicked,[this,PasswordEdit](){
      QString pass = QString::fromStdString(GeneratePassword());
      PasswordEdit->setText(pass);
    });

    connect(ADDButton,&QPushButton::clicked,[this,ServiceNameEdit,UserNameEdit,PasswordEdit,ServiceNameMissingLabel,UserNameMissingLabel,PasswordMissingLabel](){
      bool CloseWindow = true;
      ServiceName = ServiceNameEdit->text();
      UserName = UserNameEdit->text();
      Password = PasswordEdit->text();

      ServiceNameMissingLabel->setText(" ");
      UserNameMissingLabel->setText(" ");
      PasswordMissingLabel->setText(" ");

      if(ServiceName.isEmpty()){
        ServiceNameMissingLabel->setText("Enter The Service Name");
        CloseWindow = false;
      }
      if(UserName.isEmpty()){
        UserNameMissingLabel->setText("Enter an Identification (username, email, phone number...)");
        CloseWindow = false;
      }
      if(Password.isEmpty()){
        PasswordMissingLabel->setText("Enter The Password");
        CloseWindow = false;
      }
      if(CloseWindow) QDialog::accept();
    }); 
  }

  QString ServiceName;
  QString UserName;
  QString Password;
};
