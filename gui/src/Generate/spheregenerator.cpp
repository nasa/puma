#include "spheregenerator.h"
#include "ui_spheregenerator.h"
#include <iostream>
#include <QFileDialog>

SphereGenerator::SphereGenerator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::spheregenerator)
{
    ui->setupUi(this);


}

SphereGenerator::~SphereGenerator()
{
    delete ui;
}
//Passes the view
void SphereGenerator::setViewer(Viewer *view) {
    this->view = view;
}
//When generate button is clicked. Perform these actions
void SphereGenerator::on_Generate_clicked()
{
    int xSize = ui->xSizeEdit->text().toInt();
    int ySize = ui->ySizeEdit->text().toInt();
    int zSize = ui->zSizeEdit->text().toInt();
    double xCenter = ui->xcenterEdit->text().toDouble();
    double yCenter = ui->ycenterEdit->text().toDouble();
    double zCenter = ui->zcenterEdit->text().toDouble();
    double radius = ui->radiusEdit->text().toDouble();

    if(xSize <= 0 || ySize <=0 || zSize <=0) {
        std::cout << "Invalid domain size" << std::endl;
        return;
    }

    if(xCenter <= 0 || yCenter <=0 || zCenter <=0) {
        std::cout << "Invalid center" << std::endl;
        return;
    }

    if(xCenter >= xSize || yCenter >= ySize|| zCenter >= zSize ) {
        std::cout << "Invalid center" << std::endl;
        return;
    }

    if(radius <= 0) {
        std::cout << "Invalid radius" << std::endl;
        return;
    }

    view->workspace->resize(xSize, ySize, zSize);

    puma::Vec3<double> center(xCenter,yCenter,zCenter);

    Sphere mySphere(radius, center);

    if( mySphere.addSphereToDomain(view->workspace) ) {
        view->setTomographySlice(0);
        view->slider->setValue(0);
    }

}
