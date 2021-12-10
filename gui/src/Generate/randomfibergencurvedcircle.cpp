#include "randomfibergencurvedcircle.h"
#include "ui_randomfibergencurvedcircle.h"
#include <iostream>
#include <QFileDialog>

RandomFiberGenCurvedCircle::RandomFiberGenCurvedCircle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RandomFiberGenCurvedCircle)
{
    ui->setupUi(this);

    ui->variability_edit->setVisible(false);
    ui->variability_label->setVisible(false);
    ui->direction_combo->setVisible(false);
    ui->direction_label->setVisible(false);
}

RandomFiberGenCurvedCircle::~RandomFiberGenCurvedCircle()
{
    delete ui;
}

//Passes the view
void RandomFiberGenCurvedCircle::setViewer(Viewer *view) {
    this->view = view;
}

//When generate button is clicked. Perform these actions
void RandomFiberGenCurvedCircle::on_Generate_clicked()
{
    //These are the domain size variables
    int xSize = ui->xSizeEdit_2->text().toInt();
    int ySize = ui->ySizeEdit_2->text().toInt();
    int zSize = ui->zSizeEdit_2->text().toInt();
    double avgRadius = ui->avgRadiusEdit_2->text().toDouble();
    double radiusDev = ui->radiusDevEdit_2->text().toDouble();
    double avgLength = ui->avgLengthEdit_2->text().toDouble();
    double lengthDev = ui->lengthDevEdit_2->text().toDouble();
    int angleType = ui->angle_combo->currentIndex();
    int var_direction = ui->direction_combo->currentIndex();
    double angle_variation = ui->variability_edit->text().toDouble();
    bool Intersect = ui->Intersect->isChecked();
    double Porosity = ui->porosityEdit_2->text().toDouble();
    int randomSeed = ui->randSeedEdit_2->text().toInt();
    double avgRadiusCurvature = ui->avgRadiusCurvEdit->text().toInt();
    double radiusCurvatureDev = ui->curvDeviationEdit->text().toInt();
    double  Accuracy = ui->accuracyEdit->text().toDouble();
    double binderRadius = ui->bindRadiusEdit->text().toDouble();
    bool bindFibers = ui->bindCheck->isChecked();

    RandomFibersInput input_curvedCircle;
    input_curvedCircle.curvedCircle(xSize,ySize,zSize,avgRadius,radiusDev,avgLength,lengthDev,angleType,angle_variation,var_direction,Intersect,Porosity,randomSeed,avgRadiusCurvature,radiusCurvatureDev,Accuracy);

    if(bindFibers) {
        input_curvedCircle.addBinder(binderRadius);
    }

    if ( puma::generateRandomFibers(view->workspace,input_curvedCircle) ) {

        if(bindFibers) {
            view->workspace->setMaterialID(puma::Cutoff(2,32000), 255);
            view->workspace->setMaterialID(puma::Cutoff(1,1), 128);
        }

        view->setTomographySlice(0);
        view->slider->setValue(0);
    }
}

void RandomFiberGenCurvedCircle::on_angle_combo_currentIndexChanged(int index)
{
    if(index == 0) { //isotropic
        ui->variability_edit->setVisible(false);
        ui->variability_label->setVisible(false);
        ui->direction_combo->setVisible(false);
        ui->direction_label->setVisible(false);
    } else if(index == 1) { // transverse isotropic
        ui->variability_edit->setVisible(true);
        ui->variability_label->setVisible(true);
        ui->direction_combo->setVisible(true);
        ui->direction_label->setVisible(true);
    } else if(index == 2) { // 1D
        ui->variability_edit->setVisible(false);
        ui->variability_label->setVisible(false);
        ui->direction_combo->setVisible(true);
        ui->direction_label->setVisible(true);
    }
}

