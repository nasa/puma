#ifndef VIEWER_H
#define VIEWER_H

#include "puma.h"

#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QSlider>
#include <QMessageBox>

class Viewer
{
public:
    Viewer();
    ~Viewer();

    void setParameters(puma::Workspace *workspace,QLabel *label, QLabel *sliceLabel, QSlider *slider,QPixmap *pixmap, QLabel *x_label, QLabel *y_label, QLabel *z_label, QLabel *gray_min, QLabel *gray_max);

    puma::Workspace *workspace;
    QLabel *label;
    QSlider *slider;
    QPixmap *pixmap;
    QLabel *sliceLabel;
    QLabel *x_label;
    QLabel *y_label;
    QLabel *z_label;
    QLabel *gray_min;
    QLabel *gray_max;
    double min, max;

    QMessageBox *msgBox; //msg

    int currentSlice;

    void updateSize();
    void setTomographySlice(int slice);
    void refresh();
    void error(std::string m);
    void warning(std::string m);
    void wait(std::string m);
    void updatePixmap();

};

#endif // VIEWER_H
