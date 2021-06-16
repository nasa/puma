#include "randomspheresgenerator.h"
#include "ui_randomspheresgenerator.h"
#include <iostream>
#include <QFileDialog>

RandomSpheresGenerator::RandomSpheresGenerator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RandomSpheresGenerator)
{
    ui->setupUi(this);


}

RandomSpheresGenerator::~RandomSpheresGenerator()
{
    delete ui;
}
//Passes the view
void RandomSpheresGenerator::setViewer(Viewer *view) {
    this->view = view;
}
//When generate button is clicked. Perform these actions
void RandomSpheresGenerator::on_Generate_clicked()
{
    int xSize = ui->xSizeEdit->text().toInt();
    int ySize = ui->ySizeEdit->text().toInt();
    int zSize = ui->zSizeEdit->text().toInt();;
    double avgRadius = ui->avgRadiusEdit->text().toDouble();
    double radiusDev = ui->radiusDevEdit->text().toDouble();
    bool Intersect = ui->Intersect->isChecked();
    double Porosity= ui->porosityEdit->text().toDouble();
    int randomSeed=ui->randSeedEdit->text().toInt();

    RandomSpheresInput input(xSize,ySize,zSize,avgRadius,radiusDev,Intersect,Porosity,randomSeed);

    if ( puma::generateRandomSpheres(view->workspace,input) ) {
        view->setTomographySlice(0);
        view->slider->setValue(0);
    }


}
