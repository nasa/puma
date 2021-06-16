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

    void setParameters(puma::Workspace *workspace,QLabel *label, QLabel *sliceLabel, QSlider *slider,QPixmap *pixmap);

    puma::Workspace *workspace;
    QLabel *label;
    QSlider *slider;
    QPixmap *pixmap;
    QLabel *sliceLabel;

    QMessageBox *msgBox; //msg

    int currentSlice;

    void setTomographySlice(int slice);
    void refresh();
    void error(std::string m);
    void warning(std::string m);
    void wait(std::string m);

};

#endif // VIEWER_H
