/*
 Copyright @ 2017, 2020, 2021 United States Government as represented by the Administrator of the National Aeronautics and Space Administration. All Rights Reserved.
 This software may be used, reproduced, and provided to others only as permitted under the terms of the agreement under which it was acquired from the U.S. Government. Neither title to, nor ownership of, the software is hereby transferred. This notice shall remain on all copies of the software.
 This file is available under the terms of the NASA Open Source Agreement (NOSA), and further subject to the additional disclaimer below:
 Disclaimer:
 THE SOFTWARE AND/OR TECHNICAL DATA ARE PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SOFTWARE AND/OR TECHNICAL DATA WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT THE SOFTWARE AND/OR TECHNICAL DATA WILL BE ERROR FREE, OR ANY WARRANTY THAT TECHNICAL DATA, IF PROVIDED, WILL CONFORM TO THE SOFTWARE. IN NO EVENT SHALL THE UNITED STATES GOVERNMENT, OR ITS CONTRACTORS OR SUBCONTRACTORS, BE LIABLE FOR ANY DAMAGES, INCLUDING, BUT NOT LIMITED TO, DIRECT, INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES, ARISING OUT OF, RESULTING FROM, OR IN ANY WAY CONNECTED WITH THIS SOFTWARE AND/OR TECHNICAL DATA, WHETHER OR NOT BASED UPON WARRANTY, CONTRACT, TORT, OR OTHERWISE, WHETHER OR NOT INJURY WAS SUSTAINED BY PERSONS OR PROPERTY OR OTHERWISE, AND WHETHER OR NOT LOSS WAS SUSTAINED FROM, OR AROSE OUT OF THE RESULTS OF, OR USE OF, THE SOFTWARE AND/OR TECHNICAL DATA.
 THE UNITED STATES GOVERNMENT DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD PARTY COMPUTER SOFTWARE, DATA, OR DOCUMENTATION, IF SAID THIRD PARTY COMPUTER SOFTWARE, DATA, OR DOCUMENTATION IS PRESENT IN THE NASA SOFTWARE AND/OR TECHNICAL DATA, AND DISTRIBUTES IT "AS IS."
 RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT AND ITS CONTRACTORS AND SUBCONTRACTORS, AND SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT AND ITS CONTRACTORS AND SUBCONTRACTORS FOR ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES THAT MAY ARISE FROM RECIPIENT'S USE OF THE SOFTWARE AND/OR TECHNICAL DATA, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, THE USE THEREOF.
 IF RECIPIENT FURTHER RELEASES OR DISTRIBUTES THE NASA SOFTWARE AND/OR TECHNICAL DATA, RECIPIENT AGREES TO OBTAIN THIS IDENTICAL WAIVER OF CLAIMS, INDEMNIFICATION AND HOLD HARMLESS, AGREEMENT WITH ANY ENTITIES THAT ARE PROVIDED WITH THE SOFTWARE AND/OR TECHNICAL DATA.
 */

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
    void on_actionPaper_3_triggered();
    void on_actionPaper_4_triggered();
    void on_actionApplication_triggered();
    void on_actionAnisotropic_Conductivity_Paper_triggered();
    void on_actionOrientation_Estimation_triggered();

    void on_updateVoxelLength_Button_clicked();
    void on_actionVisualization_triggered();


    void on_actionOverview_Talk_triggered();


    void on_rescale_vis_button_clicked();

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
