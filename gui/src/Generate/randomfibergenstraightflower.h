#ifndef RANDOMFIBERGENSTRAIGHTFLOWER_H
#define RANDOMFIBERGENSTRAIGHTFLOWER_H

#include <QDialog>
#include <QLabel>
#include <QPixmap>

#include "puma.h"
#include "../src/Utilities/viewer.h"

namespace Ui {
class RandomFiberGenStraightFlower;
}

class RandomFiberGenStraightFlower : public QDialog
{
    Q_OBJECT

public:
    explicit RandomFiberGenStraightFlower(QWidget *parent = nullptr);
    void setViewer(Viewer *view);
    ~RandomFiberGenStraightFlower();

private slots:
    void on_pushButton_clicked();

    void on_hollowCheckBox_clicked(bool checked);

    void on_angle_combo_currentIndexChanged(int index);

private:
    Ui::RandomFiberGenStraightFlower *ui;
    Viewer *view;

};

#endif // RANDOMFIBERGENSTRAIGHTFLOWER_H
