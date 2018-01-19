#ifndef SQLSERVERFORM_H
#define SQLSERVERFORM_H

#include "tabpage.h"
#include "qsqldriver.h"
#include "qsqlquery.h"
#include "qsqltablemodel.h"
#include "qsqlrecord.h"
#include "qsqlresult.h"

#include "sqlquery.h"

namespace Ui {
class SQLServerForm;
}

class SQLServerForm : public TabPage
{
    Q_OBJECT
public:
    explicit SQLServerForm(QWidget *parent = 0);
    ~SQLServerForm();
    
    void InitSQLForm();

public slots:
    void on_Search_Btn_clicked();
    void on_Add_Record_clicked();
    void on_Delete_Record_clicked();
    void on_confirm_clicked();
    void on_cancel_clicked();
    void on_Table_Confirm_clicked(QModelIndex);
    void User_Modefy();
    void on_User_Confirm_Clicked();
    void on_TreeWidget_Clicked(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_Device_Sreach_Btn_clicked();

    void on_Device_Add_Record_clicked();
    void on_Devide_confirm_clicked();
    void on_Device_Del_Record_clicked();
    void Device_Modefy();
    void on_Device_Confime_Btn_Clicked();

    void on_Rc_Sreach_Btn_clicked();
    void on_Rc_Add_Record_clicked();
    void on_Rc_Del_Record_clicked();
    void Rc_Modefy();
    void on_Rc_confirm_clicked();
    void on_Rc_Comfirm_Btn_clicked();

    void on_Rc_Table_doubleClicked(const QModelIndex &index);
    void on_Device_Table_doubleClicked(const QModelIndex &index);
    void on_Users_Table_doubleClicked(const QModelIndex &index);

    void slot_ReadFileOperateOutput();

private:
    Ui::SQLServerForm *ui;

    QWidget         *widget;
    QStringList      LineEditNameList;
    QStringList      Device_LineEditNameList;
    QStringList      Rc_LineEditNameList;
    QStringList      TableList;
    QTableWidget    *table;

    QProcess           *m_FileProcess;
};

#endif // SQLSERVERFORM_H
