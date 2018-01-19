#ifndef UPGRADEFORM_H
#define UPGRADEFORM_H

#include "tabpage.h"

namespace Ui {
class UpgradeForm;
}

class UpgradeForm : public TabPage
{
    Q_OBJECT
    
public:
    explicit UpgradeForm(QWidget *parent = 0);
    ~UpgradeForm();
    
private:
    Ui::UpgradeForm *ui;
};

#endif // UPGRADEFORM_H
