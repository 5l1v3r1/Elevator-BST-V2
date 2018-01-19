#include "sqlserverform.h"
#include "ui_sqlserverform.h"

SQLServerForm::SQLServerForm(QWidget *parent) :
    TabPage(parent),
    ui(new Ui::SQLServerForm)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(0);

    ui->Users_Table->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    ui->Users_Table->horizontalHeader()->setStretchLastSection(true);  //列宽度自适应
    ui->Users_Table->verticalHeader()->hide();

    ui->Device_Table->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    ui->Device_Table->horizontalHeader()->setStretchLastSection(true);  //列宽度自适应
    ui->Device_Table->verticalHeader()->hide();

    ui->Rc_Table->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    ui->Rc_Table->horizontalHeader()->setStretchLastSection(true);  //列宽度自适应
    ui->Rc_Table->verticalHeader()->hide();

    ui->TableContent->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    ui->TableContent->horizontalHeader()->setStretchLastSection(true);  //列宽度自适应
    ui->TableContent->verticalHeader()->hide();

    ui->TableContent->setEditTriggers(QTableView::NoEditTriggers); //设为禁止编辑
    ui->Users_Table->setEditTriggers(QTableView::NoEditTriggers);
    ui->Device_Table->setEditTriggers(QTableView::NoEditTriggers);
    ui->Rc_Table->setEditTriggers(QTableView::NoEditTriggers);

    InitSQLForm();

    m_FileProcess = new QProcess;
    connect(m_FileProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(slot_ReadFileOperateOutput()));
}

SQLServerForm::~SQLServerForm()
{
    delete ui;
}

void SQLServerForm::InitSQLForm()
{
    QSqlQuery query2(GlobalPara.m_SqlDatabase);
    //UserManager
    if(GlobalPara.m_SqlDatabase.isOpen())
    {
        QString Column_Cmd = "SELECT SysColumns.name AS Columnsname FROM SysObjects, SysColumns WHERE Sysobjects.Id=Syscolumns.Id AND sysobjects.name = '用户信息管理'";
        if(query2.exec(Column_Cmd))
        {
            ui->Users->addItem(QIcon(""),tr("*"),0);
            while(query2.next())
            {
                ui->Users->addItem(QIcon(""),QString(query2.value(0).toString()),0);
                LineEditNameList<<query2.value(0).toString();
            }
            on_Search_Btn_clicked();
        }
    }
    //UserManager

    //DeviceMamager
    if(GlobalPara.m_SqlDatabase.isOpen())
    {
        QString Column_Cmd = "SELECT SysColumns.name AS Columnsname FROM SysObjects, SysColumns WHERE Sysobjects.Id=Syscolumns.Id AND sysobjects.name = '设备信息'";
        if(query2.exec(Column_Cmd))
        {
            ui->Devices->addItem(QIcon(""),tr("*"),0);
            while(query2.next())
            {
                ui->Devices->addItem(QIcon(""),QString(query2.value(0).toString()),0);
                Device_LineEditNameList<<query2.value(0).toString();
            }
            on_Device_Sreach_Btn_clicked();
        }
    }
    //DeviceMamager

    //RcManager
    if(GlobalPara.m_SqlDatabase.isOpen())
    {
        QString Rc_Column_Cmd = "SELECT SysColumns.name AS Columnsname FROM SysObjects, SysColumns WHERE Sysobjects.Id=Syscolumns.Id AND sysobjects.name = '资源管理'";
        if(query2.exec(Rc_Column_Cmd))
        {
            ui->Rc->addItem(QIcon(""),tr("*"),0);
            while(query2.next())
            {
                ui->Rc->addItem(QIcon(""),QString(query2.value(0).toString()),0);
                Rc_LineEditNameList<<query2.value(0).toString();
            }
            on_Rc_Sreach_Btn_clicked();
        }
    }
    //RcManager

    //TableExplorer
    if(GlobalPara.m_SqlDatabase.isOpen())
    {
        QString Table_Cmd = "SELECT * FROM SYSOBJECTS WHERE XTYPE='U ' and name <> 'dtproperties' and name <> 'sysdiagrams'";
        if(query2.exec(Table_Cmd))
        {
            ui->Sql_Table->clear();
            while(query2.next())
            {
                TableList<<query2.value(0).toString();
                QTreeWidgetItem * tmpitem = new QTreeWidgetItem(0);
                tmpitem->setText(0,query2.value(0).toString());
                ui->Sql_Table->addTopLevelItem(tmpitem);
            }
            connect(ui->Sql_Table,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),this,SLOT(on_TreeWidget_Clicked(QTreeWidgetItem*,QTreeWidgetItem*)));
        }
    }
    //TableExplorer
}

