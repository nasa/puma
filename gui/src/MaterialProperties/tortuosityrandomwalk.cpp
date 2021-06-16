#include "tortuosityrandomwalk.h"
#include "ui_tortuosityrandomwalk.h"

#include "../Utilities/tortuosityprinter.h"

tortuosityRandomWalk::tortuosityRandomWalk(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tortuosityRandomWalk)
{
    ui->setupUi(this);

    ui->Tort_Output_Label->setVisible(false);
    ui->TortuosityXLabel->setVisible(false);
    ui->TortuosityYLabel->setVisible(false);
    ui->TortuosityZLabel->setVisible(false);

    ui->Diff_Output_Label->setText("Charactersitic Length (m):");

    ui->knudsenEdit->setVisible(false);
    ui->knudsenLabel->setVisible(false);

}

tortuosityRandomWalk::~tortuosityRandomWalk()
{
    delete ui;
}

//Passing in the current workspace
void tortuosityRandomWalk::setWorkspace(puma::Workspace *workspace) {

    this->workspace = workspace;
}

void tortuosityRandomWalk::on_pushButton_clicked()
{
    TortuosityPrinter myPrinter(ui->msdPlot, qApp);
    workspace->setPrinter(&myPrinter);


    int lowCutOff = ui->lowCutOffEdit->text().toInt();
    int highCutOff = ui->highCutOffEdit->text().toInt();
    int numParticles = ui->numParticlesEdit->text().toInt();
    double meanFreePath = ui->meanFreePathEdit->text().toDouble();
    double meanVelocity = ui->meanThermalVelocityEdit->text().toDouble();
    double totalLength = ui->totWalkLengthEdit->text().toDouble();
    int randomSeed = ui->randSeedEdit->text().toInt();
    int numThreads = ui->numThreadEdit->text().toInt();
    int tortMethod = ui->comboBox->currentIndex();

    bool constTort = ui->comboBox_2->currentIndex() == 0;

    if(constTort) {
        if(ui->eta_x_edit->text()=="" || ui->eta_y_edit->text()=="" || ui->eta_z_edit->text()=="") {
            std::cout << "Must enter continuum tortuosity values" << std::endl;
            return;
        }
    }

    double nx = ui->eta_x_edit->text().toDouble();
    double ny = ui->eta_y_edit->text().toDouble();
    double nz = ui->eta_z_edit->text().toDouble();




    puma::Cutoff cutoff(lowCutOff,highCutOff);

    double porosity = puma::compute_VolumeFraction(workspace, cutoff, 0);

    puma::TortuosityReturn TortuosityOut;

    if(tortMethod == 0) {
        TortuosityOut = puma::compute_particle_isosurface_Tortuosity(workspace, cutoff, numParticles, meanFreePath, meanVelocity, randomSeed, totalLength, numThreads);
    }
    else if(tortMethod == 1) {
        TortuosityOut = puma::compute_particle_cuberille_Tortuosity(workspace, cutoff, numParticles, meanFreePath, meanVelocity, randomSeed, totalLength, numThreads);
    }

    puma::Vec3<double> Tortuosity = TortuosityOut.tortuosity;
    QString TortX = QString::number(Tortuosity.x);
    QString TortY = QString::number(Tortuosity.y);
    QString TortZ = QString::number(Tortuosity.z);

    puma::Vec3<double> Diffusivity = TortuosityOut.diffusivity;
    QString DiffX = QString::number(Diffusivity.x);
    QString DiffY = QString::number(Diffusivity.y);
    QString DiffZ = QString::number(Diffusivity.z);

    puma::Vec3<double> DiffusionCoefficient = TortuosityOut.diffusionCoefficient;
    QString DeffX = QString::number(DiffusionCoefficient.x);
    QString DeffY = QString::number(DiffusionCoefficient.y);
    QString DeffZ = QString::number(DiffusionCoefficient.z);

    QString meanInterceptLength = QString::number(TortuosityOut.meanInterceptLength);


    if(constTort) {
        double lx = 3.0 * nx * DiffusionCoefficient.x / (porosity * meanVelocity);
        double ly = 3.0 * ny * DiffusionCoefficient.y / (porosity * meanVelocity);
        double lz = 3.0 * nz * DiffusionCoefficient.z / (porosity * meanVelocity);
        ui->DiffusivityXLabel->setText(QString::number(lx));
        ui->DiffusivityYLabel->setText(QString::number(ly));
        ui->DiffusivityZLabel->setText(QString::number(lz));
    }
    else {
        ui->DiffusivityXLabel->setText(DeffX);
        ui->DiffusivityYLabel->setText(DeffY);
        ui->DiffusivityZLabel->setText(DeffZ);
        ui->TortuosityXLabel->setText(TortX);
        ui->TortuosityYLabel->setText(TortY);
        ui->TortuosityZLabel->setText(TortZ);
    }

    ui->DeffXLabel->setText(DiffX);
    ui->DeffYLabel->setText(DiffY);
    ui->DeffZLabel->setText(DiffZ);
    ui->MeanInterceptLengthEdit->setText(meanInterceptLength);
    ui->poroEdit->setText(QString::number(porosity));


    workspace->newPrinter();

    this->repaint();


}

void tortuosityRandomWalk::on_comboBox_2_currentIndexChanged(int index)
{
    if(index==0) {
        ui->eta_x_edit->setVisible(true);
        ui->eta_y_edit->setVisible(true);
        ui->eta_z_edit->setVisible(true);
        ui->eta_x_label->setVisible(true);
        ui->eta_y_label->setVisible(true);
        ui->eta_z_label->setVisible(true);

        ui->Tort_Output_Label->setVisible(false);
        ui->TortuosityXLabel->setVisible(false);
        ui->TortuosityYLabel->setVisible(false);
        ui->TortuosityZLabel->setVisible(false);

        ui->Diff_Output_Label->setText("Charactersitic Length (m):");

    }
    else if(index==1) {
        ui->eta_x_edit->setVisible(false);
        ui->eta_y_edit->setVisible(false);
        ui->eta_z_edit->setVisible(false);
        ui->eta_x_label->setVisible(false);
        ui->eta_y_label->setVisible(false);
        ui->eta_z_label->setVisible(false);

        ui->Tort_Output_Label->setVisible(true);
        ui->TortuosityXLabel->setVisible(true);
        ui->TortuosityYLabel->setVisible(true);
        ui->TortuosityZLabel->setVisible(true);

        ui->Diff_Output_Label->setText("Diffusivity:");
    }

    this->repaint();
}
