#ifndef ElectricalCOND_H
#define ElectricalCOND_H


#include <QDialog>
#include <QDebug>
#include <QTextEdit>
#include "../src/Utilities/q_debugstream.h"
#include "puma.h"
#include <QThread>

#include <QProcess>
#include <QTextBrowser>

#include "../src/Utilities/viewer.h"


namespace Ui {
class ElectricalCond;
}

class ElectricalCond : public QDialog
{
    Q_OBJECT

public:
    explicit ElectricalCond(QWidget *parent = 0);
    void setViewer(Viewer *view);
    ~ElectricalCond();

    // QMessage
    /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    static  void QMessageOutput(QtMsgType , const QMessageLogContext &, const QString &msg);
    /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

private slots:
    void on_CalculateButton_clicked();

    void on_comboBox_currentIndexChanged(int index);



    void on_vtkButton_clicked();

    void on_binaryButton_clicked();

private:
    Ui::ElectricalCond *ui;
    puma::Workspace *workspace;
    Viewer *view;

    void Calculate();

    // MessageHandler for display and ThreadLogStream for redirecting cout
    /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    MessageHandler *msgHandler = Q_NULLPTR;
    ThreadLogStream* m_qd;

    puma::Matrix<double> Tx;
    puma::Matrix<double> Ty;
    puma::Matrix<double> Tz;



    /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
};

#endif // ElectricalCOND_H
