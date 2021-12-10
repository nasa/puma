#ifndef RANDOMFIBERSGENCURVEDFLOWER_H
#define RANDOMFIBERSGENCURVEDFLOWER_H

#include <QDialog>
#include <QLabel>
#include <QPixmap>

#include "puma.h"
#include "../src/Utilities/viewer.h"

namespace Ui {
class RandomFibersGenCurvedFlower;
}

class RandomFibersGenCurvedFlower : public QDialog
{
    Q_OBJECT

public:
    explicit RandomFibersGenCurvedFlower(QWidget *parent = nullptr);
    void setViewer(Viewer *view);
    ~RandomFibersGenCurvedFlower();

private slots:
    void on_Generate_clicked();

    void on_hollowCheckBox_clicked(bool checked);

    void on_angle_combo_currentIndexChanged(int index);

private:
    Ui::RandomFibersGenCurvedFlower *ui;
    Viewer *view;

};

#endif // RANDOMFIBERSGENCURVEDFLOWER_H
