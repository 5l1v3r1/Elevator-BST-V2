#ifndef LOGQUERYPAGE_H
#define LOGQUERYPAGE_H

#include "global.h"
#include <QWidget>

namespace Ui {
class logQueryPage;
}

class logQueryPage : public QWidget
{
    Q_OBJECT
    
public:
    explicit logQueryPage(QWidget *parent = 0);
    explicit logQueryPage(QString pLogDirPath, QWidget *parent = 0);
    ~logQueryPage();
    bool InitEnvir(QString pLogDirPath);

public slots:
    void on_LogFilter_currentIndexChanged(const QString &arg1);
    void InitLogPage(QString pLogDirPath);
    void LogFileParse(QString filename);
    void on_Log_Serach_clicked();
    void on_LogDirPath_clicked();

private:
    Ui::logQueryPage *ui;
    QString     m_LogDirPath;
};

#endif // LOGQUERYPAGE_H
