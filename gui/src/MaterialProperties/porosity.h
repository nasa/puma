#ifndef POROSITY_H
#define POROSITY_H

#include <QDialog>
#include "puma.h"

namespace Ui {
class Porosity;
}

class Porosity : public QDialog
{
    Q_OBJECT

public:
    explicit Porosity(QWidget *parent = 0);
    void setWorkspace(puma::Workspace *workspace);
    ~Porosity();

private slots:

    void on_calculateButton_clicked();

private:
    Ui::Porosity *ui;
    puma::Workspace *workspace;
    int numThreads;
};

#endif // POROSITY_H
