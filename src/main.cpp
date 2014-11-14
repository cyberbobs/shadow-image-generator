#include "MainWindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
  QApplication a(argc, argv);
  a.setOrganizationName("cyberbobs");
  a.setOrganizationDomain("https://github.com/cyberbobs");
  a.setApplicationName("ShadowImageGenerator");

  MainWindow w;
  w.show();

  return a.exec();
}
