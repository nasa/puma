#ifndef THRESHOLD_H
#define THRESHOLD_H

#include <QDialog>
#include <QLabel>

#include "puma.h"
#include "../src/Utilities/viewer.h"

namespace Ui {
class Threshold;
}

class Threshold : public QDialog
{
    Q_OBJECT

public:
    explicit Threshold(QWidget *parent = 0);
    void setViewer(Viewer *view);
    ~Threshold();
    void histogram();

private slots:

    void on_Boundary1_Edit_textChanged(const QString &arg1);

    void on_Boundary2_Edit_textChanged(const QString &arg1);

    void on_valueEdit_textChanged(const QString &arg1);

    void on_Boundary1_Slider_valueChanged(int slider1);

    void on_Boundary2_Slider_valueChanged(int slider2);

    void on_pushButton_clicked();


    void on_pushButton_2_clicked();

private:
    Ui::Threshold *ui;

    Viewer *view;

    int Boundary1;
    int Boundary2;
    int value;

    bool ignoreSlider;

    void updateTiff();


};

#endif // THRESHOLD_H
