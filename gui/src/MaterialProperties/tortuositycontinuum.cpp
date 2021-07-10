#include "tortuositycontinuum.h"
#include "ui_tortuositycontinuum.h"

#include "../Utilities/guiprinter.h"

#include <QFileDialog>

tortuosityContinuum::tortuosityContinuum(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tortuosityContinuum)
{
    ui->setupUi(this);

    // ui default startup
    ui->comboBox_4->setVisible(false);
    ui->bcSideDirectLabel->setVisible(false);
    ui->comboBox_3->setVisible(false);
    ui->solverTypeLabel->setVisible(false);

}

tortuosityContinuum::~tortuosityContinuum()
{
    delete ui;
}

//Passing in the current workspace
void tortuosityContinuum::setWorkspace(puma::Workspace *workspace) {

    this->workspace = workspace;
}

//Checks which solver method the user has chosen
void tortuosityContinuum::on_comboBox_currentIndexChanged(int index)
{
    //Explicit Jump
    if (index == 0){
        ui->comboBox_4->setVisible(false);
        ui->bcSideDirectLabel->setVisible(false);
        ui->comboBox_3->setVisible(false);
        ui->solverTypeLabel->setVisible(false);
    }
    //Finite Volume
    else if(index == 1){
        ui->comboBox_4->setVisible(true);
        ui->bcSideDirectLabel->setVisible(true);
        ui->comboBox_3->setVisible(true);
        ui->solverTypeLabel->setVisible(true);
    }
}

void tortuosityContinuum::Calculate() {

    //Defining variables

    Cx.resize(0,0,0);
    Cy.resize(0,0,0);
    Cz.resize(0,0,0);

    puma::Vec3<double> Tx;
    puma::Vec3<double> Ty;
    puma::Vec3<double> Tz;
    std::string solverType;
    std::string bcSideDirection;

    //Gathering user input for threshold values, number of cores, iterations and accuracy
    int lowThreshold = ui->lowThreshEdit->text().toInt();
    int highThreshold = ui->highThreshEdit->text().toInt();
    int numThreads = ui->threadsEdit->text().toInt();
    int iterations = ui->iterationsEdit->text().toInt();
    double accuracy = ui->accuracyEdit->text().toDouble();

    puma::Cutoff cutoff(lowThreshold,highThreshold);

    //Explicit Jump
    if (ui->comboBox->currentIndex() == 0){

        //Check for which directions the user wants to solve
        if(ui->xDirCheckBox->isChecked() == true){
            std::cout << "X-Direction:" <<  std::endl;
            Tx = puma::compute_EJTortuosity(workspace, &Cx, cutoff,'x',accuracy,iterations,true,numThreads);
        }
        if (ui->yDirCheckBox->isChecked() == true){
            std::cout << "\nY-Direction:" <<  std::endl;
            Ty = puma::compute_EJTortuosity(workspace, &Cy,cutoff,'y',accuracy,iterations,true,numThreads);
        }
        if (ui->zDirCheckBox->isChecked() == true){
            std::cout << "\nZ-Direction:" <<  std::endl;
            Tz = puma::compute_EJTortuosity(workspace, &Cz,cutoff,'z',accuracy,iterations,true,numThreads);    }
    }
    // Finite Volume
    if (ui->comboBox->currentIndex() == 1){

        //Determine solver Type
        if(ui->comboBox_4->currentIndex() == 0) {
            solverType = "Conjugate Gradient";
        }
        else if (ui->comboBox_4->currentIndex() == 1){
            std::cout << "Label 1.00" << std::endl;
            solverType = "BiCGSTAB";
        }
        //Determine bc side directions
        if(ui->comboBox_3->currentIndex() == 0) {
            bcSideDirection = "Periodic";
        }
        else if (ui->comboBox_3->currentIndex() == 1){
            bcSideDirection = "Symmetric";
        }
        //Check for which directions the user wants to solve
        if(ui->xDirCheckBox->isChecked() == true){
            std::cout << "X-Direction:" <<  std::endl;
            Tx = puma::compute_FVTortuosity(workspace, &Cx, cutoff, bcSideDirection, solverType, 'x', accuracy, iterations, true, numThreads);
        }
        if (ui->yDirCheckBox->isChecked() == true){
            std::cout << "\nY-Direction:" <<  std::endl;
            Ty = puma::compute_FVTortuosity(workspace, &Cy, cutoff, bcSideDirection, solverType, 'y', accuracy, iterations, true, numThreads);
        }
        if (ui->zDirCheckBox->isChecked() == true){
            std::cout << "\nZ-Direction:" <<  std::endl;
            Tz = puma::compute_FVTortuosity(workspace, &Cz, cutoff, bcSideDirection, solverType, 'z', accuracy, iterations, true, numThreads);
        }

        //TortuosityParticles::computeDiffusionCoefficient()
    }


    std::cout << Tx.x << " " << Tx.y << " " << Tx.z << std::endl;
    std::cout << Ty.x << " " << Ty.y << " " << Ty.z << std::endl;
    std::cout << Tz.x << " " << Tz.y << " " << Tz.z << std::endl;

    //    //Print To Tensor matrix
    QTableWidgetItem *item_txx = new QTableWidgetItem();
    item_txx->setData(Qt::EditRole, Tx.x);
    ui->cond_table->setItem(0, 0, item_txx);

    QTableWidgetItem *item_txy = new QTableWidgetItem();
    item_txy->setData(Qt::EditRole, Tx.y);
    ui->cond_table->setItem(0, 1, item_txy);

    QTableWidgetItem *item_txz = new QTableWidgetItem();
    item_txz->setData(Qt::EditRole, Tx.z);
    ui->cond_table->setItem(0, 2, item_txz);

    QTableWidgetItem *item_tyx = new QTableWidgetItem();
    item_tyx->setData(Qt::EditRole, Ty.x);
    ui->cond_table->setItem(1, 0, item_tyx);

    QTableWidgetItem *item_tyy = new QTableWidgetItem();
    item_tyy->setData(Qt::EditRole, Ty.y);
    ui->cond_table->setItem(1, 1, item_tyy);

    QTableWidgetItem *item_tyz = new QTableWidgetItem();
    item_tyz->setData(Qt::EditRole, Ty.z);
    ui->cond_table->setItem(1, 2, item_tyz);

    QTableWidgetItem *item_tzx = new QTableWidgetItem();
    item_tzx->setData(Qt::EditRole, Tz.x);
    ui->cond_table->setItem(2, 0, item_tzx);

    QTableWidgetItem *item_tzy = new QTableWidgetItem();
    item_tzy->setData(Qt::EditRole, Tz.y);
    ui->cond_table->setItem(2, 1, item_tzy);

    QTableWidgetItem *item_tzz = new QTableWidgetItem();
    item_tzz->setData(Qt::EditRole, Tz.z);
    ui->cond_table->setItem(2, 2, item_tzz);

    this->repaint();

}

