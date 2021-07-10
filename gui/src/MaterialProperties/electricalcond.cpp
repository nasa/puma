#include "electricalcond.h"
#include "ui_electricalcond.h"


#include "../Utilities/guiprinter.h"

#include <QObject>
#include <QTimer>
#include <QFileDialog>
#include <QString>


ElectricalCond::ElectricalCond(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ElectricalCond){
    ui->setupUi(this);

    //Explicit Jump is the default. These options are hidden in the beginning.
    ui->comboBox_2->setVisible(false);
    ui->bcSideDirectLabel->setVisible(false);
    ui->comboBox_3->setVisible(false);
    ui->solverTypeLabel->setVisible(false);

}

ElectricalCond::~ElectricalCond(){
    delete ui;
}

void ElectricalCond::setViewer(Viewer *view) {
    this->view = view;
    this->workspace = view->workspace;

}


//Checks which solver method the user has chosen
void ElectricalCond::on_comboBox_currentIndexChanged(int index)
{
    if(index == 0){
        ui->comboBox_2->setVisible(false);
        ui->bcSideDirectLabel->setVisible(false);
        ui->comboBox_3->setVisible(false);
        ui->solverTypeLabel->setVisible(false);

    }
    else if (index == 1){
        ui->comboBox_2->setVisible(true);
        ui->bcSideDirectLabel->setVisible(true);
        ui->comboBox_3->setVisible(true);
        ui->solverTypeLabel->setVisible(true);
    }
}

void ElectricalCond::Calculate() {

    //Defining variables
    std::string solverType;
    std::string bcSideDirection;
    puma::Vec3<double> kx;
    puma::Vec3<double> ky;
    puma::Vec3<double> kz;

    Tx.resize(0,0,0);
    Ty.resize(0,0,0);
    Tz.resize(0,0,0);

    std::map<int, double> matCond;

    //Checks for which materials the user wants to include
    if(ui->includeCheckBox1->isChecked()== true){
        int highCutoff1 = ui->highCutoffEdit1->text().toInt();
        double k1 = ui->sigmaEdit1->text().toDouble();
        matCond[highCutoff1]=k1;
    }
    if(ui->includeCheckBox2->isChecked()== true){
        int highCutoff2 = ui->highCutEdit2->text().toInt();
        double k2 = ui->sigmaEdit2->text().toDouble();
        matCond[highCutoff2]=k2;
    }
    if(ui->includeCheckBox3->isChecked()== true){
        int highCutoff3 = ui->highCutOffEdit3->text().toInt();
        double k3 = ui->sigmaEdit3->text().toDouble();
        matCond[highCutoff3]=k3;
    }
    if(ui->includeCheckBox4->isChecked()== true){
        int highCutOff4 = ui->highCutOffEdit4->text().toInt();
        double k4 = ui->sigmaEdit4->text().toDouble();
        matCond[highCutOff4]=k4;
    }
    if(ui->includeCheckBox5->isChecked()== true){
        int highCutOff5 = ui->highCutOffEdit5->text().toInt();
        double k5 = ui->sigmaEdit5->text().toDouble();
        matCond[highCutOff5]=k5;
    }
    if(ui->includeCheckBox6->isChecked()== true){
        int highCutOff6 = ui->highCutOffEdit6->text().toInt();
        double k6 = ui->sigmaEdit6->text().toDouble();
        matCond[highCutOff6]=k6;
    }
    //User input for accuracy, cores used, and # of iterations
    double accuracy =  ui->solverToleranceEdit->text().toDouble();
    int numThreads = ui->numThreadsEdit->text().toInt();
    int iterations = ui->iterationsEdit->text().toInt();


    //Explicit Jump calculation
    if (ui->comboBox->currentIndex() == 0){

        if(ui->xDirCheckBox->isChecked() == true){
            std::cout << "X-Direction:" <<  std::endl;
            kx = puma::compute_EJElectricalConductivity(workspace, &Tx,matCond,'x',accuracy,iterations,true,numThreads);
        }

        this->repaint();

        if (ui->yDirCheckBox->isChecked() == true){
            std::cout << "\nY-Direction:" <<  std::endl;
            ky = puma::compute_EJElectricalConductivity(workspace, &Ty,matCond,'y',accuracy,iterations,true,numThreads);
        }

        this->repaint();


        if (ui->zDirCheckBox->isChecked() == true){
            std::cout << "\nZ-Direction:" <<  std::endl;
            kz = puma::compute_EJElectricalConductivity(workspace, &Tz,matCond,'z',accuracy,iterations,true,numThreads);
        }

        this->repaint();

    }
    //Finite Volume calculation
    if (ui->comboBox->currentIndex() == 1){
        //Determine solver Type
        if(ui->comboBox_2->currentIndex() == 0) {
            solverType = "Conjugate Gradient";
        }
        else if (ui->comboBox_2->currentIndex() == 1){
            solverType = "BiCGSTAB";
        }
        //Determine bc side directions
        if(ui->comboBox_3->currentIndex() == 0) {
            bcSideDirection = "Periodic";
        }
        else if (ui->comboBox_2->currentIndex() == 1){
            solverType = "Symmetric";
        }
        //calculate Electrical Conductivity for the directions that are selected
        if(ui->xDirCheckBox->isChecked() == true){
            std::cout << "X-Direction:" <<  std::endl;
            kx = puma::compute_FVElectricalConductivity(workspace, &Tx,matCond,bcSideDirection,solverType,'x',accuracy,iterations,true, numThreads);
        }

        this->repaint();


        if (ui->yDirCheckBox->isChecked() == true){
            std::cout << "\nY-Direction:" <<  std::endl;
            ky = puma::compute_FVElectricalConductivity(workspace, &Ty,matCond,bcSideDirection,solverType,'y',accuracy,iterations,true,numThreads);
        }

        this->repaint();

        if (ui->zDirCheckBox->isChecked() == true){
            std::cout << "\nZ-Direction:" <<  std::endl;
            kz = puma::compute_FVElectricalConductivity(workspace, &Tz,matCond,bcSideDirection,solverType,'z',accuracy,iterations,true,numThreads);
        }

        this->repaint();

    }

    std::cout << kx.x << " " << kx.y << " " << kx.z << std::endl;
    std::cout << ky.x << " " << ky.y << " " << ky.z << std::endl;
    std::cout << kz.x << " " << kz.y << " " << kz.z << std::endl;


//    //Print to tensor matrix
    QTableWidgetItem *item_kxx = new QTableWidgetItem();
    item_kxx->setData(Qt::EditRole, kx.x);
    ui->cond_table->setItem(0, 0, item_kxx);

    QTableWidgetItem *item_kxy = new QTableWidgetItem();
    item_kxy->setData(Qt::EditRole, kx.y);
    ui->cond_table->setItem(0, 1, item_kxy);

    QTableWidgetItem *item_kxz = new QTableWidgetItem();
    item_kxz->setData(Qt::EditRole, kx.z);
    ui->cond_table->setItem(0, 2, item_kxz);

    QTableWidgetItem *item_kyx = new QTableWidgetItem();
    item_kyx->setData(Qt::EditRole, ky.x);
    ui->cond_table->setItem(1, 0, item_kyx);

    QTableWidgetItem *item_kyy = new QTableWidgetItem();
    item_kyy->setData(Qt::EditRole, ky.y);
    ui->cond_table->setItem(1, 1, item_kyy);

    QTableWidgetItem *item_kyz = new QTableWidgetItem();
    item_kyz->setData(Qt::EditRole, ky.z);
    ui->cond_table->setItem(1, 2, item_kyz);

    QTableWidgetItem *item_kzx = new QTableWidgetItem();
    item_kzx->setData(Qt::EditRole, kz.x);
    ui->cond_table->setItem(2, 0, item_kzx);

    QTableWidgetItem *item_kzy = new QTableWidgetItem();
    item_kzy->setData(Qt::EditRole, kz.y);
    ui->cond_table->setItem(2, 1, item_kzy);

    QTableWidgetItem *item_kzz = new QTableWidgetItem();
    item_kzz->setData(Qt::EditRole, kz.z);
    ui->cond_table->setItem(2, 2, item_kzz);

    this->repaint();



}


