#include "tortuosityprinter.h"

TortuosityPrinter::TortuosityPrinter(QCustomPlot *plot, QApplication *myApp)
{
    myPlot = plot;
    myApp = qApp;

    y_MSD0.push_back(0);
    y_MSD1.push_back(0);
    y_MSD2.push_back(0);
    x_time.push_back(0);

}

void TortuosityPrinter::passData(std::string tag, std::vector<double> data) {

    y_MSD0.push_back(data[0]);
    y_MSD1.push_back(data[1]);
    y_MSD2.push_back(data[2]);
    x_time.push_back(data[3]);

    myPlot->clearGraphs();
    myPlot->xAxis->setLabel("Time (s)");
    myPlot->yAxis->setLabel("Mean Square Disp. (m^2)");
    myPlot->addGraph(0);
    myPlot->addGraph(0);
    myPlot->addGraph(0);
    myPlot->graph(0)->setData(x_time, y_MSD0);
    myPlot->graph(1)->setData(x_time, y_MSD1);
    myPlot->graph(2)->setData(x_time, y_MSD2);
    myPlot->yAxis->setTickLabels(true);
    myPlot->xAxis->setTickLabels(true);
    double maxValue=std::max(y_MSD0[y_MSD0.size()-1],y_MSD2[y_MSD2.size()-1]);
    maxValue=std::max(maxValue,y_MSD1[y_MSD1.size()-1]);
    myPlot->xAxis->setRange(0, x_time[x_time.size()-1]);
    myPlot->yAxis->setRange(0, maxValue);
    myPlot->replot();

    myApp->processEvents();


}
