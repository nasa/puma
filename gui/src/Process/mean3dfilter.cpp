#include "mean3dfilter.h"
#include "ui_mean3dfilter.h"

Mean3DFilter::Mean3DFilter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Mean3DFilter)
{
    ui->setupUi(this);
    numThreads = 0;
}

Mean3DFilter::~Mean3DFilter()
{
    delete ui;
}

void Mean3DFilter::setViewer(Viewer *view) {
    this->view = view;
}


void Mean3DFilter::on_previewButton_clicked()
{

    int radius = ui->radiusEdit->text().toInt();
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


    puma::filter_Mean3D(&tmpWorkspace,radius, numThreads);


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

void Mean3DFilter::on_originalButton_clicked()
{
    view->setTomographySlice(view->currentSlice);
}

void Mean3DFilter::on_applyButton_clicked()
{
    int window = ui->radiusEdit->text().toInt();

    if( puma::filter_Mean3D(view->workspace,window,0) ) {
        view->setTomographySlice(view->currentSlice);
    }


}