void SQLServerForm::on_Search_Btn_clicked()
{
    QString Cmd;
    QSqlQueryModel *model = new QSqlTableModel;
    QSqlQuery query(GlobalPara.m_SqlDatabase);

    if(ui->Users->currentText().compare("*")==0)
        Cmd = "select * from 用户信息管理";
    else
        Cmd = "select * from 用户信息管理 where " + ui->Users->currentText() + "='" + ui->Users_lineedit->text() + "'";

    query.exec(Cmd);
    ui->RecordCount->display(query.numRowsAffected());
    model->setQuery(query);
    ui->Users_Table->setModel(model);
}

void SQLServerForm::on_Add_Record_clicked()
{
    widget = new QWidget(0);
    widget->setWindowModality(Qt::ApplicationModal);
    widget->setWindowTitle("Insert A Record");

    QString Column_Cmd = "SELECT SysColumns.name AS Columnsname FROM SysObjects, SysColumns WHERE Sysobjects.Id=Syscolumns.Id AND sysobjects.name = '用户信息管理'";
    QSqlQuery query(GlobalPara.m_SqlDatabase);
    query.exec(Column_Cmd);

    int count =0;
    QVBoxLayout *layout = new QVBoxLayout(0);

    while(query.next())
    {
        QHBoxLayout * tmplayout = new QHBoxLayout(0);
        QLabel *tmplabel = new QLabel(query.value(0).toString());
        QLineEdit *tmplineedit = new QLineEdit;
        tmplayout->addWidget(tmplabel);
        tmplayout->addWidget(tmplineedit);
        tmplineedit->setObjectName(query.value(0).toString());
        layout->addLayout(tmplayout);
        tmplayout->setParent(layout);
        count++;
    }
    QHBoxLayout * tmplayout = new QHBoxLayout(0);
    QPushButton *confirm = new QPushButton("确定",0);
    QPushButton *cancel = new QPushButton("取消",0);
    tmplayout->addWidget(confirm);
    tmplayout->addWidget(cancel);
    layout->addLayout(tmplayout);
    widget->setLayout(layout);

    connect(confirm,SIGNAL(clicked()),this,SLOT(on_confirm_clicked()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(on_cancel_clicked()));
    widget->show();
}

void SQLServerForm::on_Table_Confirm_clicked(QModelIndex index)
{
    IDE_TRACE_INT(LineEditNameList.count());
    if(index.row()==1&&index.column()==LineEditNameList.count()-3)
    {
        int count = LineEditNameList.count();
        QString Cmd = QString("insert into %1 values(").arg("用户信息管理");
        for(int i=0;i<count;i++)
        {
            if(i <count-1)
                Cmd = Cmd + "'" + table->item(0,i)->text() + "'" + ",";
            else
                Cmd = Cmd + "'" + table->item(0,i)->text() + "'" + ")";
        }
        QSqlQuery query(GlobalPara.m_SqlDatabase);
        bool flag = query.exec(Cmd);
        if(flag == false)
        {
            QMessageBox::warning(this, tr("Error"),
                                 tr("Cannot Insert Data. \nData Invalid"));
            widget->close();
        }
        else
        {
            on_Search_Btn_clicked();
            widget->close();
        }
    }
    else if(index.row()==1&&index.column()==LineEditNameList.count()-2)
    {
        widget->close();
    }
}

void SQLServerForm::on_confirm_clicked()
{
    int count = LineEditNameList.count();
    QString Cmd = QString("insert into %1 values(").arg("用户信息管理");
    for(int i=0;i<count;i++)
    {
        QLineEdit *lineEdit = widget->findChild<QLineEdit *>(LineEditNameList.at(i));
        if(i <count-1)
            Cmd = Cmd + "'" + lineEdit->text() + "'" + ",";
        else
            Cmd = Cmd + "'" + lineEdit->text() + "'" + ")";
    }
    QSqlQuery query(GlobalPara.m_SqlDatabase);
    bool flag = query.exec(Cmd);
    if(flag == false)
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("Cannot Insert Data. \nIncomplete Data"));
    }
    ui->Users->setCurrentIndex(0);
    on_Search_Btn_clicked();
    widget->close();
}

