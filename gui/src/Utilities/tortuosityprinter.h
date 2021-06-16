#ifndef TortuosityPRINTER_H
#define TortuosityPRINTER_H

#include "puma.h"

#include <string>
#include <vector>
#include <QVector>

#include "../Utilities/qcustomplot/qcustomplot.h"

#include <QApplication>

class TortuosityPrinter : public puma::Printer
{
public:
    TortuosityPrinter(QCustomPlot *plot, QApplication *myApp);

    virtual void passData(std::string tag, std::vector<double> data) override;

private:
    QCustomPlot *myPlot;
    QApplication *myApp;

    QVector<double> y_MSD0;
    QVector<double> y_MSD1;
    QVector<double> y_MSD2;
    QVector<double> x_time;

};

#endif // TortuosityPRINTER_H
