#ifndef TIFFIMPORT_3D_H
#define TIFFIMPORT_3D_H

#include <QDialog>

#include "puma.h"

namespace Ui {
class TiffImport_3D;
}

class TiffImport_3D : public QDialog
{
    Q_OBJECT

public:
    explicit TiffImport_3D(QWidget *parent = 0);
    ~TiffImport_3D();

private:
    Ui::TiffImport_3D *ui;


};

#endif // TIFFIMPORT_3D_H