void SQLServerForm::on_cancel_clicked()
{
    IDE_TRACE();
    on_Search_Btn_clicked();
    on_Device_Sreach_Btn_clicked();
    on_Rc_Sreach_Btn_clicked();
    widget->close();
}

void SQLServerForm::on_Delete_Record_clicked()
{
    QString Cmd = "delete from 用户信息管理 where ";
    int count = LineEditNameList.count();
    int curRow = ui->Users_Table->currentIndex().row();
    QAbstractItemModel *modessl = ui->Users_Table->model();
    QModelIndex indextemp;
    QVariant datatemp;
    QString tmpString;
    for(int i=0;i<count;i++)
    {
        indextemp = modessl->index(curRow,i);
        datatemp = modessl->data(indextemp);
        tmpString = datatemp.toString();
        if(tmpString.isEmpty() || tmpString.isNull())
            continue;
        if(i<count-1)
            Cmd = Cmd + LineEditNameList.at(i) + "= '" + tmpString + "' AND ";
        else
            Cmd = Cmd + LineEditNameList.at(i) + "= '" + tmpString + "'";
    }
    QSqlQuery query(GlobalPara.m_SqlDatabase);
    query.exec(Cmd);
    on_Search_Btn_clicked();
}

void SQLServerForm::User_Modefy()
{
    widget = new QWidget(0);
    widget->setWindowFlags(Qt::WindowCancelButtonHint);
    widget->setWindowModality(Qt::ApplicationModal);
    widget->setWindowTitle("Modefy A Record");

    QString Column_Cmd = "SELECT SysColumns.name AS Columnsname FROM SysObjects, SysColumns WHERE Sysobjects.Id=Syscolumns.Id AND sysobjects.name = '用户信息管理'";
    QSqlQuery query(GlobalPara.m_SqlDatabase);
    query.exec(Column_Cmd);

    int count =0;
    QVBoxLayout *layout = new QVBoxLayout(0);

    int curRow = ui->Users_Table->currentIndex().row();
    QAbstractItemModel *modessl = ui->Users_Table->model();
    QModelIndex indextemp;
    QVariant datatemp;
    QString tmpString;

    while(query.next())
    {
        QHBoxLayout * tmplayout = new QHBoxLayout(0);
        QLabel *tmplabel = new QLabel(query.value(0).toString());
        QLineEdit *tmplineedit = new QLineEdit;
        tmplayout->addWidget(tmplabel);
        tmplayout->addWidget(tmplineedit);
        indextemp = modessl->index(curRow,count);
        datatemp = modessl->data(indextemp);
        tmpString = datatemp.toString();
        tmplineedit->setText(tmpString);
        tmplineedit->setObjectName(query.value(0).toString());
        layout->addLayout(tmplayout);
        tmplayout->setParent(layout);
        count++;
    }
    QHBoxLayout * tmplayout = new QHBoxLayout(0);
    QPushButton *confirm = new QPushButton("确定",0);
    QPushButton *cancel = new QPushButton("取消",0);
    tmplayout->addWidget(confirm);
    tmplayout->addWidget(cancel);
    layout->addLayout(tmplayout);
    QSpacerItem * tmpspacer = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Expanding);
    layout->addSpacerItem(tmpspacer);
    widget->setMinimumWidth(300);
    widget->setLayout(layout);

    connect(confirm,SIGNAL(clicked()),this,SLOT(on_User_Confirm_Clicked()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(on_cancel_clicked()));
    widget->show();
}

