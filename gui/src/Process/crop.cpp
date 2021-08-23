#include "crop.h"
#include "ui_crop.h"

Crop::Crop(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Crop)
{
    ui->setupUi(this);
}

Crop::~Crop()
{
    delete ui;
}

void Crop::setViewer(Viewer *view) {
    this->view = view;

}

void Crop::on_applyButton_clicked()
{

    int x1 = ui->x1_Edit->text().toInt();
    int x2 = ui->x2_Edit->text().toInt();
    int y1 = ui->y1_Edit->text().toInt();
    int y2 = ui->y2_Edit->text().toInt();
    int z1 = ui->z1_Edit->text().toInt();
    int z2 = ui->z2_Edit->text().toInt();

    if( view->workspace->crop(x1,x2,y1,y2,z1,z2) ) {
        view->setTomographySlice(0);
        view->slider->setValue(0);
    }

    this->repaint();


}

void Crop::on_x2_Edit_textChanged(const QString &arg1)
{
    updateWindow();
}

void Crop::on_x1_Edit_textChanged(const QString &arg1)
{
    updateWindow();
}

void Crop::on_y2_Edit_textChanged(const QString &arg1)
{
    updateWindow();
}

void Crop::on_y1_Edit_textChanged(const QString &arg1)
{
    updateWindow();
}

void Crop::on_z2_Edit_textChanged(const QString &arg1)
{
    updateWindow();
}

void Crop::on_z1_Edit_textChanged(const QString &arg1)
{
    updateWindow();
}


void Crop::on_previewButton_clicked()
{
    updateWindow();
}


void Crop::updateWindow() {
    int x1 = ui->x1_Edit->text().toInt();
    int x2 = ui->x2_Edit->text().toInt();
    int y1 = ui->y1_Edit->text().toInt();
    int y2 = ui->y2_Edit->text().toInt();


    if(x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0) {
        view->refresh();
        return;
    }

    if(x1 >= view->workspace->X() || x2 >= view->workspace->X() || y1 >= view->workspace->Y() || y2 >= view->workspace->Y() ) {
        view->refresh();
        return;
    }

    if(x1 > x2) {
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }
    if(y1 > y2) {
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }

    int slice = view->currentSlice;
    QImage image(view->workspace->X(),view->workspace->Y(), QImage::Format_RGB16);

#pragma omp parallel for
    for(int i=0;i<view->workspace->X();i++) {
        for(int j=0;j<view->workspace->Y();j++) {
            image.setPixel(i,j, qRgb(view->workspace->operator ()(i,j,slice),view->workspace->operator ()(i,j,slice),view->workspace->operator ()(i,j,slice)));
        }
    }

    int lowX = std::max(0,x1-1);
    int highX = std::min(view->workspace->X()-1,(long)x2+1);
    int lowY = std::max(0,y1-1);
    int highY = std::min(view->workspace->Y()-1,(long)y2+1);



    for(int i=lowX; i<highX; i ++) {
        image.setPixel(i,lowY, qRgb(255,0,0));
        image.setPixel(i,highY, qRgb(255,0,0));
    }
    for(int j=lowY; j<highY; j++) {
        image.setPixel(lowX,j, qRgb(255,0,0));
        image.setPixel(highX,j, qRgb(255,0,0));
    }

    view->pixmap->convertFromImage(image);

    view->updatePixmap();


    this->repaint();


}

/*
 *
 *     currentSlice = slice;

    std::cout << "Displaying Slice " << slice << std::endl;

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
 *
 */
