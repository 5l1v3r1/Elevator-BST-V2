#include "stcpage.h"
#include "ui_stcpage.h"

stcPage::stcPage(QString fileName, QWidget *parent) :
    TabPage(parent),
    ui(new Ui::stcPage)
{
    ui->setupUi(this);
    m_TreeItem = 0;
    m_StcManager = 0;
}