void SQLServerForm::on_User_Confirm_Clicked()
{
    int count = LineEditNameList.count();
    int curRow = ui->Users_Table->currentIndex().row();
    QAbstractItemModel *modessl = ui->Users_Table->model();
    QModelIndex indextemp;
    QVariant datatemp;
    QString tmpString;

    QString ColumnName,Context;
    QLineEdit *lineEdit;
    QString Cmd = QString("update 用户信息管理 set ");
    for(int i=0;i<count;i++)
    {
        ColumnName = LineEditNameList.at(i);
        lineEdit = widget->findChild<QLineEdit *>(ColumnName);
        Context = lineEdit->text();
        if(i<count-1)
        {
            Cmd = Cmd + ColumnName + "= '" + Context + "' , ";
        }
        else
        {
            Cmd = Cmd + ColumnName + "= '" + Context + "' where ";
        }
    }

    for(int i=0;i<count;i++)
    {
        indextemp = modessl->index(curRow,i);
        datatemp = modessl->data(indextemp);
        tmpString = datatemp.toString();
        if(tmpString.isEmpty() || tmpString.isNull())
            continue;
        ColumnName = LineEditNameList.at(i);
        if(i<count-1)
            Cmd = Cmd + ColumnName + "= '" + tmpString + "' AND ";
        else
            Cmd = Cmd + ColumnName + "= '" + tmpString + "'";
    }
    IDE_TRACE_STR(Cmd);
    QSqlQuery query(GlobalPara.m_SqlDatabase);
    query.exec(Cmd);
    on_Search_Btn_clicked();
    ui->Users_Table->clearSelection();
    widget->close();
}

void SQLServerForm::on_TreeWidget_Clicked(QTreeWidgetItem* current,QTreeWidgetItem* previous)
{
    QString Cmd = QString("select * from %1").arg(current->text(0));
    QSqlQueryModel *model = new QSqlTableModel;
    QSqlQuery query(GlobalPara.m_SqlDatabase);
    query.exec(Cmd);
    model->setQuery(query);
    ui->TableContent->setModel(model);
}

void SQLServerForm::on_Device_Sreach_Btn_clicked()
{
    QString Cmd;
    QSqlQueryModel *model = new QSqlTableModel;
    QSqlQuery query(GlobalPara.m_SqlDatabase);

    if(ui->Devices->currentText().compare("*") == 0)
        Cmd = "select * from 设备信息";
    else
        Cmd = "select * from 设备信息 where " + ui->Devices->currentText() + "='" + ui->Devices_lineedit->text() + "'";

    query.exec(Cmd);
    ui->DeviceRecordCount->display(query.numRowsAffected());
    model->setQuery(query);
    ui->Device_Table->setModel(model);
}

