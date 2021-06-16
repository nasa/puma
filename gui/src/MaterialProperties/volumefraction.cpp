#include "volumefraction.h"
#include "ui_volumefraction.h"

#include <QMessageBox>

VolumeFraction::VolumeFraction(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VolumeFraction)
{
    ui->setupUi(this);
}

VolumeFraction::~VolumeFraction()
{
    delete ui;
}

//Passing in the current workspace
void VolumeFraction::setWorkspace(puma::Workspace *workspace) {

    this->workspace = workspace;
}

void VolumeFraction::on_calculateButton_clicked()
{
    int lowVoid = ui->Boundary1Edit->text().toInt();
    int highVoid = ui->Boundary2Edit->text().toInt();

    if(lowVoid == 0 && ui->Boundary1Edit->text() != "0") {
        std::cout << "Invalid Low Boundary Input. Please put an integer value" << std::endl;

        QMessageBox box;
        box.setWindowTitle("Error");
        box.setText("Invalid Low Boundary Input. Please put an integer value");
        box.exec();

    }

    if(highVoid == 0 && ui->Boundary2Edit->text() != "0") {
        std::cout << "Invalid High Boundary Input. Please put an integer value" << std::endl;

        QMessageBox box;
        box.setWindowTitle("Error");
        box.setText("Invalid High Boundary Input. Please put an integer value");
        box.exec();

    }

    if(lowVoid > highVoid) {
        int bound = highVoid;
        highVoid = lowVoid;
        lowVoid = bound;
    }

    puma::Cutoff cutoff = puma::Cutoff(lowVoid, highVoid);

    double vFraction = puma::compute_VolumeFraction(workspace, cutoff, 0);
    QString vfString = QString::number(vFraction); //Store porosity value as a string
    ui->porositResult->setText(vfString);

    this->repaint();

}
