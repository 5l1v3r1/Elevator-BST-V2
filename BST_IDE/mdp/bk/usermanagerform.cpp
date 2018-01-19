#include "usermanagerform.h"
#include "ui_usermanagerform.h"

userManagerForm::userManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::userManagerForm)
{
    ui->setupUi(this);
}

userManagerForm::~userManagerForm()
{
    delete ui;
}
