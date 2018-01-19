#ifndef RCPAGE_H
#define RCPAGE_H

#include <QWidget>
#include "global.h"

namespace Ui {
class rcPage;
}

class rcPage : public QWidget
{
    Q_OBJECT
    
public:
    explicit rcPage(QWidget *parent = 0);
    rcPage(QWidget *parent, QString fileName);
    ~rcPage();

    void InitRCUi();
    void OpenFlorder(QString selectedPath);
    void removedirectory(QString tmpdir);

    void serach(QStringList SearchFileList);

public slots:
    void changeIndex(QModelIndex index);
    void isSelected(const QModelIndex& current,const QModelIndex& previous);
    void on_BtnSearch_clicked();
    void on_Goto_clicked();
    void on_Back_clicked();
    void on_Forward_clicked();
    void on_UpFlorder_clicked();
    void on_refresh_clicked();
    void on_ExplorerList_clicked(const QModelIndex &index);
    void on_Source_Add_clicked();
    void on_Source_Del_clicked();
    
private:
    Ui::rcPage *ui;
    int RuntimeStatus;
    QComboBox *comboBox;
    QDirModel *dirModel,*dirModel1;
    QItemSelectionModel* selectionModel;
    QModelIndex current;
    QModelIndex previous;
    QLineEdit *lineEdit;
    QLabel *label,*label1;
    QList<QString> pathList;
    int pos,flag;
    QString sourcePath,desPath;
    QToolBar *toolbar;
    QVBoxLayout *layout;
    QMenu *menu;
    QAction *IconAction;
    QAction *DetailAction;
    QAction *LineAction;
    QString CurrentPath;
    QAction *action;
    int SerachNum;
    QString Current_File_Path;
    QModelIndex CurrentIndex;

};

#endif // RCPAGE_H
