#ifndef NETWORKMANAGERFORM_H
#define NETWORKMANAGERFORM_H

#include "tabpage.h"
#include "bearercloud.h"

namespace Ui {
class NetWorkManagerForm;
}

class NetWorkManagerForm : public TabPage
{
    Q_OBJECT
public:
    explicit NetWorkManagerForm(QWidget *parent = 0);
    ~NetWorkManagerForm();
    
private:
    Ui::NetWorkManagerForm *ui;
    BearerCloud            *m_BearerCloud;
};

#endif // NETWORKMANAGERFORM_H
