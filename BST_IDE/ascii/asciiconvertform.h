#ifndef ASCIICONVERTFORM_H
#define ASCIICONVERTFORM_H

#include "tabpage.h"
#include "previewform.h"

namespace Ui {
class AsciiConvertForm;
}

class AsciiConvertForm : public TabPage
{
    Q_OBJECT
public:
    explicit AsciiConvertForm(QWidget *parent = 0);
    ~AsciiConvertForm();

    QMdiSubWindow  *createMdiChild(QString pFilePath);
    bool MdiChildHasCreated(QString pFilePath);

private slots:
    void open();
    void save(QObject *pObject);
    void save(QObject *pObject, QString fileName);
    void saveAll();
    void on_FileCatalog_textEdited(const QString &arg1);
    void on_BtnBrowser_clicked();
    void on_FileTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_BtnSaveAll_clicked();

private:
    void findCodecs();
    void createActions();
    void createMenus();

private:
    Ui::AsciiConvertForm *ui;
    QStringList     m_FileBrowserSuffixList;
    QList<QTextCodec *> codecs;
    QMenu *fileMenu;
    QMenu *saveAsMenu;
    QAction *openAct;
    QAction *saveAllAct;
    QList<QAction *> saveAsActs;
};

#endif // ASCIICONVERTFORM_H
