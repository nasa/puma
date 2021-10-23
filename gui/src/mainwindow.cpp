#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Export/exportstl.h"

#include "Generate/randomfiberstraightcircle.h"
#include "Generate/randomfibergencurvedcircle.h"
#include "Generate/randomfibergenstraightflower.h"
#include "Generate/randomfibersgencurvedflower.h"
#include "Generate/randomspheresgenerator.h"
#include "Generate/tpmsgenerator.h"
#include "Generate/spheregenerator.h"
#include "Generate/boxgenerator.h"
#include "Generate/cylindergenerator.h"

#include "Plugins/porousmediaburnergenerator.h"

#include "Log/openlog.h"

#include "MaterialProperties/electricalcond.h"
#include "MaterialProperties/porosity.h"
#include "MaterialProperties/surface_area.h"
#include "MaterialProperties/thermalcond.h"
#include "MaterialProperties/tortuositycontinuum.h"
#include "MaterialProperties/tortuosityrandomwalk.h"
#include "MaterialProperties/volumefraction.h"

#include "Visualization/Visualization.h"

#include "Process/bilateralfilt.h"
#include "Process/crop.h"
#include "Process/mean3dfilter.h"
#include "Process/median3dfilter.h"
#include "Process/threshold.h"

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <iostream>
#include <QImage>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QList>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QInputDialog>
#include <QString>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {


    ui->setupUi(this);

    QString startPage = QString::fromStdString(":/images/StartPage_InvertedBlurred.jpg");

    pixmap.load(startPage);
    ui->mainLabel->setPixmap(pixmap.scaledToHeight(600, Qt::SmoothTransformation));

    ui->menuBar->setNativeMenuBar(false);

    if(pixmap.isNull())
        std::cout << "Null Pixmap" << std::endl;

    //added so that the startup image loads properly
    ui->mainLabel->resize(800,600);
    ui->mainLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    createActions();

    view.setParameters(&workspace,ui->mainLabel, ui->sliceLabel,ui->verticalSlider,&pixmap,ui->x_size, ui->y_size, ui->z_size, ui->gray_min, ui->gray_max);
    ui->sliceLabel->setText(QString::number(0));

    setAcceptDrops(true);

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    resizeLabelContents();
    QWidget::resizeEvent(event);

}

void MainWindow::resizeLabelContents() {
    view.updatePixmap();
}

void MainWindow::on_verticalSlider_valueChanged(int value)
{

    int position = value;

    if(workspace.Z() == 0 ) {
        return;
    }

    double ratio = (double)workspace.Z()/100.0;

    int slice = (int) (ratio * position);

    view.setTomographySlice(slice);

}


