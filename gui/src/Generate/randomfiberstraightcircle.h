#ifndef RANDOMFIBERSTRAIGHTCIRCLE_H
#define RANDOMFIBERSTRAIGHTCIRCLE_H

#include <QDialog>
#include <QLabel>
#include <QPixmap>

#include "puma.h"
#include "../src/Utilities/viewer.h"


namespace Ui {
class RandomFiberStraightCircle;
}

class RandomFiberStraightCircle : public QDialog
{
    Q_OBJECT

public:
    explicit RandomFiberStraightCircle(QWidget *parent = 0);
    void setViewer(Viewer *view);
    ~RandomFiberStraightCircle();

private slots:

    void on_Generate_clicked();

    void on_angle_combo_currentIndexChanged(int index);

private:
    Ui::RandomFiberStraightCircle *ui;
    Viewer *view;
};

#endif // RANDOMFIBERSTRAIGHTCIRCLE_H
