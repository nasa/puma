#ifndef MEDIAN3DFILTER_H
#define MEDIAN3DFILTER_H

#include <QDialog>
#include "../src/Utilities/viewer.h"
#include "puma.h"

namespace Ui {
class Median3DFilter;
}

class Median3DFilter : public QDialog
{
    Q_OBJECT

public:
    explicit Median3DFilter(QWidget *parent = 0);
    void setViewer(Viewer *view);
    ~Median3DFilter();

private slots:
    void on_previewButton_clicked();

    void on_originalButton_clicked();

    void on_applyButton_clicked();

private:
    Ui::Median3DFilter *ui;
    Viewer *view;
    int numThreads;
};

#endif // MEDIAN3DFILTER_H