void SQLServerForm::on_Device_Add_Record_clicked()
{
    widget = new QWidget(0);
    widget->setWindowModality(Qt::ApplicationModal);
    widget->setWindowTitle("Insert A Record");

    QString Column_Cmd = "SELECT SysColumns.name AS Columnsname FROM SysObjects, SysColumns WHERE Sysobjects.Id=Syscolumns.Id AND sysobjects.name = '设备信息'";
    QSqlQuery query(GlobalPara.m_SqlDatabase);
    query.exec(Column_Cmd);

    int count =0;
    QVBoxLayout *layout = new QVBoxLayout(0);

    while(query.next())
    {
        QHBoxLayout * tmplayout = new QHBoxLayout(0);
        QLabel *tmplabel = new QLabel(query.value(0).toString());
        QLineEdit *tmplineedit = new QLineEdit;
        tmplayout->addWidget(tmplabel);
        tmplayout->addWidget(tmplineedit);
        tmplineedit->setObjectName(query.value(0).toString());
        layout->addLayout(tmplayout);
        tmplayout->setParent(layout);
        count++;
    }
    QHBoxLayout * tmplayout = new QHBoxLayout(0);
    QPushButton *confirm = new QPushButton("确定",0);
    QPushButton *cancel = new QPushButton("取消",0);
    tmplayout->addWidget(confirm);
    tmplayout->addWidget(cancel);
    layout->addLayout(tmplayout);
    widget->setLayout(layout);

    connect(confirm,SIGNAL(clicked()),this,SLOT(on_Devide_confirm_clicked()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(on_cancel_clicked()));

    widget->show();
}

void SQLServerForm::on_Devide_confirm_clicked()
{
    IDE_TRACE();
    int count = Device_LineEditNameList.count();
    QString Cmd = QString("insert into %1 values(").arg("设备信息");
    IDE_TRACE();
    for(int i=0;i<count;i++)
    {
        QLineEdit *lineEdit = widget->findChild<QLineEdit *>(Device_LineEditNameList.at(i));
        if(i <count-1)
            Cmd = Cmd + "'" + lineEdit->text() + "'" + ",";
        else
            Cmd = Cmd + "'" + lineEdit->text() + "'" + ")";
    }
    QSqlQuery query(GlobalPara.m_SqlDatabase);
    bool flag = query.exec(Cmd);
    if(flag == false)
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("Cannot Insert Data. \nIncomplete Data"));
    }
    ui->Devices->setCurrentIndex(0);
    on_Device_Sreach_Btn_clicked();
    widget->close();
}

void SQLServerForm::on_Device_Del_Record_clicked()
{
    QString Cmd = "delete from 设备信息 where ";
    int count = Device_LineEditNameList.count();
    int curRow = ui->Device_Table->currentIndex().row();
    QAbstractItemModel *modessl = ui->Device_Table->model();
    QModelIndex indextemp;
    QVariant datatemp;
    QString tmpString;
    for(int i=0;i<count;i++)
    {
        indextemp = modessl->index(curRow,i);
        datatemp = modessl->data(indextemp);
        tmpString = datatemp.toString();
        if(tmpString.isEmpty() || tmpString.isNull())
            continue;
        if(i<count-1)
            Cmd = Cmd + Device_LineEditNameList.at(i) + "= '" + tmpString + "' AND ";
        else
            Cmd = Cmd + Device_LineEditNameList.at(i) + "= '" + tmpString + "'";
    }
    IDE_TRACE_STR(Cmd);
    QSqlQuery query(GlobalPara.m_SqlDatabase);
    query.exec(Cmd);
    on_Device_Sreach_Btn_clicked();
}

