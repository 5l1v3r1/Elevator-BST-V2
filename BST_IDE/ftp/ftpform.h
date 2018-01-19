#ifndef FTP_H
#define FTP_H

#include "tabpage.h"

#include <QtCore/QHash>
#include <QtNetwork/QFtp>

#include <QtGui/QDialogButtonBox>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QProgressDialog>
#include <QtNetwork/QUrlInfo>
#include <QtGui/QMessageBox>
#include <QtCore/QUrl>
#include <QtGui/QFileDialog>
#include <QtCore/QTextCodec>
#include <QtGui/QDropEvent>
#include <QtGui/QDirModel>

namespace Ui {
    class Ftp;
}

#define TREEITEMTYPE 0
#define TREEITEMDIR  1
#define TREEITEMFILE 2

class FtpForm : public TabPage
{
    Q_OBJECT
public:
    FtpForm(QString pFtpAddr, QWidget *parent = 0);
    ~FtpForm();

    void Connected();
    void Disconnected();

    void InitFTP(QString pFtpAddr, quint16 pFtpPort, QString pUsrname=QString(), QString pPasswd=QString(), QString pDefaultPath=QString());
    void ReleaseFTP();

    QTreeWidgetItem *CreateTreeItem(QUrlInfo urlInfo);
    void UpdateFtpTree();

    bool ConnectToHost(QString pFtpAddr, quint16 pFtpPort, qint32 pDieTime);
    bool LoginFtp(QString pUsrname, QString pPasswd, qint32 pDieTime);
    bool GotoFtpPath(QString pPath, qint32 pDieTime);
    bool ListFtpPath(QString pPath, qint32 pDieTime);
    bool MakeFtpPath(QString pPath, qint32 pDieTime);
    bool GetFtpFile(QString pSrcPath, QFile &pDstFile, qint32 pDieTime);
    bool PutFtpFile(QFile &pSrcFile, QString pDstPath, qint32 pDieTime);

    bool DownloadFile(QString pSrcPath, QString pDstPath, qint32 pDieTime);
    bool DownloadDirectory(QString pSrcPath, QString pDstPath, qint32 pDieTime);
    bool UploadDirectory(QString pSrcPath, QString pDstPath, qint32 pDieTime);
    bool UploadFile(QString pSrcPath, QString pDstPath, qint32 pDieTime);

    void RefreshList();
    void RefreshTree();

public slots:
    void FtpStateChanged(int pState);

    void doubleClickListItem(QModelIndex index);
    void selectListItem(QModelIndex index);
    void doubleClickTreeItem(QTreeWidgetItem *item, int column);
    void addToFileList(const QUrlInfo &urlInfo);
    void FtpCommandFinished(int commandId, bool error);
    void connectOrDisconnect();
    void DownLoad();
    void Upload();
    void CancelFTP();
    void updateDataTransferProgress(qint64 readBytes, qint64 totalBytes);

    void on_Local_Back_clicked();
    void on_Local_Next_clicked();
    void on_Local_Upwards_clicked();
    void on_Upwards_clicked();

    void on_BtnRefresh_clicked();
    void on_BtnAbort_clicked();

    void on_LocalFileAdd_clicked();
    void on_LocalFileDel_clicked();
    void on_RemoteFileAdd_clicked();
    void on_RemoteFileDel_clicked();

private:
    Ui::Ftp *ui;
    QDirModel  *LocalDirModel;
    int         pos;
    QString     Local_CurrentPath;
    QStringList nameFilter;
    QList<QString> pathList;

    QFileInfo       m_CurListItemFile;
    QList<QUrlInfo> m_CurDirFileList;     //>@用于list后存储当前目录下的所有文件
    QFtp            *ftpobj;

    bool            m_ListCommandFinised;
    bool            m_GetCommandFinised;
    bool            m_PutCommandFinised;
    bool            m_MkdirCommandFinised;
    bool            m_CdCommandFinised;
    bool            m_CancelFTP;    //>@终止FTP的一切操作
};

#endif // FTP_H
