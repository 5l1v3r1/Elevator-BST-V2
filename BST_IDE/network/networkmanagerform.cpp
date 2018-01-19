#include "networkmanagerform.h"
#include "ui_networkmanagerform.h"

NetWorkManagerForm::NetWorkManagerForm(QWidget *parent) :
    TabPage(parent),
    ui(new Ui::NetWorkManagerForm)
{
    ui->setupUi(this);
    m_BearerCloud = new BearerCloud;
    ui->ManagerView->setScene(m_BearerCloud);
}

NetWorkManagerForm::~NetWorkManagerForm()
{
    delete ui;
}
