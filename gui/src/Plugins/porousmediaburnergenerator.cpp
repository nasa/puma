#include "porousmediaburnergenerator.h"
#include "ui_porousmediaburnergenerator.h"

PorousMediaBurnerGenerator::PorousMediaBurnerGenerator(QWidget *parent) :
    QDialog(parent), ui(new Ui::PorousMediaBurnerGenerator) {
    ui->setupUi(this);

    editOutputs();
}

//Passes the view
void PorousMediaBurnerGenerator::setViewer(Viewer *view) {
    this->view = view;
}

PorousMediaBurnerGenerator::~PorousMediaBurnerGenerator() {
    delete ui;
}

void PorousMediaBurnerGenerator::on_Generate_clicked() {

    double diameter = ui->diameterEdit->text().toDouble(); // in mm
    double height = ui->heightEdit->text().toDouble(); // in mm
    double p = ui->pEdit->text().toDouble();
    double q = ui->qEdit->text().toDouble();
    bool circle = ui->circleCheck->isChecked();
    bool binaryTiff = ui->binaryCheck->isChecked();
    int equationNumber = ui->equationCombo->currentIndex(); // equation 0, 1 or 2
    int scale = ui->scaleEdit->text().toDouble(); // keep between 5-20

    if(scale == 0) {
        ui->errorLabel->setText("Invalid Scale");
        return;
    }
    if(diameter <= 0) {
        ui->errorLabel->setText("Invalid Diameter");
        return;
    }
    if(height <= 0) {
        ui->errorLabel->setText("Invalid Height");
        return;
    }

    int voxelsX = diameter * scale;
    int voxelsY = diameter * scale;
    int voxelsZ = height * scale;

    double radius = diameter * scale / 2.0;
    double p_scaled = p / scale;

    TPMSInput input(voxelsX,voxelsY,voxelsZ,p_scaled,p_scaled,q,q, equationNumber);

    if( puma::generateTPMS(view->workspace, input) ) {
        if(circle){
            for(int i=0;i<view->workspace->X();i++){
                for(int j=0;j<view->workspace->Y();j++){
                    for(int k=0;k<view->workspace->Z();k++){
                        if(std::sqrt((i-view->workspace->X()/2.0)*(i-view->workspace->X()/2.0) + (j-view->workspace->Y()/2.0)*(j-view->workspace->Y()/2.0)) > radius ){
                            view->workspace->matrix(i,j,k) = 0;
                        }
                    }
                }
            }
        }

        if(binaryTiff) {
            view->workspace->setMaterialID(puma::Cutoff(0,127),0);
            view->workspace->setMaterialID(puma::Cutoff(128,255),255);
        }

        view->setTomographySlice(0);
        view->slider->setValue(0);

        ui->errorLabel->setText("");

    }

    this->repaint();


}

void PorousMediaBurnerGenerator::editOutputs() {
    if(ui->diameterEdit->text()=="" || ui->heightEdit->text()=="" || ui->scaleEdit->text()=="" ) {
        return;
    }

    double diameter = ui->diameterEdit->text().toDouble(); // in mm
    double height = ui->heightEdit->text().toDouble(); // in mm
    int scale = ui->scaleEdit->text().toDouble(); // keep between 5-20

    int voxelsX = diameter * scale;
    int voxelsY = diameter * scale;
    int voxelsZ = height * scale;

    double voxelLength = 1.0/scale;

    ui->xDomainEdit->setText(QString::number(voxelsX));
    ui->yDomainEdit->setText(QString::number(voxelsY));
    ui->zDomainEdit->setText(QString::number(voxelsZ));
    ui->voxelEdit->setText(QString::number(voxelLength));

    this->repaint();

}

void PorousMediaBurnerGenerator::on_diameterEdit_textChanged(const QString &arg1) {
    editOutputs();
}

void PorousMediaBurnerGenerator::on_heightEdit_textChanged(const QString &arg1) {
    editOutputs();
}

void PorousMediaBurnerGenerator::on_scaleEdit_textChanged(const QString &arg1) {
    editOutputs();
}
