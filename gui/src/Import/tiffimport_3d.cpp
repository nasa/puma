#include "tiffimport_3d.h"
#include "ui_tiffimport_3d.h"

TiffImport_3D::TiffImport_3D(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TiffImport_3D)
{
    ui->setupUi(this);


}

TiffImport_3D::~TiffImport_3D()
{
    delete ui;
}
