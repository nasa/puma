#ifndef OPENLOG_H
#define OPENLOG_H

#include <QDialog>
#include "puma.h"

namespace Ui {
class OpenLog;
}

class OpenLog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenLog(QWidget *parent = nullptr);
    void setWorkspace(puma::Workspace *workspace);
    ~OpenLog();

private slots:
    void on_refreshButton_clicked();


private:
    Ui::OpenLog *ui;
    puma::Workspace *workspace;
};

#endif // OPENLOG_H
