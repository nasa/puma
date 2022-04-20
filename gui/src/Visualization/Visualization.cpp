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

    // export vtk file to visualize
    bool check = puma::export_vtk(workspace, "tmp-gui-ws.vtk");

    if (!check){
        std::cout << "Workspace is empty, need to create it first." << std::endl;
        return;
    }

    // Python script
    std::string python_commands;
    python_commands += "import pumapy as puma;";
    python_commands += "ws = puma.import_vti('tmp-gui-ws.vtk');";
    python_commands += "cutoff = (" + std::to_string(lowVoid) + ", " + std::to_string(highVoid) + ");";
    if (visType == 1){
        python_commands += "puma.render_contour(ws, cutoff, solid_color=(255,255,255));";
    } else{
        python_commands += "puma.render_volume(ws, cutoff, solid_color=(255,255,255));";
    }
    python_commands += "import os; os.remove('tmp-gui-ws.vtk')";

    QProcess p;
    QStringList arguments = QStringList() << "-c" << QString::fromStdString(python_commands);
    int exitCode = p.execute("python", arguments);
}

void Visualization::on_visContourButton_clicked()
{
    Visualization::launchPython(1);
}

void Visualization::on_visVolumeButton_clicked()
{
    Visualization::launchPython(2);
}
