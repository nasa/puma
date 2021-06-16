#include "guiprinter.h"

GUIPrinter::GUIPrinter(QTextEdit *browser, QApplication *myApp)
{
    myBrowser = browser;
    myApp = qApp;
}

void GUIPrinter::print(std::string s) {

    myBrowser->append(QString::fromStdString(s).trimmed());
    myBrowser->repaint();
    myApp->processEvents();

}

void GUIPrinter::error(std::string s) {

    myBrowser->append(QString::fromStdString(s).trimmed());
    myBrowser->repaint();
    myApp->processEvents();

}