//Actions that occur after calculate button is clicked
void tortuosityContinuum::on_calculateButton_clicked()
{
    //This clears previous iterations
    ui->textBrowser->clear();

    ui->textBrowser->clear();
    GUIPrinter printer(ui->textBrowser, qApp);
    workspace->setPrinter(&printer);
    Calculate();
    workspace->newPrinter();


}

void tortuosityContinuum::on_vtkButton_clicked()
{
    if(Cx.size() == 0 && Cy.size() == 0 && Cz.size() == 0) {
        return;
    }

    QString baseName = QFileDialog::getSaveFileName(this,"Export File Name", ".");

    if(baseName == "") {
        return;
    }

    if(baseName.endsWith(".vtk")) {
        baseName.replace(".vtk","");
    }

    if(Cx.size() !=0 ) {
        QString fileName = baseName + "_Cx.vtk";
        puma::export_vtk(&Cx, fileName.toStdString());
    }
    if(Cy.size() !=0 ) {
        QString fileName = baseName + "_Cy.vtk";
        puma::export_vtk(&Cy, fileName.toStdString());
    }
    if(Cz.size() !=0 ) {
        QString fileName = baseName + "_Cz.vtk";
        puma::export_vtk(&Cz, fileName.toStdString());
    }
}

void tortuosityContinuum::on_binaryButton_clicked()
{
    if(Cx.size() == 0 && Cy.size() == 0 && Cz.size() == 0) {
        return;
    }

    QString baseName = QFileDialog::getSaveFileName(this,"Export File Name", ".");

    if(baseName == "") {
        return;
    }

    QString suffix = ".puma";
    if(baseName.endsWith(".bin")) {
        baseName.replace(".bin","");
        suffix = ".bin";
    }
    if(baseName.endsWith(".puma")) {
        baseName.replace(".puma","");
    }
    if(baseName.endsWith(".raw")) {
        baseName.replace(".raw","");
        suffix = ".raw";
    }

    if(Cx.size() !=0 ) {
        QString fileName = baseName + "_Cx" + suffix;
        puma::export_bin(&Cx, fileName.toStdString());
    }
    if(Cy.size() !=0 ) {
        QString fileName = baseName + "_Cy" + suffix;
        puma::export_bin(&Cy, fileName.toStdString());
    }
    if(Cz.size() !=0 ) {
        QString fileName = baseName + "_Cz" + suffix;
        puma::export_bin(&Cz, fileName.toStdString());
    }
}
