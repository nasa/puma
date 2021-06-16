#ifndef POROUSMEDIABURNERGENERATOR_H
#define POROUSMEDIABURNERGENERATOR_H

#include <QDialog>
#include <iostream>
#include <QFileDialog>

#include "puma.h"
#include "../src/Utilities/viewer.h"

namespace Ui {
class PorousMediaBurnerGenerator;
}

class PorousMediaBurnerGenerator : public QDialog
{
    Q_OBJECT

public:
    explicit PorousMediaBurnerGenerator(QWidget *parent = 0);
    void setViewer(Viewer *view);
    ~PorousMediaBurnerGenerator();

private slots:
    void on_Generate_clicked();

    void on_diameterEdit_textChanged(const QString &arg1);

    void on_heightEdit_textChanged(const QString &arg1);

    void on_scaleEdit_textChanged(const QString &arg1);

    void editOutputs();

private:
    Ui::PorousMediaBurnerGenerator *ui;
    Viewer *view;

};

#endif // POROUSMEDIABURNERGENERATOR_H
