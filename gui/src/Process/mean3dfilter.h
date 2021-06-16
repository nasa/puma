#ifndef MEAN3DFILTER_H
#define MEAN3DFILTER_H

#include <QDialog>
#include "../src/Utilities/viewer.h"
#include "puma.h"

namespace Ui {
class Mean3DFilter;
}

class Mean3DFilter : public QDialog
{
    Q_OBJECT

public:
    explicit Mean3DFilter(QWidget *parent = 0);
    void setViewer(Viewer *view);
    ~Mean3DFilter();

private slots:


    void on_previewButton_clicked();

    void on_originalButton_clicked();

    void on_applyButton_clicked();

private:
    Ui::Mean3DFilter *ui;
    Viewer *view;
    int numThreads;
};

#endif // MEAN3DFILTER_H
