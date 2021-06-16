#ifndef EXPORTSTL_H
#define EXPORTSTL_H

#include <QDialog>

#include "puma.h"

namespace Ui {
class ExportSTL;
}

class ExportSTL : public QDialog
{
    Q_OBJECT

public:
    explicit ExportSTL(QWidget *parent = 0);
    void setWorkspace(puma::Workspace *workspace);
    ~ExportSTL();

private slots:


    void on_lowEdit_textChanged(const QString &arg1);

    void on_highEdit_textChanged(const QString &arg1);

    void on_pushButton_clicked();

private:
    Ui::ExportSTL *ui;

    puma::Workspace *workspace;

};

#endif // EXPORTSTL_H
