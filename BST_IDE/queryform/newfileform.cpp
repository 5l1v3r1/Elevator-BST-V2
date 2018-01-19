#include "newfileform.h"

newFileForm::newFileForm(QWidget *parent) :
    BaseForm(parent)
{
    setWindowTitle(tr("new file"));
    setWindowIcon(QIcon(":/Menu/rc/Menu/new.png"));
    HideLeftFrame(true);
    ShowFiles();
}

newFileForm::~newFileForm()
{
}

void newFileForm::accept()
{
    QDialog::accept();
}

void newFileForm::reject()
{
    QDialog::reject();
}
