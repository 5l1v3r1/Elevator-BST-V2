#include "upgradeform.h"
#include "ui_upgradeform.h"

UpgradeForm::UpgradeForm(QWidget *parent) :
    TabPage(parent),
    ui(new Ui::UpgradeForm)
{
    ui->setupUi(this);
}

UpgradeForm::~UpgradeForm()
{
    delete ui;
}
