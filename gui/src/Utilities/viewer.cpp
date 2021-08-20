#include "viewer.h"

Viewer::Viewer(){

    msgBox = new QMessageBox(QMessageBox::NoIcon,"Test","Test",QMessageBox::Ok);
    msgBox->setModal(false);

}

Viewer::~Viewer() {
    delete msgBox;
}

void Viewer::error(std::string m){
    msgBox->setWindowTitle("Error");
    msgBox->setText(QString::fromStdString(m));
    msgBox->exec();
}

void Viewer::warning(std::string m){
    msgBox->setWindowTitle("Warning");
    msgBox->setText(QString::fromStdString(m));
    msgBox->exec();
}

void Viewer::wait(std::string m){
    msgBox->setWindowTitle("Please Wait");
    msgBox->setText(QString::fromStdString(m));
    msgBox->exec();
}


void Viewer::setParameters(puma::Workspace *workspace,QLabel *label, QLabel *sliceLabel, QSlider *slider,QPixmap *pixmap,QLabel *x_label, QLabel *y_label, QLabel *z_label, QLabel *gray_min, QLabel *gray_max) {
    this->workspace = workspace;
    this->label = label;
    this->slider = slider;
    this->pixmap = pixmap;
    this->sliceLabel = sliceLabel;
    this->x_label = x_label;
    this->y_label = y_label;
    this->z_label = z_label;
    this->gray_min = gray_min;
    this->gray_max = gray_max;
    min = 0;
    max = 255;
    this->currentSlice = 0;
}

void Viewer::updatePixmap() {
    QPixmap scaled_pixmap = pixmap->scaledToHeight(label->height()*0.98);
    if(scaled_pixmap.width() > std::max((int)(label->width() * 0.9), 1920)) {
        scaled_pixmap = pixmap->scaledToWidth(std::max((int)(label->width() * 0.9), 1920));
    }
    label->setPixmap(scaled_pixmap);
}

void Viewer::setTomographySlice(int slice) {

    currentSlice = slice;
    QImage image(workspace->X(),workspace->Y(), QImage::Format_RGB16);

#pragma omp parallel for
    for(int i=0;i<workspace->X();i++) {
        for(int j=0;j<workspace->Y();j++) {
            short val = (uint8_t)((255*(workspace->operator ()(i,j,slice)-min)/(max-min)));
            image.setPixel(i,j, qRgb(val,val,val));
        }
    }

    pixmap->convertFromImage(image);

    updatePixmap();

    updateSize();

    sliceLabel->setText(QString::number(currentSlice));
}

void Viewer::updateSize() {
    x_label->setText(QString::number(workspace->X()));
    y_label->setText(QString::number(workspace->Y()));
    z_label->setText(QString::number(workspace->Z()));
}

void Viewer::refresh(){
    setTomographySlice(currentSlice);
}
