#include "boxgenerator.h"
#include "ui_boxgenerator.h"
#include <iostream>
#include <QFileDialog>

BoxGenerator::BoxGenerator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BoxGenerator)
{
    ui->setupUi(this);


}

BoxGenerator::~BoxGenerator()
{
    delete ui;
}
//Passes the view
void BoxGenerator::setViewer(Viewer *view) {
    this->view = view;
}
//When generate button is clicked. Perform these actions
void BoxGenerator::on_Generate_clicked()
{
    int xSize = ui->xSizeEdit->text().toInt();
    int ySize = ui->ySizeEdit->text().toInt();
    int zSize = ui->zSizeEdit->text().toInt();
    int xStart = ui->xStartEdit->text().toInt();
    int yStart = ui->yStartEdit->text().toInt();
    int zStart = ui->zStartEdit->text().toInt();
    int xLength = ui->xLengthEdit->text().toInt();
    int yLength = ui->yLengthEdit->text().toInt();
    int zLength = ui->zLengthEdit->text().toInt();

    if(xSize <= 0 || ySize <=0 || zSize <=0) {
        std::cout << "Invalid domain size" << std::endl;
        return;
    }

    if(xStart < 0 || yStart <0 || zStart <0) {
        std::cout << "Invalid center" << std::endl;
        return;
    }

    if(xStart >= xSize || yStart >= ySize|| zStart >= zSize ) {
        std::cout << "Invalid Length - Exits domain" << std::endl;
        return;
    }

    if(xLength <= 0 || yLength <=0 || zLength <=0) {
        std::cout << "Invalid Length - zeros" << std::endl;
        return;
    }

    if(xLength + xStart > xSize || yLength + yStart > ySize|| zLength + zStart > zSize ) {
        std::cout << "Invalid Length - Exits domain" << std::endl;
        return;
    }

    view->workspace->resize(xSize, ySize, zSize);

    view->workspace->matrix.set(xStart, xStart + xLength - 1, yStart, yStart + yLength - 1, zStart, zStart + zLength - 1, 255);

    view->setTomographySlice(0);
    view->slider->setValue(0);


}