void SQLServerForm::Device_Modefy()
{
    widget = new QWidget(0);
    widget->setWindowFlags(Qt::WindowCancelButtonHint);
    widget->setWindowModality(Qt::ApplicationModal);
    widget->setWindowTitle("Modefy A Record");

    QString Column_Cmd = "SELECT SysColumns.name AS Columnsname FROM SysObjects, SysColumns WHERE Sysobjects.Id=Syscolumns.Id AND sysobjects.name = '设备信息'";
    QSqlQuery query(GlobalPara.m_SqlDatabase);
    query.exec(Column_Cmd);

    int count =0;
    QVBoxLayout *layout = new QVBoxLayout(0);

    int curRow = ui->Device_Table->currentIndex().row();
    QAbstractItemModel *modessl = ui->Device_Table->model();
    QModelIndex indextemp;
    QVariant datatemp;
    QString tmpString;

    while(query.next())
    {
        QHBoxLayout * tmplayout = new QHBoxLayout(0);
        QLabel *tmplabel = new QLabel(query.value(0).toString());
        QLineEdit *tmplineedit = new QLineEdit;
        tmplayout->addWidget(tmplabel);
        tmplayout->addWidget(tmplineedit);
        indextemp = modessl->index(curRow,count);
        datatemp = modessl->data(indextemp);
        tmpString = datatemp.toString();
        tmplineedit->setText(tmpString);
        tmplineedit->setObjectName(query.value(0).toString());
        layout->addLayout(tmplayout);
        tmplayout->setParent(layout);
        count++;
    }
    QHBoxLayout * tmplayout = new QHBoxLayout(0);
    QPushButton *confirm = new QPushButton("确定",0);
    QPushButton *cancel = new QPushButton("取消",0);
    tmplayout->addWidget(confirm);
    tmplayout->addWidget(cancel);
    layout->addLayout(tmplayout);
    QSpacerItem * tmpspacer = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Expanding);
    layout->addSpacerItem(tmpspacer);
    widget->setMinimumWidth(300);
    widget->setLayout(layout);

    connect(confirm,SIGNAL(clicked()),this,SLOT(on_Device_Confime_Btn_Clicked()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(on_cancel_clicked()));
    widget->show();
}

void SQLServerForm::on_Device_Confime_Btn_Clicked()
{
    int count = Device_LineEditNameList.count();
    int curRow = ui->Device_Table->currentIndex().row();
    QAbstractItemModel *modessl = ui->Device_Table->model();
    QModelIndex indextemp;
    QVariant datatemp;
    QString tmpString;

    QString ColumnName,Context;
    QLineEdit *lineEdit;
    QString Cmd = QString("update 设备信息 set ");
    for(int i=0;i<count;i++)
    {
        ColumnName = Device_LineEditNameList.at(i);
        lineEdit = widget->findChild<QLineEdit *>(ColumnName);
        Context = lineEdit->text();
        if(i<count-1)
        {
            Cmd = Cmd + ColumnName + "= '" + Context + "' , ";
        }
        else
        {
            Cmd = Cmd + ColumnName + "= '" + Context + "' where ";
        }
    }

    for(int i=0;i<count;i++)
    {
        indextemp = modessl->index(curRow,i);
        datatemp = modessl->data(indextemp);
        tmpString = datatemp.toString();
        if(tmpString.isEmpty() || tmpString.isNull())
            continue;
        ColumnName = Device_LineEditNameList.at(i);
        if(i<count-1)
            Cmd = Cmd + ColumnName + "= '" + tmpString + "' AND ";
        else
            Cmd = Cmd + ColumnName + "= '" + tmpString + "'";
    }
    IDE_TRACE_STR(Cmd);
    QSqlQuery query(GlobalPara.m_SqlDatabase);
    query.exec(Cmd);
    on_Device_Sreach_Btn_clicked();
    ui->Device_Table->clearSelection();
    widget->close();
}

void SQLServerForm::on_Rc_Sreach_Btn_clicked()
{
    QString Cmd;
    QSqlQueryModel *model = new QSqlTableModel;
    QSqlQuery query(GlobalPara.m_SqlDatabase);

    if(ui->Rc->currentText().compare("*") == 0)
    {
        Cmd = "select * from 资源管理";
    }
    else
    {
        Cmd = "select * from 资源管理 where " + ui->Rc->currentText() + "='" + ui->Rc_lineedit->text() + "'";
    }

    query.exec(Cmd);
    ui->RcRecordCount->display(query.numRowsAffected());
    model->setQuery(query);
    ui->Rc_Table->setModel(model);
}

