#ifndef VOLUMEFRACTION_H
#define VOLUMEFRACTION_H

#include <QDialog>
#include "puma.h"

namespace Ui {
class VolumeFraction;
}

class VolumeFraction : public QDialog
{
    Q_OBJECT

public:
    explicit VolumeFraction(QWidget *parent = 0);
    void setWorkspace(puma::Workspace *workspace);
    ~VolumeFraction();

private slots:
    void on_calculateButton_clicked();

private:
    Ui::VolumeFraction *ui;
    puma::Workspace *workspace;
};

#endif // VOLUMEFRACTION_H