//Actions that occur after calculate button is clicked
void ElectricalCond::on_CalculateButton_clicked()
{
    ui->textBrowser_2->clear();
    GUIPrinter printer(ui->textBrowser_2, qApp);
    workspace->setPrinter(&printer);
    Calculate();
    workspace->newPrinter();
}


void ElectricalCond::on_vtkButton_clicked()
{
    if(Tx.size() == 0 && Ty.size() == 0 && Tz.size() == 0) {
        return;
    }

    QString baseName = QFileDialog::getSaveFileName(this,"Export File Name", ".");

    if(baseName == "") {
        return;
    }

    if(baseName.endsWith(".vtk")) {
        baseName.replace(".vtk","");
    }

    if(Tx.size() !=0 ) {
        QString fileName = baseName + "_Tx.vtk";
        puma::export_vtk(&Tx, fileName.toStdString());
    }
    if(Ty.size() !=0 ) {
        QString fileName = baseName + "_Ty.vtk";
        puma::export_vtk(&Ty, fileName.toStdString());
    }
    if(Tz.size() !=0 ) {
        QString fileName = baseName + "_Tz.vtk";
        puma::export_vtk(&Tz, fileName.toStdString());
    }

}

void ElectricalCond::on_binaryButton_clicked()
{
    if(Tx.size() == 0 && Ty.size() == 0 && Tz.size() == 0) {
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

    if(Tx.size() !=0 ) {
        QString fileName = baseName + "_Tx" + suffix;
        puma::export_bin(&Tx, fileName.toStdString());
    }
    if(Ty.size() !=0 ) {
        QString fileName = baseName + "_Ty" + suffix;
        puma::export_bin(&Ty, fileName.toStdString());
    }
    if(Tz.size() !=0 ) {
        QString fileName = baseName + "_Tz" + suffix;
        puma::export_bin(&Tz, fileName.toStdString());
    }
}
