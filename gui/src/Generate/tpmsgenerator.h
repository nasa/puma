#ifndef TPMSGENERATOR_H
#define TPMSGENERATOR_H

#include <QDialog>
#include <iostream>
#include <QFileDialog>

#include "puma.h"
#include "../src/Utilities/viewer.h"

namespace Ui {
class tpmsgenerator;
}

class TPMSGenerator : public QDialog
{
    Q_OBJECT

public:
    explicit TPMSGenerator(QWidget *parent = 0);
    void setViewer(Viewer *view);
    ~TPMSGenerator();

private slots:
    void on_Generate_clicked();

private:
    Ui::tpmsgenerator *ui;
    Viewer *view;

};

#endif // TPMSGENERATOR_H
