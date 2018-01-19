#ifndef STOFORM_H
#define STOFORM_H

#include "global.h"
#include "pieview.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QAbstractItemView;
class QItemSelectionModel;
QT_END_NAMESPACE

namespace Ui {
class StoForm;
}

class StoForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit StoForm(QString pFilePath, QWidget *parent = 0);
    ~StoForm();
private slots:
    void openFile(const QString &path = QString());
    void saveFile();

private:
    void setupModel();
    void setupViews();

    QAbstractItemModel *model;
    QAbstractItemView *pieChart;
    QItemSelectionModel *selectionModel;
private:
    Ui::StoForm *ui;
};

#endif // STOFORM_H
