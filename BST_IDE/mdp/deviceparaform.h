#ifndef DEVICEPARAFORM_H
#define DEVICEPARAFORM_H

#include "deviceitem.h"
#include <QStackedWidget>

namespace Ui {
class DeviceParaForm;
}

class DeviceParaForm : public QDialog
{
    Q_OBJECT
public:
    DeviceParaForm(DeviceItem *pItem, QWidget *parent = 0);
    ~DeviceParaForm();

    void InitAdvancedTabWidget();
    void ReleaseAdvancedTabWidget();

private slots:
    void on_BtnAdvanced_clicked(bool checked);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void UpdateAdvancedTabWidget();

public:
    Ui::DeviceParaForm         *ui;
    DeviceItem                 *m_Item;
};

#endif // DEVICEPARAFORM_H
