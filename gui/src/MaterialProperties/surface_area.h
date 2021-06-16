#ifndef SURFACE_AREA_H
#define SURFACE_AREA_H

#include <QDialog>
#include <QObject>
#include <QString>
#include "puma.h"

namespace Ui {
class Surface_Area;
}

class Surface_Area : public QDialog
{
    Q_OBJECT

public:
    explicit Surface_Area(QWidget *parent = 0);
    void setWorkspace(puma::Workspace *workspace);
    ~Surface_Area();

private slots:
    void on_pushButton_clicked();
    void on_comboBox_currentIndexChanged(int index);

private:

    Ui::Surface_Area *ui;
    puma::Workspace *workspace;

};

#endif // SURFACE_AREA_H
