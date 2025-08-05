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
  void CreateNewPasswordFile();
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
  QWidget * MainCentralWidget; // main central widget
  QHBoxLayout *MainLayout; // main layout (it's horizontal)

  QVBoxLayout *RightLayout; // this layout is gonna contain the scrollbar of the password elements
  QHBoxLayout *TopRightLayout; // top layout it's gonna be added to the right part of teh mainlayout
 
  QVBoxLayout *LeftLayout;  // this layout is gonna contain the scrollbar of the Files elements 
  QHBoxLayout *TopLeftLayout; // top left layout

  //right side
  QScrollArea* RigthScrollArea;
  QWidget* widgetholder;
  QVBoxLayout* RightEnternelHolderLayout;

  //top right
  QPushButton *addNewPasswordElement;
  QLineEdit *SearchBar;
  QPushButton *editContent;
  QWidget* rightWidgetHolder;

  //left
  QScrollArea* LeftScrollArea;
  QWidget* LeftScrollAreaWidgetHolder;
  QVBoxLayout* LeftLayoutHolder;
  QWidget* LeftWidgetHolder;//so I can change the color of the whole layout
   
  //top left
  QPushButton *loadFileBtn;
  QPushButton *CreateNewFile;

  std::vector<DataFile*> loadedFiles; 
  std::vector<std::vector<QWidget*>> editLineVector;
  std::vector<QWidget*> lineEditElementsHolders;
  DataFile* currentlyLoadedFile = nullptr;
};
