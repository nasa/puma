#include "randomfibersgencurvedflower.h"
#include "ui_randomfibersgencurvedflower.h"
#include "../src/Utilities/viewer.h"
#include <iostream>
#include <QFileDialog>

RandomFibersGenCurvedFlower::RandomFibersGenCurvedFlower(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RandomFibersGenCurvedFlower)
{
    ui->setupUi(this);
    ui->hollowRadiusDevEdit->setVisible(false);
    ui->hollowRadiusDevLabel->setVisible(false);
    ui->avgHollowRadiusLabel->setVisible(false);
    ui->avgHollowRadiusEdit->setVisible(false);
    ui->hollowDimLabel->setVisible(false);
    ui->fracFiberLabel->setVisible(false);
    ui->fracHollowFiberEdit->setVisible(false);


    ui->variability_edit->setVisible(false);
    ui->variability_label->setVisible(false);
    ui->direction_combo->setVisible(false);
    ui->direction_label->setVisible(false);
}

RandomFibersGenCurvedFlower::~RandomFibersGenCurvedFlower()
{
    delete ui;
}
void RandomFibersGenCurvedFlower::setViewer(Viewer *view) {
    this->view = view;
}
//Performs actions when the hollow checkbox is clicked
void RandomFibersGenCurvedFlower::on_hollowCheckBox_clicked(bool checked)
{
    //Make the hollow dimension fields visible
    if(checked == true){
        ui->hollowRadiusDevEdit->setVisible(true);
        ui->hollowRadiusDevLabel->setVisible(true);
        ui->avgHollowRadiusLabel->setVisible(true);
        ui->avgHollowRadiusEdit->setVisible(true);
        ui->hollowDimLabel->setVisible(true);
        ui->fracFiberLabel->setVisible(true);
        ui->fracHollowFiberEdit->setVisible(true);
    }
    //Hide the hollow dimension fields visible
    else if(checked == false){
        ui->hollowRadiusDevEdit->setVisible(false);
        ui->hollowRadiusDevLabel->setVisible(false);
        ui->avgHollowRadiusLabel->setVisible(false);
        ui->avgHollowRadiusEdit->setVisible(false);
        ui->hollowDimLabel->setVisible(false);
        ui->fracFiberLabel->setVisible(false);
        ui->fracHollowFiberEdit->setVisible(false);
    }
}
//When generate button is clicked. Perform these actions
void RandomFibersGenCurvedFlower::on_Generate_clicked()
{
    //User inputs for workspace generation
    int xSize = ui->xSizeEdit->text().toInt();
    int ySize = ui->ySizeEdit->text().toInt();
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
    double avgRadiusCurvature = ui->avgRadiusCurvEdit->text().toDouble();
    double radiusCurvatureDev = ui->radiusCurvDeviationEdit->text().toDouble();
    double Accuracy = ui->accuracyEdit->text().toDouble();
    double avgPetalRadius = ui->avgPetalRadiusEdit->text().toDouble();
    double petalRadiusDev= ui->petalRadiusDevEdit->text().toDouble();
    int avgNumPetals = ui->avgNumPetalsEdit->text().toInt();
    int numPetalsDev = ui->numPetalsDevEdit->text().toInt();
    double  anglePlacement = ui->petalPlacementEdit->text().toDouble();
    double binderRadius = ui->bindRadiusEdit->text().toDouble();
    bool bindFibers = ui->bindCheck->isChecked();

    //Determine whether the user wants hollow fibers or not and generate accordingly.
    if (ui->hollowCheckBox->isChecked() == true){

        double fracHollowFibers = ui->fracHollowFiberEdit->text().toDouble();
        double avgHollowRadius = ui->avgHollowRadiusEdit->text().toDouble();
        double hollowRadiusDev = ui->hollowRadiusDevEdit->text().toDouble();

        RandomFibersInput input_CurvedFlower_Hollow;
        input_CurvedFlower_Hollow.curvedFlower_Hollow(xSize, ySize, zSize, avgRadius, radiusDev, avgLength, lengthDev, angleType, angle_variation, var_direction, Intersect,
                                                      Porosity, randomSeed,avgRadiusCurvature, radiusCurvatureDev, Accuracy, avgPetalRadius, petalRadiusDev, avgNumPetals, numPetalsDev, anglePlacement,fracHollowFibers, avgHollowRadius,
                                                      hollowRadiusDev);

        if(bindFibers) {
            input_CurvedFlower_Hollow.addBinder(binderRadius);
        }


        if ( puma::generateRandomFibers(view->workspace,input_CurvedFlower_Hollow) ){

            if(bindFibers) {
                view->workspace->setMaterialID(puma::Cutoff(2,32000), 255);
                view->workspace->setMaterialID(puma::Cutoff(1,1), 128);
            }

            view->setTomographySlice(0);
            view->slider->setValue(0);
        }
    }
    else if (ui->hollowCheckBox->isChecked() == false){
        RandomFibersInput input_CurvedFlower;
        input_CurvedFlower.curvedFlower(xSize, ySize, zSize, avgRadius, radiusDev, avgLength, lengthDev, angleType, angle_variation, var_direction, Intersect,
                                        Porosity, randomSeed,avgRadiusCurvature, radiusCurvatureDev, Accuracy, avgPetalRadius, petalRadiusDev, avgNumPetals, numPetalsDev, anglePlacement);

        if(bindFibers) {
            input_CurvedFlower.addBinder(binderRadius);
        }

        if ( puma::generateRandomFibers(view->workspace,input_CurvedFlower) ){

            if(bindFibers) {
                view->workspace->setMaterialID(puma::Cutoff(2,32000), 255);
                view->workspace->setMaterialID(puma::Cutoff(1,1), 128);
            }

            view->setTomographySlice(0);
            view->slider->setValue(0);
        }
    }
}


void RandomFibersGenCurvedFlower::on_angle_combo_currentIndexChanged(int index)
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

