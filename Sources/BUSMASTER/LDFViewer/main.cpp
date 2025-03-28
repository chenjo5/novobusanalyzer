#include <QtWidgets/QApplication>

#include "ldfviewer.h"

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  QString strFileName = "";

  if (argc > 1) {
    strFileName = argv[1];
  }

  LDFViewer w(nullptr, strFileName);

  w.showMaximized();

  return a.exec();
}