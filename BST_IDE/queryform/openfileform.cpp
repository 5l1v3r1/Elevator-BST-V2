#include "openfileform.h"

OpenFileForm::OpenFileForm(QWidget *parent) :
    BaseForm(parent)
{
    setWindowTitle(tr("open file"));
    setWindowIcon(QIcon(":/Menu/rc/Menu/open.png"));
    HideLeftFrame(true);
    ShowFiles();
}

OpenFileForm::~OpenFileForm()
{
}

void OpenFileForm::accept()
{
    QDialog::accept();
}

void OpenFileForm::reject()
{
    QDialog::reject();
}
