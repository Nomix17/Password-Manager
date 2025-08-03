#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class InputWindow:public QDialog{
  Q_OBJECT

  public:
  QString Password = "";

  InputWindow(QDialog* parent ,QString responce=" "):QDialog(parent){
    this->setFixedSize(800,400);
    QVBoxLayout *MainLayout = new QVBoxLayout;
    QLabel* passwordLabel = new QLabel;
    passwordLabel->setObjectName("passwordLabel");
    passwordLabel->setText("Please Enter Your Password:");
    QLineEdit *PasswordInput = new QLineEdit;
    QLabel *WrongPassLabel = new QLabel(responce);
    WrongPassLabel->setObjectName("somethingMissingLabel");
    QPushButton *OkButton = new QPushButton("OK");
    OkButton->setObjectName("okButton");
    connect(OkButton, &QPushButton::clicked,[this,PasswordInput](){
      Password = PasswordInput->text();
      if(!Password.isEmpty()) QDialog::accept();
    });
    MainLayout->addWidget(passwordLabel);
    MainLayout->addWidget(PasswordInput);
    MainLayout->addWidget(WrongPassLabel);
    MainLayout->addStretch();
    MainLayout->addWidget(OkButton,0,Qt::AlignHCenter);
    setLayout(MainLayout);
  }
};
