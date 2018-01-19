#ifndef STREAMMEDIASERVER_H
#define STREAMMEDIASERVER_H

#include "tabpage.h"

namespace Ui {
class StreamMediaServer;
}

typedef struct{
    QString mPath;
    bool    mIsPlayed;
} MEDIAINFO;

class StreamMediaServer : public TabPage
{
    Q_OBJECT
public:
    explicit StreamMediaServer(QWidget *parent = 0);
    ~StreamMediaServer();

    void DelChildTreeItem(QTreeWidgetItem *parentItem);
    QTreeWidgetItem *DelTreeItem(QTreeWidgetItem *pItem);
    QTreeWidgetItem *AddTreeItem(QTreeWidgetItem *parentItem, QString content, QIcon pIcon=QIcon(), bool Editable=false);
    void ScanDirToMediaList(QTreeWidgetItem *parentItem, QString pPath, QStringList pSuffixList);

    void MediaTreeListUpdate(QString pPath, QString pFileType=QString());

    void UpdateMediaList();
    void AddMedia(QString pMediaPath);
    void DelMedia(QString pMediaPath);
    void DelMedia(int pUserID);

    void ServerOpened();
    void ServerClosed();

    void UpdateMediaSource();
    
private slots:
    void on_MediaType_currentIndexChanged(const QString &arg1);
    void on_MediaPath_textEdited(const QString &arg1);
    void on_MediaPathSelect_clicked();

    void slot_AddToMediaList();
    void slot_SetMediaUp();
    void slot_SetMediaDown();
    void slot_DelMedia();
    void slot_PlayMedia();
    void slot_DelUser();

    void on_MediaTreeList_itemPressed(QTreeWidgetItem *item, int column);
    void on_MediaQueneList_itemPressed(QListWidgetItem *item);

    void slot_OutputCommand();

    void on_ServerConfSelect_clicked();
    void on_BtnOpenServer_clicked();

    void on_BtnStart_clicked();
    void on_BtnStop_clicked();

    void on_MediaQueneList_doubleClicked(const QModelIndex &index);

private:
    Ui::StreamMediaServer *ui;

    QList<MEDIAINFO> m_MediaList;

    QProcess *m_Process;

    QAction * m_AddToMediaList;
    QAction * m_SetMediaUp;
    QAction * m_SetMediaDown;
    QAction * m_DelMedia;
    QAction * m_PlayMedia;
    QAction * m_DelUser;
};

#endif // STREAMMEDIASERVER_H
