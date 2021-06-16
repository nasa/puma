#ifndef BILATERALFILT_H
#define BILATERALFILT_H

#include <QDialog>
#include "../src/Utilities/viewer.h"
#include "puma.h"


namespace Ui {
class BilateralFilt;
}

class BilateralFilt : public QDialog
{
    Q_OBJECT

public:
    explicit BilateralFilt(QWidget *parent = nullptr);
    void setViewer(Viewer *view);
    ~BilateralFilt();

private slots:
    void on_previewButton_clicked();

    void on_originalButton_clicked();

    void on_applyButton_clicked();

private:
    Ui::BilateralFilt *ui;
    Viewer *view;
    int numThreads;
};

#endif // BILATERALFILT_H