void SQLServerForm::on_Rc_Add_Record_clicked()
{
    widget = new QWidget(0);
    widget->setWindowModality(Qt::ApplicationModal);
    widget->setWindowTitle("Insert A Record");

    QString Column_Cmd = "SELECT SysColumns.name AS Columnsname FROM SysObjects, SysColumns WHERE Sysobjects.Id=Syscolumns.Id AND sysobjects.name = '资源管理'";
    QSqlQuery query(GlobalPara.m_SqlDatabase);
    query.exec(Column_Cmd);

    int count =0;
    QVBoxLayout *layout = new QVBoxLayout(0);

    while(query.next())
    {
        QHBoxLayout * tmplayout = new QHBoxLayout(0);
        QLabel *tmplabel = new QLabel(query.value(0).toString());
        QLineEdit *tmplineedit = new QLineEdit;
        tmplayout->addWidget(tmplabel);
        tmplayout->addWidget(tmplineedit);
        tmplineedit->setObjectName(query.value(0).toString());
        layout->addLayout(tmplayout);
        tmplayout->setParent(layout);
        count++;
    }
    QHBoxLayout * tmplayout = new QHBoxLayout(0);
    QPushButton *confirm = new QPushButton("确定",0);
    QPushButton *cancel = new QPushButton("取消",0);
    tmplayout->addWidget(confirm);
    tmplayout->addWidget(cancel);
    layout->addLayout(tmplayout);
    widget->setLayout(layout);

    connect(confirm,SIGNAL(clicked()),this,SLOT(on_Rc_confirm_clicked()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(on_cancel_clicked()));

    widget->show();
}

void SQLServerForm::on_Rc_confirm_clicked()
{
    QString RcFilename;
    RcFilename = Rc_LineEditNameList.at(Rc_LineEditNameList.count()-1);
    QLineEdit *lineEdit = widget->findChild<QLineEdit *>(RcFilename);
    RcFilename = lineEdit->text();
    IDE_TRACE_STR(RcFilename);
    QFile file(RcFilename);
    if(file.exists())
    {
        int count = Rc_LineEditNameList.count();
        QString Cmd = QString("insert into %1 values(").arg("资源管理");
        for(int i=0;i<count;i++)
        {
            QLineEdit *lineEdit = widget->findChild<QLineEdit *>(Rc_LineEditNameList.at(i));
            if(i <count-1)
                Cmd = Cmd + "'" + lineEdit->text() + "'" + ",";
            else
                Cmd = Cmd + "'" + lineEdit->text() + "'" + ")";
        }
        QSqlQuery query(GlobalPara.m_SqlDatabase);
        bool flag = query.exec(Cmd);
        if(flag == false)
        {
            QMessageBox::warning(this, tr("Error"),
                                 tr("Cannot Insert Data. \nIncomplete Data"));
        }
        ui->Users->setCurrentIndex(0);
        on_Rc_Sreach_Btn_clicked();
        widget->close();
    }
    else
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("Can't find file:%1").arg(RcFilename));
    }
}

void SQLServerForm::on_Rc_Del_Record_clicked()
{
    QString Cmd = "delete from 资源管理 where ";
    int count = Rc_LineEditNameList.count();
    int curRow = ui->Rc_Table->currentIndex().row();
    QAbstractItemModel *modessl = ui->Rc_Table->model();
    QModelIndex indextemp;
    QVariant datatemp;
    QString tmpString;
    for(int i=0;i<count;i++)
    {
        indextemp = modessl->index(curRow,i);
        datatemp = modessl->data(indextemp);
        tmpString = datatemp.toString();
        if(tmpString.isEmpty() || tmpString.isNull())
            continue;
        if(i<count-1)
            Cmd = Cmd + Rc_LineEditNameList.at(i) + "= '" + tmpString + "' AND ";
        else
            Cmd = Cmd + Rc_LineEditNameList.at(i) + "= '" + tmpString + "'";
    }
    QSqlQuery query(GlobalPara.m_SqlDatabase);
    query.exec(Cmd);
    on_Rc_Sreach_Btn_clicked();
}

