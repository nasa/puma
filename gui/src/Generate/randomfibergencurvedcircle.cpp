#include "randomfibergencurvedcircle.h"
#include "ui_randomfibergencurvedcircle.h"
#include <iostream>
#include <QFileDialog>

RandomFiberGenCurvedCircle::RandomFiberGenCurvedCircle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RandomFiberGenCurvedCircle)
{
    ui->setupUi(this);
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
    int angleVarX = ui->xAngleEdit_2->text().toInt();
    int angleVarY = ui->yAngleEdit_2->text().toInt();
    int angleVarZ = ui->zAngleEdit_2->text().toInt();
    bool Intersect = ui->Intersect->isChecked();
    double Porosity = ui->porosityEdit_2->text().toDouble();
    int randomSeed = ui->randSeedEdit_2->text().toInt();
    double avgRadiusCurvature = ui->avgRadiusCurvEdit->text().toInt();
    double radiusCurvatureDev = ui->curvDeviationEdit->text().toInt();
    double  Accuracy = ui->accuracyEdit->text().toDouble();
    double binderRadius = ui->bindRadiusEdit->text().toDouble();
    bool bindFibers = ui->bindCheck->isChecked();

    RandomFibersInput input_curvedCircle;
    input_curvedCircle.curvedCircle(xSize,ySize,zSize,avgRadius,radiusDev,avgLength,lengthDev,angleVarX,angleVarY,angleVarZ,Intersect,Porosity,randomSeed,avgRadiusCurvature,radiusCurvatureDev,Accuracy);

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
