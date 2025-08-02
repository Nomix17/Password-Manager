#include "mainwindow.h"
#include <QApplication>
#include <fstream>

std::string StyleFilePath = "main.css";

int main(int argc, char* argv[]){
  QApplication app(argc, argv);
  std::ifstream file(StyleFilePath);
  if(file){
    std::string script;
    std::stringstream sstr;
    sstr << file.rdbuf();
    script = sstr.str();
    app.setStyleSheet(QString::fromStdString(script));
  }
  MainWindow win;
  win.show();
  return app.exec();
}
