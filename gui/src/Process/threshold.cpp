#include "threshold.h"
#include "ui_threshold.h"
#include <QVector>
#include <sstream>

Threshold::Threshold(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Threshold)
{
    ui->setupUi(this);

    Boundary1 = 0;
    Boundary2 = 0;
    value = 0;

    ignoreSlider = false;

}

Threshold::~Threshold()
{
    delete ui;
}


void Threshold::setViewer(Viewer *view) {
    this->view = view;
}


void Threshold::histogram(){

    QVector<double> x(256), y(256);

    std::vector<double> hist(256);
    for(long i=0;i<(long)(*view->workspace).size();i+=10){
        hist[(int)(*view->workspace).matrix.at(i)]++;
    }

    for(int i=0;i<(int)hist.size();i++){
        hist[i]=hist[i]/(*view->workspace).size();
    }

    double maxY= hist[0];
    int maxYValue=0;
    long averageValue=0;

    for(int i=0;i<(int)hist.size();i++){
        if(hist[i]>maxY){
            maxY=hist[i];
            maxYValue=i;
        }
        averageValue+=hist[i]*i;
    }

    if(maxYValue==0){
        hist[0]=0;
        maxY=0;
        for(int i=0;i<(int)hist.size();i++){
            if(hist[i]>maxY){
                maxY=hist[i];
                maxYValue=i;
            }
            averageValue+=hist[i]*i;
        }
    }

    for (int i=0; i<x.size(); i++)
    {
        x[i] = i;
        y[i] = hist[i];
    }

    ui->grayPlot->addGraph();
    ui->grayPlot->graph(0)->setData(x, y);

    ui->grayPlot->xAxis->setRange(0, 255);
    ui->grayPlot->yAxis->setRange(0, maxY);
    ui->grayPlot->yAxis->setTickLabels(false);
    ui->grayPlot->replot();

    this->repaint();


}

/*//////////////////////////////////////////////////////////
Below are the functions that take the values from the sliders
and turn them into strings. These strings are then n in the
line edit text boxes.

1 known bug: Will only go up to Gscale value of
252
///////////////////////////////////////////////////////////*/

void Threshold::on_Boundary1_Slider_valueChanged(int slider1){

    if( ignoreSlider ) {
        return;
    }

    int bound1 = (slider1/99.0)*255.0;

    QString strVal = QString::number(bound1);
    ui->Boundary1_Edit->setText(strVal);

}

void Threshold::on_Boundary2_Slider_valueChanged(int slider2){

    if( ignoreSlider ) {
        return;
    }

    int bound2 = (slider2/99.0)*255.0;

    QString strVal = QString::number(bound2);
    ui->Boundary2_Edit->setText(strVal);

}
/*//////////////////////////////////////////////////////////
The functions below takes the 1st boundary entered by the user and
finds updates the tiff
///////////////////////////////////////////////////////////*/
void Threshold::on_Boundary1_Edit_textChanged(const QString &arg1)
{


    Boundary1 = arg1.toInt();

    if(Boundary1 > 255) {
        Boundary1 = 255;
        ui->Boundary1_Edit->setText("255");

    }

    if(Boundary1 < 0) {
        Boundary1 = 0;
        ui->Boundary1_Edit->setText("0");
    }

    ignoreSlider = true; // to avoid feedback loop with the slider
    ui->Boundary1_Slider->setValue( (int) ( Boundary1*(100.0/255.0) ) );
    ignoreSlider = false;

    updateTiff();


}
/*//////////////////////////////////////////////////////////
The functions below takes the 2nd boundary entered by the user and
finds updates the tiff
///////////////////////////////////////////////////////////*/
void Threshold::on_Boundary2_Edit_textChanged(const QString &arg1)
{
    Boundary2 = arg1.toInt();

    if(Boundary2 > 255) {
        Boundary2 = 255;
        ui->Boundary2_Edit->setText("255");
    }

    if(Boundary2 < 0) {
        Boundary2 = 0;
        ui->Boundary2_Edit->setText("0");
    }

    ignoreSlider = true;
    ui->Boundary2_Slider->setValue( (int) ( Boundary2*(100.0/255.0) ) );
    ignoreSlider = false;

    updateTiff();

}

void Threshold::updateTiff() {


    if(view->workspace->size()==0) {
        return;
    }

    QImage image((*view->workspace).X(),(*view->workspace).Y(), QImage::Format_RGB16);


    int slice = view->currentSlice;


#pragma omp parallel for
    for(int i=0;i<(*view->workspace).X();i++) {
        for(int j=0;j<(*view->workspace).Y();j++) {

            if((*view->workspace)(i,j,slice) < Boundary1 && (*view->workspace)(i,j,slice) < Boundary2) { //if image grayscale value is below both thresholds
                image.setPixel(i,j, qRgb((*view->workspace)(i,j,slice),(*view->workspace)(i,j,slice),(*view->workspace)(i,j,slice)));
            } else if ((*view->workspace)(i,j,slice) > Boundary1 && (*view->workspace)(i,j,slice) > Boundary2) { //if the image grayscale value is above both thresholds
                image.setPixel(i,j, qRgb((*view->workspace)(i,j,slice),(*view->workspace)(i,j,slice),(*view->workspace)(i,j,slice)));
            } else { // If the value in the pixel is between boundary 1 and boundary 2, then we want to assign it to the new value
                image.setPixel(i,j, qRgb(value,value,value));
            }
        }
    }

    (*view->pixmap).convertFromImage(image);

    view->updatePixmap();
}

void Threshold::on_valueEdit_textChanged(const QString &arg1)
{
    value = arg1.toInt();

    if(value > 255) {
        value = 255;
        ui->valueEdit->setText("255");
    }

    if(value < 0) {
        value = 0;
        ui->valueEdit->setText("0");
    }

    updateTiff();
}

void Threshold::on_pushButton_clicked()
{
#pragma omp parallel for
    for(int i=0;i<(*view->workspace).X();i++) {
        for(int j=0;j<(*view->workspace).Y();j++) {
            for(int k=0;k<(*view->workspace).Z();k++) {
                if((*view->workspace)(i,j,k) < Boundary1 && (*view->workspace)(i,j,k) < Boundary2) { //if image grayscale value is below both thresholds

                } else if ((*view->workspace)(i,j,k) > Boundary1 && (*view->workspace)(i,j,k) > Boundary2) { //if the image grayscale value is above both thresholds

                } else { // If the value in the pixel is between boundary 1 and boundary 2, then we want to assign it to the new value
                    view->workspace->matrix(i,j,k) = value;
                }
            }
        }
    }

}

void Threshold::on_pushButton_2_clicked()
{
    histogram();
}
