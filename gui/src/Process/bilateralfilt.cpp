#include "bilateralfilt.h"
#include "ui_bilateralfilt.h"

BilateralFilt::BilateralFilt(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BilateralFilt)
{
    ui->setupUi(this);
    numThreads = 0;
}

BilateralFilt::~BilateralFilt()
{
    delete ui;
}

void BilateralFilt::setViewer(Viewer *view) {
    this->view = view;
}

/* /////////////////////////////////////////////////////////////////
This let's user preview filter before making the filter permanent.
////////////////////////////////////////////////////////////////*/
void BilateralFilt::on_previewButton_clicked()
{
    int window = ui->radiusEdit->text().toInt();
    double rangeParam = ui->rangeParameterEdit->text().toDouble();
    double spatialParam = ui->spatialParamEdit->text().toDouble();
    numThreads = ui->numThreadsEdit->text().toInt();


    int X = view->workspace->X();
    int Y = view->workspace->Y();
    int k = view->currentSlice;


    puma::Workspace tmpWorkspace;
    tmpWorkspace.setSize(X,Y,1);

    for(int i=0;i<X;i++){
        for(int j=0;j<Y;j++) {
            tmpWorkspace.matrix.at(i,j,0) = view->workspace->operator ()(i,j,k);
        }
    }


    puma::filter_Bilateral(&tmpWorkspace,window,spatialParam,rangeParam ,numThreads);
    QImage image(X,Y, QImage::Format_RGB16);

#pragma omp parallel for
    for(int i=0;i<X;i++) {
        for(int j=0;j<Y;j++) {
            image.setPixel(i,j, qRgb(tmpWorkspace(i,j,0),tmpWorkspace(i,j,0),tmpWorkspace(i,j,0)));
        }
    }

    view->pixmap->convertFromImage(image);

    view->updatePixmap();

    this->repaint();

}

/* /////////////////////////////////////////////////////////////////
This let's user remove the filter, bringing back the original
////////////////////////////////////////////////////////////////*/
void BilateralFilt::on_originalButton_clicked()
{
view->setTomographySlice(view->currentSlice);
}

/* /////////////////////////////////////////////////////////////////
This let's user permanently apply the filter
////////////////////////////////////////////////////////////////*/

void BilateralFilt::on_applyButton_clicked()
{
    int window = ui->radiusEdit->text().toInt();
    double rangeParam = ui->rangeParameterEdit->text().toDouble();
    double spatialParam = ui->spatialParamEdit->text().toDouble();

    if( puma::filter_Bilateral(view->workspace,window,spatialParam,rangeParam,0) ) {
        view->setTomographySlice(view->currentSlice);
    }

    this->repaint();
}
