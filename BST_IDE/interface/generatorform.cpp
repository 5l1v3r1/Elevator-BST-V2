#include "generatorform.h"
#include "ui_generatorform.h"

GeneratorForm::GeneratorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneratorForm)
{
    ui->setupUi(this);
}

GeneratorForm::~GeneratorForm()
{
    delete ui;
}
