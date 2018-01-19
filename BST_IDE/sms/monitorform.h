#ifndef MONITORFORM_H
#define MONITORFORM_H

#include "tabpage.h"
#include "streammedialabel.h"

namespace Ui {
class MonitorForm;
}

class MonitorForm : public TabPage
{
    Q_OBJECT
public:
    explicit MonitorForm(QWidget *parent = 0);
    ~MonitorForm();

    void AddNewMonitor(QString pUrl);
    bool MdiChildHasCreated(QString pFilePath);
    QMdiSubWindow *createMdiChild(QString pFilePath);

private slots:

    void on_MonitorTree_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void destroyed(QObject *pObject);

    void on_BtnGrid_clicked();

private:
    Ui::MonitorForm *ui;
};

#endif // MONITORFORM_H
