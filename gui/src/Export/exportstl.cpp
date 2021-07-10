#include "exportstl.h"
#include "ui_exportstl.h"

#include <QFileDialog>

ExportSTL::ExportSTL(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportSTL)
{
    ui->setupUi(this);
}

ExportSTL::~ExportSTL()
{
    delete ui;
}

void ExportSTL::setWorkspace(puma::Workspace *workspace) {

    this->workspace = workspace;
}


void ExportSTL::on_lowEdit_textChanged(const QString &arg1)
{
    double value = arg1.toInt();

    if(value > 255) {
        value = 255;
        ui->lowEdit->setText("255");
    }

    if(value < 0) {
        value = 0;
        ui->lowEdit->setText("0");
    }
}

void ExportSTL::on_highEdit_textChanged(const QString &arg1)
{
    double value = arg1.toInt();

    if(value > 255) {
        value = 255;
        ui->highEdit->setText("255");
    }

    if(value < 0) {
        value = 0;
        ui->highEdit->setText("0");
    }
}

void ExportSTL::on_pushButton_clicked()
{
    int lowVal = ui->lowEdit->text().toInt();
    int highVal = ui->highEdit->text().toInt();

    puma::Cutoff cutoff = puma::Cutoff(lowVal,highVal);

    std::string fileName = QFileDialog::getSaveFileName(this,"Export File Name", ".").toStdString();

    if(fileName == "") {
        return;
    }

    puma::export_STL(workspace,cutoff,false,fileName);

}
