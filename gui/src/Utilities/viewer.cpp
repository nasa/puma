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


void Viewer::setParameters(puma::Workspace *workspace,QLabel *label, QLabel *sliceLabel, QSlider *slider,QPixmap *pixmap) {
    this->workspace = workspace;
    this->label = label;
    this->slider = slider;
    this->pixmap = pixmap;
    this->sliceLabel = sliceLabel;

    this->currentSlice = 0;
}

void Viewer::setTomographySlice(int slice) {

    currentSlice = slice;
    QImage image(workspace->X(),workspace->Y(), QImage::Format_RGB16);

#pragma omp parallel for
    for(int i=0;i<workspace->X();i++) {
        for(int j=0;j<workspace->Y();j++) {
            image.setPixel(i,j, qRgb(workspace->operator ()(i,j,slice),workspace->operator ()(i,j,slice),workspace->operator ()(i,j,slice)));
        }
    }

    pixmap->convertFromImage(image);

    label->setPixmap(pixmap->scaledToHeight(label->height()*0.98));

    sliceLabel->setText(QString::number(currentSlice));
}

void Viewer::refresh(){
    setTomographySlice(currentSlice);
}
