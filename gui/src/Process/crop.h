#ifndef CROP_H
#define CROP_H

#include <QDialog>

#include "../src/Utilities/viewer.h"
#include "puma.h"

namespace Ui {
class Crop;
}

class Crop : public QDialog
{
    Q_OBJECT

public:
    explicit Crop(QWidget *parent = nullptr);
    void setViewer(Viewer *view);
    ~Crop();

private slots:
    void on_applyButton_clicked();

    void on_x2_Edit_textChanged(const QString &arg1);

    void on_x1_Edit_textChanged(const QString &arg1);

    void on_y2_Edit_textChanged(const QString &arg1);

    void on_y1_Edit_textChanged(const QString &arg1);

    void on_z2_Edit_textChanged(const QString &arg1);

    void on_z1_Edit_textChanged(const QString &arg1);



    void on_previewButton_clicked();

private:
    Ui::Crop *ui;
    Viewer *view;

    void updateWindow();

};

#endif // CROP_H
