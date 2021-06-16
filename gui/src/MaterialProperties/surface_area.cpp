#include "surface_area.h"
#include "ui_surface_area.h"

Surface_Area::Surface_Area(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Surface_Area)
{
    ui->setupUi(this);
}

Surface_Area::~Surface_Area()
{
    delete ui;
}

//passing in the current workspace
void Surface_Area::setWorkspace(puma::Workspace *workspace) {

    this->workspace = workspace;
}

//Looks at change in Combobox selection
void Surface_Area::on_comboBox_currentIndexChanged(int index)
{
    if (index == 0){
        ui->interpVertsCheckBox->setVisible(true); //if Marching Cubes surface chosen, show this checkbox
    }
    else if (index == 1){
        ui->interpVertsCheckBox->setVisible(false); //if Cuberille surface chosen, hide checkbox
    }
}

//Actions that occur after calculate button is clicked
void Surface_Area::on_pushButton_clicked()
{

    int lowCutOff = ui->Boundary1Edit->text().toInt();
    int highCutOff = ui->Boundary2Edit->text().toInt();
    int numThreads = ui->numThreadsEdit->text().toInt();
    bool interpVerts;
    double sArea;

    //Looks at changes in state of vertice interpolation check box
    if (ui->interpVertsCheckBox->isChecked() == true){
        interpVerts = true;
    }
    else if (ui->interpVertsCheckBox->isChecked() == false){
        interpVerts = false;
    }

    //makes it so boundaries are put into function irrespective of which boundary they enter 1st
    if (lowCutOff > highCutOff){
        highCutOff = lowCutOff;
    }

    if (ui->comboBox->currentIndex() == 0){

        puma::Cutoff cutoff(lowCutOff,highCutOff);
        std::pair<double,double> sArea = puma::compute_SurfaceAreaMarchingCubes(workspace, cutoff, interpVerts, numThreads);

        double sa = sArea.first; //store surface area value as a double
        double ssa =sArea.second; //store specific surface area value as a double

        std::cout << sa << std::endl; //Store surface area value as a string
        std::cout << ssa << std::endl; //Store specific surface area value as a string


        QString strSa = QString::number(sa); //Store surface area value as a string
        QString strSsa = QString::number(ssa); //Store specific surface area value as a string

        ui->surfaceAreaValue->setText(strSa); //Display surface Area Value in ui
        ui->specificSurfaceAreaValue->setText(strSsa); //Display surface area value in ui

        this->repaint();

    }

    else if(ui->comboBox->currentIndex()==1){

        puma::Cutoff cutoff(lowCutOff,highCutOff);
        std::pair<double,double> sArea = puma::compute_SurfaceAreaVoxels(workspace,cutoff, numThreads);

        double sa = sArea.first; //store surface area value as a double
        double ssa =sArea.second; //store specific surface area value as a double

        std::cout << sa << std::endl; //Store surface area value as a string
        std::cout << ssa << std::endl; //Store specific surface area value as a string

        QString strSa = QString::number(sa); //Store surface area value as a string
        QString strSsa = QString::number(ssa); //Store specific surface area value as a string

        ui->surfaceAreaValue->setText(strSa); //Display surface Area Value in ui
        ui->specificSurfaceAreaValue->setText(strSsa); //Display surface area value in ui

        this->repaint();
    }
}







