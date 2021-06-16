#include "randomfiberstraightcircle.h"
#include "ui_randomfiberstraightcircle.h"
#include <iostream>
#include <QFileDialog>

RandomFiberStraightCircle::RandomFiberStraightCircle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RandomFiberStraightCircle)
{
    ui->setupUi(this);
}

RandomFiberStraightCircle::~RandomFiberStraightCircle()
{
    delete ui;
}

//passes the view (2D visual of workspace)
void RandomFiberStraightCircle::setViewer(Viewer *view) {
    this->view = view;
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
    int angleVarX = ui->xAngleEdit->text().toInt();
    int angleVarY = ui->yAngleEdit->text().toInt();
    int angleVarZ = ui->zAngleEdit->text().toInt();
    bool Intersect = ui->Intersect->isChecked();
    double Porosity = ui->porosityEdit->text().toDouble();
    int randomSeed = ui->randSeedEdit->text().toInt();
    double binderRadius = ui->bindRadiusEdit->text().toDouble();
    bool bindFibers = ui->bindCheck->isChecked();

    RandomFibersInput input_straightCircle;
    input_straightCircle.straightCircle(xSize,ySize,zSize,avgRadius,radiusDev,avgLength,lengthDev,angleVarX,angleVarY,angleVarZ,Intersect,Porosity,randomSeed);

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
