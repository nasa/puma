#ifndef RANDOMFIBERGENCURVEDCIRCLE_H
#define RANDOMFIBERGENCURVEDCIRCLE_H

#include <QDialog>
#include <QLabel>
#include <QPixmap>

#include "puma.h"
#include "../src/Utilities/viewer.h"

namespace Ui {
class RandomFiberGenCurvedCircle;
}

class RandomFiberGenCurvedCircle : public QDialog
{
    Q_OBJECT

public:
    explicit RandomFiberGenCurvedCircle(QWidget *parent = nullptr);
    void setViewer(Viewer *view);
    ~RandomFiberGenCurvedCircle();

private slots:
    void on_Generate_clicked();

    void on_angle_combo_currentIndexChanged(int index);

private:
    Ui::RandomFiberGenCurvedCircle *ui;
    Viewer *view;

};

#endif // RANDOMFIBERGENCURVEDCIRCLE_H
