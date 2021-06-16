#ifndef BoxGenerator_H
#define BoxGenerator_H

#include <QDialog>
#include <iostream>
#include <QFileDialog>

#include "puma.h"

#include "../src/Utilities/viewer.h"

namespace Ui {
class BoxGenerator;
}

class BoxGenerator : public QDialog
{
    Q_OBJECT

public:
    explicit BoxGenerator(QWidget *parent = nullptr);
    void setViewer(Viewer *view);
    ~BoxGenerator();

private slots:
    void on_Generate_clicked();

private:
    Ui::BoxGenerator *ui;
    Viewer *view;


};

#endif // BoxGenerator_H
