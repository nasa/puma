#ifndef Visualization_H
#define Visualization_H

#include <QDialog>
#include "puma.h"

namespace Ui {
class Visualization;
}

class Visualization : public QDialog
{
    Q_OBJECT

public:
    explicit Visualization(QWidget *parent = 0);
    void setWorkspace(puma::Workspace *workspace);
    ~Visualization();

private slots:
    void on_visContourButton_clicked();

    void on_visVolumeButton_clicked();

    void launchPython(int visType);

private:
    Ui::Visualization *ui;
    puma::Workspace *workspace;
};

#endif // Visualization_H
