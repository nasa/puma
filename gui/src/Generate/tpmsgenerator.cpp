#include "tpmsgenerator.h"
#include "ui_tpmsgenerator.h"

TPMSGenerator::TPMSGenerator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tpmsgenerator)
{
    ui->setupUi(this);
}

//Passes the view
void TPMSGenerator::setViewer(Viewer *view) {
    this->view = view;
}

TPMSGenerator::~TPMSGenerator()
{
    delete ui;
}

void TPMSGenerator::on_Generate_clicked()
{
    int xSize = ui->xSizeEdit->text().toInt();
    int ySize = ui->ySizeEdit->text().toInt();
    int zSize = ui->zSizeEdit->text().toInt();
    double wmin = ui->wMinEdit->text().toDouble();
    double qmin = ui->qMinEdit->text().toDouble();

    double wmax = wmin;
    double qmax = qmin;

    if(ui->wMaxEdit->text() != "") {
        wmax = ui->wMaxEdit->text().toDouble();
    } else {

    }
    if(ui->qMaxEdit->text() != "") {
        qmax = ui->qMaxEdit->text().toDouble();
    }

    TPMSInput input(xSize, ySize, zSize, wmin, wmax, qmin, qmax, 0);


    if ( puma::generateTPMS(view->workspace,input) ) {
        view->setTomographySlice(0);
        view->slider->setValue(0);
    }


}