void MainWindow::dropEvent(QDropEvent *ev)
{
    std::cout << "Dopped" << std::endl;

    QList<QUrl> urls = ev->mimeData()->urls();

    if(urls.first().toString().endsWith(".tif")||urls.first().toString().endsWith(".tiff")){//dragged in a 3D tiff file
        QString fileName_Tiff;
        fileName_Tiff=urls.first().toString();
        fileName_Tiff.replace("file://","");
        puma::import_3DTiff(&workspace,fileName_Tiff.toStdString(),0);
        view.setTomographySlice(0);

    }else if(urls.first().toString().endsWith(".puma")){//dragged in an input file
        QString fileName_bin;
        fileName_bin=urls.first().toString();
        fileName_bin.replace("file://","");
        puma::import_bin(&workspace,fileName_bin.toStdString(),0);
        view.setTomographySlice(0);
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *ev)
{
    ev->accept();
}

/* ////////////////////////////////////////////////////////////////////////////
Import menu tab. The drop down menu-items include:
1. 3D Tiff
2. Tiff Stack
3. 3D Raw (Binary)
4. STL
5. VTK
///////////////////////////////////////////////////////////////////////////// */

/* /////////////////////////////////////////////////////////////////
Import -> 3D Tiff
 ////////////////////////////////////////////////////////////////*/
void MainWindow::action_EntireImage_Triggered() {
    std::string fileName_Tiff = QFileDialog::getOpenFileName(this,"Location of 3D Tiff File", ".").toStdString();

    if(puma::import_3DTiff(&workspace,fileName_Tiff,0)){
        view.setTomographySlice(0);
    }


}
/* /////////////////////////////////////////////////////////////////
Import -> Tiff Stack
Not a feature in PuMA library yet
////////////////////////////////////////////////////////////////*/

/* /////////////////////////////////////////////////////////////////
Import -> 3D Raw (binary)
////////////////////////////////////////////////////////////////*/
void MainWindow::on_action3D_Raw_triggered()
{
    std::string fileName_bin= QFileDialog::getOpenFileName(this,"Location of PuMA File", ".").toStdString();
    puma::import_bin(&workspace,fileName_bin,0);
    view.setTomographySlice(0);
}


/* ////////////////////////////////////////////////////////////////////////////
Export menu tab. The drop down menu-items include:
1. 3D Tiff
2. Tiff Stack
3. 3D Raw (Binary)
4. STl
5. VTK
///////////////////////////////////////////////////////////////////////////// */

/* /////////////////////////////////////////////////////////////////
Export -> 3D Tiff
 ////////////////////////////////////////////////////////////////*/
void MainWindow::action_Export3DTiff_Triggered() {

    if(workspace.size() == 0 ) {
        std::cout << "Label 1.00" << std::endl;
        return;
    }
    std::cout << "Label 1.01" << std::endl;

    std::string fileName_Tiff = QFileDialog::getSaveFileName(this,"Export File Name", ".").toStdString();

    puma::export_3DTiff(&workspace,fileName_Tiff, false);
}

/* /////////////////////////////////////////////////////////////////
Export -> 3D Raw (binary)
////////////////////////////////////////////////////////////////*/
void MainWindow::on_action3D_Raw_Export_triggered()
{
    if(workspace.size() == 0 ) {
        return;
    }
    std::string fileName_Binary = QFileDialog::getSaveFileName(this,"Export File Name", ".").toStdString();
    if(fileName_Binary != "") {
        puma::export_bin(&workspace,fileName_Binary);
    }

}

/* /////////////////////////////////////////////////////////////////
Export -> STL
 ////////////////////////////////////////////////////////////////*/
void MainWindow::action_ExportSTL_Triggered() {

    if(workspace.size() == 0 ) {
        return;
    }

    ExportSTL *exportSTLWindow = new ExportSTL(this);
    exportSTLWindow->setWorkspace(&workspace);
    exportSTLWindow->show();

}

/* /////////////////////////////////////////////////////////////////
Export -> VTK
 ////////////////////////////////////////////////////////////////*/
void MainWindow::on_actionVTK_Export_triggered()
{
    if(workspace.size() == 0 ) {
        return;
    }
    std::string fileName_VTK = QFileDialog::getSaveFileName(this,"Export File Name", ".").toStdString();

    if(fileName_VTK != "") {
        puma::export_vtk(&workspace,fileName_VTK);
    }
}


/* /////////////////////////////////////////////////////////////////
Generate -> Random Fibers -> Straight Circle
 ////////////////////////////////////////////////////////////////*/
void MainWindow::on_actionStraight_Circle_triggered() {
    RandomFiberStraightCircle *StraightCircleGenWindow = new RandomFiberStraightCircle(this);
    StraightCircleGenWindow->setViewer(&view);
    StraightCircleGenWindow->show();
}

/* /////////////////////////////////////////////////////////////////
Generate -> Random Fibers -> Curved Circle
 ////////////////////////////////////////////////////////////////*/
void MainWindow::on_actionCurved_Circle_triggered(){
    RandomFiberGenCurvedCircle *CurvedCircleGenWindow = new RandomFiberGenCurvedCircle(this);
    CurvedCircleGenWindow->setViewer(&view);
    CurvedCircleGenWindow->show();
}

/* /////////////////////////////////////////////////////////////////
Generate -> Random Fibers -> Straight Flower
 ////////////////////////////////////////////////////////////////*/
void MainWindow::on_actionStraight_Flower_triggered(){
    RandomFiberGenStraightFlower *StraightFlowerGenWindow = new RandomFiberGenStraightFlower(this);
    StraightFlowerGenWindow->setViewer(&view);
    StraightFlowerGenWindow->show();
}

/* /////////////////////////////////////////////////////////////////
Generate -> Random Fibers -> Curved Flower
 ////////////////////////////////////////////////////////////////*/
void MainWindow::on_actionCurved_Flower_triggered(){
    RandomFibersGenCurvedFlower *CurvedFlowerGenWindow = new RandomFibersGenCurvedFlower(this);
    CurvedFlowerGenWindow->setViewer(&view);
    CurvedFlowerGenWindow->show();
}

/* /////////////////////////////////////////////////////////////////
Generate -> Random Spheres
//////////////////////////////////////////////////////////////////*/
void MainWindow::on_actionRandom_Spheres_triggered(){
    RandomSpheresGenerator *spheresGenWindow = new RandomSpheresGenerator(this);
    spheresGenWindow->setViewer(&view);
    spheresGenWindow->show();
}

void MainWindow::on_actionTPMS_triggered(){
    TPMSGenerator *tpmsGenWindow = new TPMSGenerator(this);
    tpmsGenWindow->setViewer(&view);
    tpmsGenWindow->show();
}

void MainWindow::on_actionSphere_triggered(){
    SphereGenerator *sphereGenWindow = new SphereGenerator(this);
    sphereGenWindow->setViewer(&view);
    sphereGenWindow->show();
}

void MainWindow::on_actionCylinder_triggered(){
    CylinderGenerator *cylinderGenWindow = new CylinderGenerator(this);
    cylinderGenWindow->setViewer(&view);
    cylinderGenWindow->show();
}

void MainWindow::on_actionBox_triggered(){
    BoxGenerator *boxGenWindow = new BoxGenerator(this);
    boxGenWindow->setViewer(&view);
    boxGenWindow->show();
}

void MainWindow::on_actionPorous_Media_Burner_Generation_triggered()
{
    PorousMediaBurnerGenerator *tpbGenWindow = new PorousMediaBurnerGenerator(this);
    tpbGenWindow->setViewer(&view);
    tpbGenWindow->show();
}


/* //////////////////////////////////////////////////////////////////
Process menu tab. The drop down menu items include:
1. Crop
2. Filter
3.Segmentation
//////////////////////////////////////////////////////////////////*/

/* /////////////////////////////////////////////////////////////////
Process -> Crop
////////////////////////////////////////////////////////////////*/

void MainWindow::action_Crop_Triggered() {
    Crop *cropWindow = new Crop(this);
    cropWindow->setViewer(&view);
    cropWindow->show();
}

/* /////////////////////////////////////////////////////////////////
Below are the filter tabs and their actions
1. Median 3D
2. Mean 3D
3. Bilateral
 ////////////////////////////////////////////////////////////////*/


/* /////////////////////////////////////////////////////////////////
Process -> Filter -> Median 3D
////////////////////////////////////////////////////////////////*/
void MainWindow::on_actionMedian_3D_triggered(){
    Median3DFilter *median3DFilterWindow = new Median3DFilter(this);
    median3DFilterWindow->setViewer(&view);
    median3DFilterWindow->show();
}


/* /////////////////////////////////////////////////////////////////
Process -> Filter -> Mean 3D
////////////////////////////////////////////////////////////////*/
void MainWindow::on_actionMean_3D_triggered()
{
    Mean3DFilter *mean3DFilterWindow = new Mean3DFilter(this);
    mean3DFilterWindow->setViewer(&view);
    mean3DFilterWindow->show();
}

/* /////////////////////////////////////////////////////////////////
Process -> Filter -> Bilateral
////////////////////////////////////////////////////////////////*/
void MainWindow::on_actionBilateral_triggered()
{
    BilateralFilt *bilateralFilterWindow = new BilateralFilt(this);
    bilateralFilterWindow->setViewer(&view);
    bilateralFilterWindow->show();
}

/* /////////////////////////////////////////////////////////////////
Segmentation Tab. The drop down menu items include:
1. Threshold
////////////////////////////////////////////////////////////////*/

/* /////////////////////////////////////////////////////////////////
Process -> Segmentation -> Threshold
////////////////////////////////////////////////////////////////*/
void MainWindow::action_Threshold_Triggered() {
    Threshold *thresholdWindow = new Threshold(this);
    thresholdWindow->setViewer(&view);
    thresholdWindow->histogram();
    thresholdWindow->show();
}

void MainWindow::createActions() {
    connect(ui->actionEntireImage, SIGNAL(triggered()), this, SLOT(action_EntireImage_Triggered()));
    connect(ui->actionThreshold, SIGNAL(triggered()), this, SLOT(action_Threshold_Triggered()));
    connect(ui->actionExport3DTiff, SIGNAL(triggered()), this, SLOT(action_Export3DTiff_Triggered()));
    connect(ui->actionBinary_Marching_Cbues, SIGNAL(triggered()), this, SLOT(action_ExportSTL_Triggered()));
    connect(ui->actionCrop, SIGNAL(triggered()), this, SLOT(action_Crop_Triggered()));
}

/* ////////////////////////////////////////////////////////////////////////////
Material Properties menu tab. The drop down menu-items include:
1. Porosity
2. Volume Fraction
3. Surface Area
4. Thermal Conductivity
5. Electrical Conductivity
6. Anisotropic Conductivity
7. Fiber Direction
8. Permeability
9. Tortuosity
///////////////////////////////////////////////////////////////////////////// */

/* /////////////////////////////////////////////////////////////////
Material Properties -> Porosity
////////////////////////////////////////////////////////////////*/
void MainWindow::on_actionPorosity_triggered()
{
    Porosity *porosityWindow = new Porosity(this);
    porosityWindow->setWorkspace(&workspace);
    porosityWindow->show();
}

/* /////////////////////////////////////////////////////////////////
Material Properties -> Volume Fraction
////////////////////////////////////////////////////////////////*/
void MainWindow::on_actionVolume_Fraction_triggered()
{
    VolumeFraction *volumeFractionWindow = new VolumeFraction(this);
    volumeFractionWindow->setWorkspace(&workspace);
    volumeFractionWindow->show();
}

/* /////////////////////////////////////////////////////////////////
Material Properties -> Surface Area
////////////////////////////////////////////////////////////////*/
void MainWindow::on_actionSurface_Area_triggered()
{
    Surface_Area *SurfaceAreaWindow = new Surface_Area(this);
    SurfaceAreaWindow->setWorkspace(&workspace);
    SurfaceAreaWindow->show();
}

/* /////////////////////////////////////////////////////////////////
Material Properties -> Electrical Conductivity
////////////////////////////////////////////////////////////////*/
void MainWindow::on_actionElectrical_Conductivity_triggered()
{
    ElectricalCond *electricalConductWindow = new ElectricalCond(this);
    electricalConductWindow->setViewer(&view);
    electricalConductWindow->show();
}

/* /////////////////////////////////////////////////////////////////
Material Properties -> Fiber Direction tab

/* /////////////////////////////////////////////////////////////////
Material Properties -> Thermal Conductivity
////////////////////////////////////////////////////////////////*/
void MainWindow::on_actionThermal_Conductivity_triggered()
{
    ThermalCond *thermalConductWindow = new ThermalCond(this);
    thermalConductWindow->setViewer(&view);
    thermalConductWindow->show();
}

/* /////////////////////////////////////////////////////////////////
Material Properties -> Tortuosity tab
1. Continuum
2. Random Walk
////////////////////////////////////////////////////////////////*/

/* /////////////////////////////////////////////////////////////////
Tortuosity -> Continuum
////////////////////////////////////////////////////////////////*/

void MainWindow::on_actionContinuum_triggered()
{
    tortuosityContinuum *tortuosityContinuumWindow = new tortuosityContinuum(this);
    tortuosityContinuumWindow->setWorkspace(&workspace);
    tortuosityContinuumWindow->show();
}

/* /////////////////////////////////////////////////////////////////
Tortuosity -> Random Walk
////////////////////////////////////////////////////////////////*/
void MainWindow::on_actionRandom_Walk_triggered()
{
    tortuosityRandomWalk *tortuosityRandomWalkWindow = new tortuosityRandomWalk(this);
    tortuosityRandomWalkWindow->setWorkspace(&workspace);
    tortuosityRandomWalkWindow->show();

}


/* ////////////////////////////////////////////////////////////////////////////
Log menu tab. The drop down menu-items include:
1. Open Log
///////////////////////////////////////////////////////////////////////////// */

/* /////////////////////////////////////////////////////////////////
Log -> Open Log
////////////////////////////////////////////////////////////////*/
void MainWindow::on_actionOpen_Log_triggered()
{
    OpenLog *logWindow = new OpenLog(this);
    logWindow->setWorkspace(&workspace);
    logWindow->show();
}

void MainWindow::on_actionX_Direction_triggered() {

    if(view.workspace->Y() != view.workspace->Z()) {
        view.error("Y and Z size must be the same in order to circularize in X");
    }

    int radius = view.workspace->Y() / 2;

    for(int i=0;i<view.workspace->X();i++){
        for(int j=0;j<view.workspace->Y();j++){
            for(int k=0;k<view.workspace->Z();k++){
                if(std::sqrt((j-view.workspace->Y()/2.0)*(j-view.workspace->Y()/2.0) + (k-view.workspace->Z()/2.0)*(k-view.workspace->Z()/2.0)) > radius ){
                    view.workspace->matrix(i,j,k) = 0;
                }
            }
        }
    }

    view.setTomographySlice(0);

}

void MainWindow::on_actionY_Direction_triggered() {

    if(view.workspace->X() != view.workspace->Z()) {
        view.error("X and Z size must be the same in order to circularize in Y");
    }

    int radius = view.workspace->X() / 2;

    for(int i=0;i<view.workspace->X();i++){
        for(int j=0;j<view.workspace->Y();j++){
            for(int k=0;k<view.workspace->Z();k++){
                if(std::sqrt((i-view.workspace->X()/2.0)*(i-view.workspace->X()/2.0) + (k-view.workspace->Z()/2.0)*(k-view.workspace->Z()/2.0)) > radius ){
                    view.workspace->matrix(i,j,k) = 0;
                }
            }
        }
    }

    view.setTomographySlice(0);

}

void MainWindow::on_actionZ_Direction_triggered() {

    if(view.workspace->X() != view.workspace->Y()) {
        view.error("X and Y size must be the same in order to circularize in Z");
    }

    int radius = view.workspace->X() / 2;

    for(int i=0;i<view.workspace->X();i++){
        for(int j=0;j<view.workspace->Y();j++){
            for(int k=0;k<view.workspace->Z();k++){
                if(std::sqrt((i-view.workspace->X()/2.0)*(i-view.workspace->X()/2.0) + (j-view.workspace->Y()/2.0)*(j-view.workspace->Y()/2.0)) > radius ){
                    view.workspace->matrix(i,j,k) = 0;
                }
            }
        }
    }

    view.setTomographySlice(0);

}

void MainWindow::on_actionPaper_1_triggered() {
    QString link = "https://www.sciencedirect.com/science/article/pii/S0008622315302153";
    QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_actionPaper_2_triggered() {
    QString link = "https://www.sciencedirect.com/science/article/pii/S0008622317305808";
    QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_actionApplication_triggered() {
    QString link = "https://www.sciencedirect.com/science/article/pii/S0017931016327235";
    QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_actionAnisotropic_Conductivity_Paper_triggered() {
    QString link = "https://www.sciencedirect.com/science/article/pii/S092702562030447X";
    QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_actionOrientation_Estimation_triggered() {
    QString link = "https://www.sciencedirect.com/science/article/pii/S0927025620301221";
    QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_actionPaper_3_triggered()
{
    QString link = "https://www.sciencedirect.com/science/article/pii/S2352711018300281";
    QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_actionPaper_4_triggered()
{
    QString link = "https://www.sciencedirect.com/science/article/pii/S235271102100090X";
    QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_actionOverview_Talk_triggered()
{
    QString link = "https://www.nas.nasa.gov/pubs/ams/2020/09-29-20.html";
    QDesktopServices::openUrl(QUrl(link));
}



void MainWindow::on_updateVoxelLength_Button_clicked()
{
    double voxelLength = QInputDialog::getText(this, tr("Input Voxel Length"),
                                       tr("Voxel Length in meters: (ex 1e-6)")).toDouble();

    if(voxelLength <= 0) {
        view.error("Invalid voxel length input. Must be > 0. Typically between 1e-7 and 1e-3");
        return;
    }
    if(voxelLength > 1) {
        view.warning("Very large voxel length. Units are in meters. Typical values are between 1e-7 and 1e-3");
    }

    workspace.voxelLength = voxelLength;
    ui->voxelLength->setText(QString::number(workspace.voxelLength));

}


void MainWindow::on_actionVisualization_triggered()
{
    Visualization *visWindow = new Visualization(this);
    visWindow->setWorkspace(&workspace);
    visWindow->show();
}




void MainWindow::on_rescale_vis_button_clicked()
{
    if(view.workspace->size()!=0) {
        view.min = view.workspace->matrix.min();
        view.max = view.workspace->matrix.max();
        ui->gray_min->setText(QString::number(view.min));
        ui->gray_max->setText(QString::number(view.max));
        view.refresh();
        this->repaint();
    }

}

