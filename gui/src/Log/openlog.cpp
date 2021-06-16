#include "openlog.h"
#include "ui_openlog.h"

void OpenLog::setWorkspace(puma::Workspace *workspace) {

    this->workspace = workspace;
    ui->logTextBrowser->setText(QString::fromStdString(workspace->log->log));

}

OpenLog::OpenLog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenLog)
{
    ui->setupUi(this);

}

OpenLog::~OpenLog()
{
    delete ui;
}


void OpenLog::on_refreshButton_clicked()
{
    ui->logTextBrowser->setText(QString::fromStdString(workspace->log->log));
}
