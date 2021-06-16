#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>

#include "puma.h"
#include "../src/Utilities/viewer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void action_EntireImage_Triggered();
    void action_Threshold_Triggered();
    void action_Export3DTiff_Triggered();
    void action_ExportSTL_Triggered();
    void action_Crop_Triggered();

    void on_verticalSlider_valueChanged(int value);
    void on_actionStraight_Circle_triggered();
    void on_actionCurved_Circle_triggered();
    void on_actionStraight_Flower_triggered();
    void on_actionCurved_Flower_triggered();
    void on_actionRandom_Spheres_triggered();
    void on_actionMedian_3D_triggered();
    void on_actionMean_3D_triggered();
    void on_actionBilateral_triggered();
    void on_actionSurface_Area_triggered();
    void on_actionElectrical_Conductivity_triggered();

    void on_actionOpen_Log_triggered();
    void on_actionThermal_Conductivity_triggered();
    void on_actionContinuum_triggered();
    void on_actionPorosity_triggered();
    void on_actionVolume_Fraction_triggered();
    void on_actionRandom_Walk_triggered();

    void on_action3D_Raw_triggered();
    void on_action3D_Raw_Export_triggered();
    void on_actionVTK_Export_triggered();

    void on_actionTPMS_triggered();
    void on_actionSphere_triggered();
    void on_actionBox_triggered();
    void on_actionCylinder_triggered();

    void on_actionPorous_Media_Burner_Generation_triggered();
    void on_actionX_Direction_triggered();
    void on_actionY_Direction_triggered();
    void on_actionZ_Direction_triggered();

    void on_actionPaper_1_triggered();
    void on_actionPaper_2_triggered();
    void on_actionPuMA_Overview_triggered();
    void on_actionApplication_triggered();
    void on_actionAnisotropic_Conductivity_Paper_triggered();
    void on_actionOrientation_Estimation_triggered();

    void on_updateVoxelLength_Button_clicked();
    void on_actionVisualization_triggered();

    void on_actionPuMApy_triggered();

private:
    Ui::MainWindow *ui;

    QPixmap pixmap;
    puma::Workspace workspace;

    Viewer view;

    void createActions();

    void resizeLabelContents();


protected:
    void resizeEvent(QResizeEvent *event);
    void dragEnterEvent(QDragEnterEvent *ev);
    void dropEvent(QDropEvent *ev);
};

#endif // MAINWINDOW_H
