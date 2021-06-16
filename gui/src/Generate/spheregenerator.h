#ifndef SphereGenerator_H
#define SphereGenerator_H

#include <QDialog>
#include <iostream>
#include <QFileDialog>

#include "puma.h"

#include "../src/Utilities/viewer.h"

namespace Ui {
class spheregenerator;
}

class SphereGenerator : public QDialog
{
    Q_OBJECT

public:
    explicit SphereGenerator(QWidget *parent = nullptr);
    void setViewer(Viewer *view);
    ~SphereGenerator();

private slots:
    void on_Generate_clicked();

private:
    Ui::spheregenerator *ui;
    Viewer *view;


};

#endif // SphereGenerator_H
