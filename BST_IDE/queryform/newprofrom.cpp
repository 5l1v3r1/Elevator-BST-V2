#include "newprofrom.h"

NewProFrom::NewProFrom(QWidget *parent) :
    BaseForm(parent)
{
    setWindowTitle(tr("new project"));
    setWindowIcon(QIcon(":/Menu/rc/Menu/solution.png"));
    HideLeftFrame(true);
    ShowProjects();
}

NewProFrom::~NewProFrom()
{
}

void NewProFrom::accept()
{
    QDialog::accept();
}

void NewProFrom::reject()
{
    QDialog::reject();
}
