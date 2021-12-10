#include "randomfiberstraightcircle.h"
#include "ui_randomfiberstraightcircle.h"
#include <iostream>
#include <QFileDialog>

RandomFiberStraightCircle::RandomFiberStraightCircle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RandomFiberStraightCircle)
{
    ui->setupUi(this);

    ui->variability_edit->setVisible(false);
    ui->variability_label->setVisible(false);
    ui->direction_combo->setVisible(false);
    ui->direction_label->setVisible(false);
}

RandomFiberStraightCircle::~RandomFiberStraightCircle()
{
    delete ui;
}

//passes the view (2D visual of workspace)
void RandomFiberStraightCircle::setViewer(Viewer *view) {
    this->view = view;
}

void RandomFiberStraightCircle::on_angle_combo_currentIndexChanged(int index)
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

// Generates the new workspace using the user entries
void RandomFiberStraightCircle::on_Generate_clicked()
{
    //User inputs for workspace generation
    int xSize = ui->xSizeEdit->text().toInt();
    int  ySize = ui->ySizeEdit->text().toInt();
    int zSize = ui->zSizeEdit->text().toInt();
    double avgRadius = ui->avgRadiusEdit->text().toDouble();
    double radiusDev = ui->radiusDevEdit->text().toDouble();
    double avgLength = ui->avgLengthEdit->text().toDouble();
    double lengthDev = ui->lengthDevEdit->text().toDouble();
    int angleType = ui->angle_combo->currentIndex();
    int var_direction = ui->direction_combo->currentIndex();
    double angle_variation = ui->variability_edit->text().toDouble();
    bool Intersect = ui->Intersect->isChecked();
    double Porosity = ui->porosityEdit->text().toDouble();
    int randomSeed = ui->randSeedEdit->text().toInt();
    double binderRadius = ui->bindRadiusEdit->text().toDouble();
    bool bindFibers = ui->bindCheck->isChecked();

    RandomFibersInput input_straightCircle;
    input_straightCircle.straightCircle(xSize,ySize,zSize,avgRadius,radiusDev,avgLength,lengthDev,angleType,angle_variation,var_direction,Intersect,Porosity,randomSeed);

    if(bindFibers) {
        input_straightCircle.addBinder(binderRadius);
    }


    if ( puma::generateRandomFibers(view->workspace,input_straightCircle) ) {

        if(bindFibers) {
            view->workspace->setMaterialID(puma::Cutoff(2,32000), 255);
            view->workspace->setMaterialID(puma::Cutoff(1,1), 128);
        }

        view->setTomographySlice(0);
        view->slider->setValue(0);
    }


}