void SQLServerForm::Rc_Modefy()
{
    widget = new QWidget(0);
    widget->setWindowFlags(Qt::WindowCancelButtonHint);
    widget->setWindowModality(Qt::ApplicationModal);
    widget->setWindowTitle("Modefy A Record");

    QString Column_Cmd = "SELECT SysColumns.name AS Columnsname FROM SysObjects, SysColumns WHERE Sysobjects.Id=Syscolumns.Id AND sysobjects.name = '资源管理'";
    QSqlQuery query(GlobalPara.m_SqlDatabase);
    query.exec(Column_Cmd);

    int count =0;
    QVBoxLayout *layout = new QVBoxLayout(0);

    int curRow = ui->Rc_Table->currentIndex().row();
    QAbstractItemModel *modessl = ui->Rc_Table->model();
    QModelIndex indextemp;
    QVariant datatemp;
    QString tmpString;

    while(query.next())
    {
        QHBoxLayout * tmplayout = new QHBoxLayout(0);
        QLabel *tmplabel = new QLabel(query.value(0).toString());
        QLineEdit *tmplineedit = new QLineEdit;
        tmplayout->addWidget(tmplabel);
        tmplayout->addWidget(tmplineedit);
        indextemp = modessl->index(curRow,count);
        datatemp = modessl->data(indextemp);
        tmpString = datatemp.toString();
        tmplineedit->setText(tmpString);
        tmplineedit->setObjectName(query.value(0).toString());
        layout->addLayout(tmplayout);
        tmplayout->setParent(layout);
        count++;
    }
    QHBoxLayout * tmplayout = new QHBoxLayout(0);
    QPushButton *confirm = new QPushButton("确定",0);
    QPushButton *cancel = new QPushButton("取消",0);
    tmplayout->addWidget(confirm);
    tmplayout->addWidget(cancel);
    layout->addLayout(tmplayout);
    QSpacerItem * tmpspacer = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Expanding);
    layout->addSpacerItem(tmpspacer);
    widget->setMinimumWidth(300);
    widget->setLayout(layout);

    connect(confirm,SIGNAL(clicked()),this,SLOT(on_Rc_Comfirm_Btn_clicked()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(on_cancel_clicked()));
    widget->show();
}

void SQLServerForm::on_Rc_Comfirm_Btn_clicked()
{
    QString RcFilename;
    RcFilename = Rc_LineEditNameList.at(Rc_LineEditNameList.count()-1);
    QLineEdit *lineEdit = widget->findChild<QLineEdit *>(RcFilename);
    RcFilename = lineEdit->text();

    QFile file(RcFilename);
    if(file.exists())
    {
        QString Cmd = QString("update 资源管理 set 存储位置 = '%1'  where ").arg(RcFilename);
        int count = Rc_LineEditNameList.count();
        int curRow = ui->Rc_Table->currentIndex().row();
        QAbstractItemModel *modessl = ui->Rc_Table->model();
        QModelIndex indextemp;
        QVariant datatemp;
        QString tmpString;
        for(int i=0;i<count;i++)
        {
            indextemp = modessl->index(curRow,i);
            datatemp = modessl->data(indextemp);
            tmpString = datatemp.toString();
            if(tmpString.isEmpty() || tmpString.isNull())
                continue;
            if(i<count-1)
                Cmd = Cmd + Rc_LineEditNameList.at(i) + "= '" + tmpString + "' AND ";
            else
                Cmd = Cmd + Rc_LineEditNameList.at(i) + "= '" + tmpString + "'";
        }
        IDE_TRACE_STR(Cmd);
        QSqlQuery query(GlobalPara.m_SqlDatabase);
        query.exec(Cmd);
        on_Rc_Sreach_Btn_clicked();
        ui->Rc_Table->clearSelection();
        widget->close();
    }
    else
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("Can't find file:%1").arg(RcFilename));
    }
}

void SQLServerForm::on_Rc_Table_doubleClicked(const QModelIndex &index)
{
    Rc_Modefy();
}

void SQLServerForm::on_Device_Table_doubleClicked(const QModelIndex &index)
{
    Device_Modefy();
}

void SQLServerForm::on_Users_Table_doubleClicked(const QModelIndex &index)
{
    User_Modefy();
}

void SQLServerForm::slot_ReadFileOperateOutput()
{

}
