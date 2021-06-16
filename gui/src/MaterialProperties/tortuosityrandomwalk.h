#ifndef TORTUOSITYRANDOMWALK_H
#define TORTUOSITYRANDOMWALK_H

#include <QDialog>
#include "puma.h"

namespace Ui {
class tortuosityRandomWalk;
}

class tortuosityRandomWalk : public QDialog
{
    Q_OBJECT

public:
    explicit tortuosityRandomWalk(QWidget *parent = 0);
    void setWorkspace(puma::Workspace *workspace);
    ~tortuosityRandomWalk();

private slots:
    void on_pushButton_clicked();

    void on_comboBox_2_currentIndexChanged(int index);

private:
    Ui::tortuosityRandomWalk *ui;
    puma::Workspace *workspace; 

};

#endif // TORTUOSITYRANDOMWALK_H
