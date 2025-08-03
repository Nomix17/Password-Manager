#include <QApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>

typedef struct PasswordItem{
  QString ServiceName;
  QString userName;
  QString password;
} PasswordItem;

typedef struct DataFile{
  QString fileName;
  QString path;
  std::vector <PasswordItem> FileContent;
} DataFile;

class MainWindow: public QMainWindow{
  Q_OBJECT
  
  public:
  MainWindow(QWidget* parentWidget = nullptr);
  void getFile();
  void EnableEditContent();
  void SaveEditedContent();
  void CancelContentEdit();
  void NewPasswordElement();
  void loadFile(DataFile* File);
  void savePasswordFile(DataFile* File);
  void loadPasswordFile();
  void createFileButton(DataFile* File);
  QString GetPasswordInput(QString responce="");
  int EncryptingViaPython(QString FilePath,QString Password, std::string newContent="Just a Drill");
  QString DecryptingViaPython(QString FilePath, QString Password);
  std::vector <PasswordItem> fetchContent(QString FileOutput);
  void createFileContentElements(DataFile* File);
  void DeleteItem(const PasswordItem Item);
  void EditLineChanged();
  void RandomizePassword(PasswordItem Item);

  private:
  QWidget * MainCentralWidget; 
  QVBoxLayout *MainLayout; 
  QHBoxLayout *TopLayout; 
  QHBoxLayout *RightLayout;
  QVBoxLayout *LeftLayout; 
  QVBoxLayout *MainContentLayout;
  //right side
  QVBoxLayout* MainLayoutHolder;
  QScrollArea* RigthScrollArea;
  QWidget* widgetholder;
  QLineEdit *SearchBar;
 
  //top 
  QPushButton *editContent;
  QPushButton *loadFileBtn;
  QPushButton *addNewPasswordElement;

  QScrollArea* LeftScrollArea;
  QWidget* LeftWidgetHolder;
  QVBoxLayout* LeftLayoutHolder;

  std::vector<DataFile*> loadedFiles; 
  std::vector<std::vector<QWidget*>> editLineVector;
  std::vector<QWidget*> lineEditElementsHolders;
  DataFile* currentlyLoadedFile = nullptr;
};
