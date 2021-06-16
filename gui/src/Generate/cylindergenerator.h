#ifndef CylinderGenerator_H
#define CylinderGenerator_H

#include <QDialog>
#include <iostream>
#include <QFileDialog>

#include "puma.h"

#include "../src/Utilities/viewer.h"

namespace Ui {
class CylinderGenerator;
}

class CylinderGenerator : public QDialog
{
    Q_OBJECT

public:
    explicit CylinderGenerator(QWidget *parent = nullptr);
    void setViewer(Viewer *view);
    ~CylinderGenerator();

private slots:
    void on_Generate_clicked();

private:
    Ui::CylinderGenerator *ui;
    Viewer *view;


};

#endif // CylinderGenerator_H
