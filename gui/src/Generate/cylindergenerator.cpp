#include "cylindergenerator.h"
#include "ui_cylindergenerator.h"
#include <iostream>
#include <QFileDialog>

CylinderGenerator::CylinderGenerator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CylinderGenerator)
{
    ui->setupUi(this);


}

CylinderGenerator::~CylinderGenerator()
{
    delete ui;
}
//Passes the view
void CylinderGenerator::setViewer(Viewer *view) {
    this->view = view;
}
//When generate button is clicked. Perform these actions
void CylinderGenerator::on_Generate_clicked()
{
    int xSize = ui->xSizeEdit->text().toInt();
    int ySize = ui->ySizeEdit->text().toInt();
    int zSize = ui->zSizeEdit->text().toInt();
    double xStart = ui->xStartEdit->text().toDouble();
    double yStart = ui->yStartEdit->text().toDouble();
    double zStart = ui->zStartEdit->text().toDouble();
    double xDir = ui->xDirectionEdit->text().toDouble();
    double yDir = ui->yDirectionEdit->text().toDouble();
    double zDir = ui->zDirectionEdit->text().toDouble();

    double radius = ui->radiusEdit->text().toDouble();
    double length = ui->lengthEdit->text().toDouble();

    puma::Vec3<double> dir(xDir,yDir,zDir);
    double mag = dir.magnitude();
    puma::Vec3<double> unitVec = dir.norm();

    if(mag < 1e-6){
        std::cout << "Invalid direction vector, can't be all zeros" << std::endl;
        return;
    }

    if(xSize <= 0 || ySize <=0 || zSize <=0) {
        std::cout << "Invalid domain size" << std::endl;
        return;
    }

    if(xStart < 0 || yStart <0 || zStart <0) {
        std::cout << "Invalid center" << std::endl;
        return;
    }

    if(xStart >= xSize || yStart >= ySize|| zStart >= zSize ) {
        std::cout << "Invalid Start" << std::endl;
        return;
    }

    if(radius <= 0) {
        std::cout << "Invalid radius" << std::endl;
        return;
    }

    if(length <= 0) {
        std::cout << "Invalid length" << std::endl;
        return;
    }

    puma::Vec3<double> startVec(xStart,yStart,zStart);
    puma::Vec3<double> endVec = startVec + unitVec * length;

    view->workspace->resize(xSize, ySize, zSize);

    StraightCircleFiber cylinder(radius, length, startVec, endVec);

    if( cylinder.addFiberToDomain_Uncapped(view->workspace, NULL)) {
        view->setTomographySlice(0);
        view->slider->setValue(0);
    }

}
