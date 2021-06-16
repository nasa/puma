#ifndef RANDOMSPHERESGENERATOR_H
#define RANDOMSPHERESGENERATOR_H

#include <QDialog>
#include <iostream>
#include <QFileDialog>

#include "puma.h"
#include "../src/Utilities/viewer.h"

namespace Ui {
class RandomSpheresGenerator;
}

class RandomSpheresGenerator : public QDialog
{
    Q_OBJECT

public:
    explicit RandomSpheresGenerator(QWidget *parent = nullptr);
    void setViewer(Viewer *view);
    ~RandomSpheresGenerator();

private slots:
    void on_Generate_clicked();

private:
    Ui::RandomSpheresGenerator *ui;
     Viewer *view;


};

#endif // RANDOMSPHERESGENERATOR_H
