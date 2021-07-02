#include "Visualization.h"
#include "ui_Visualization.h"

#include <QMessageBox>
#include <QString>
#include <QProcess>
#include <QStringList>


Visualization::Visualization(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Visualization)
{
    ui->setupUi(this);
}

Visualization::~Visualization()
{
    delete ui;
}

//Passing in the current workspace
void Visualization::setWorkspace(puma::Workspace *workspace) {

    this->workspace = workspace;
}

void Visualization::launchPython(int visType)
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

    QString dir = QString::fromStdString(std::getenv("PuMA_DIR"));
    dir.append("/gui/src/pyscripts/");

    // export vtk file to visualize
    QString fileName = dir;
    fileName.append("gui-ws.vtk");
    bool check = puma::export_vtk(workspace, fileName.toStdString());

    if (!check){
        std::cout << "Workspace is empty, need to create it first." << std::endl;
        return;
    }

    QStringList args = QStringList() << dir + "access_pumapy.sh";

    // change it to the arguments to be run after "python"
    args << dir + "vis.py" << QString::number(lowVoid) << QString::number(highVoid) << QString::number(visType) ;

    QProcess p;
    p.setWorkingDirectory(dir);
    int exitCode = p.execute("/bin/bash", args);
}

void Visualization::on_visContourButton_clicked()
{
    Visualization::launchPython(1);
}

void Visualization::on_visVolumeButton_clicked()
{
    Visualization::launchPython(2);
}
