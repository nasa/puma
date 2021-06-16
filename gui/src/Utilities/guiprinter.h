#ifndef GUIPRINTER_H
#define GUIPRINTER_H

#include "puma.h"

#include <string>
#include <QTextBrowser>

#include <QApplication>

class GUIPrinter : public puma::Printer
{
public:
    GUIPrinter(QTextEdit *browser, QApplication *myApp);

    virtual void print(std::string s) override;
    virtual void error(std::string s) override;

private:
    QTextEdit *myBrowser;
    QApplication *myApp;

};

#endif // GUIPRINTER_H
