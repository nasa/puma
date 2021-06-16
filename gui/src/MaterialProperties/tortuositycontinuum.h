#ifndef TORTUOSITYCONTINUUM_H
#define TORTUOSITYCONTINUUM_H

#include <QDialog>
#include <QDebug>
#include <QTextEdit>
#include "../src/Utilities/q_debugstream.h"
#include "puma.h"

namespace Ui {
class tortuosityContinuum;
}

class tortuosityContinuum : public QDialog
{
    Q_OBJECT

public:
    explicit tortuosityContinuum(QWidget *parent = 0);
    void setWorkspace(puma::Workspace *workspace);
    ~tortuosityContinuum();

    // QMessage
    /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    static  void QMessageOutput(QtMsgType , const QMessageLogContext &, const QString &msg);
    /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

private slots:
    void on_comboBox_currentIndexChanged(int index);

    void on_calculateButton_clicked();

    void on_vtkButton_clicked();

    void on_binaryButton_clicked();

private:
    Ui::tortuosityContinuum *ui;
    puma::Workspace *workspace;
    int methodChoice;

    puma::Matrix<double> Cx;
    puma::Matrix<double> Cy;
    puma::Matrix<double> Cz;

    void Calculate();

};

#endif // TORTUOSITYCONTINUUM_H
