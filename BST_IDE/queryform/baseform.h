#ifndef BASEFORM_H
#define BASEFORM_H

#include <global.h>
#include <QDialog>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QModelIndex>

namespace Ui {
class BaseForm;
}

class BaseForm : public QDialog
{
    Q_OBJECT
public:
    explicit BaseForm(QWidget *parent = 0);
    ~BaseForm();

    void HideLeftFrame(bool flag);
    void ShowProjects();
    void ShowFiles();

    void addTableWidgetItem(QString ItemName, QString IconName);
    void updateTemplateTableWidget();

    QStandardItemModel newListViewModel;
    void updateNewItemView(QString SolutionName);

public slots:
    void newItemViewClicked(QModelIndex);
    void on_templateTableWidget_clicked(const QModelIndex &index);
    virtual void accept();
    virtual void reject();

public:
    Ui::BaseForm *ui;
    FILE_INFO     m_FileInfo;
};

#endif // BASEFORM_H
