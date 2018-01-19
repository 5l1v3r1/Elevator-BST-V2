#include "asciiconvertform.h"
#include "ui_asciiconvertform.h"

AsciiConvertForm::AsciiConvertForm(QWidget *parent) :
    TabPage(parent),
    ui(new Ui::AsciiConvertForm)
{
    ui->setupUi(this);
    createActions();
    createMenus();
    findCodecs();
    m_FileBrowserSuffixList << "cpp" << "c" << "h";
}

AsciiConvertForm::~AsciiConvertForm()
{
    ui->TextMdiArea->closeAllSubWindows();
    delete ui;
}

bool AsciiConvertForm::MdiChildHasCreated(QString pFilePath)
{
    QList<QMdiSubWindow *>  tmpList = ui->TextMdiArea->subWindowList();
    for(int i=0;i<tmpList.count();i++)
    {
        PreviewForm *child = VAR_CAST<PreviewForm *>(tmpList.at(i)->widget());
        if(child)
        {
            QString tmpString = child->windowTitle();
            if(tmpString.compare(pFilePath, Qt::CaseInsensitive) == 0)
                return true;
        }
    }
    return false;
}

QMdiSubWindow *AsciiConvertForm::createMdiChild(QString pFilePath)
{
    if (pFilePath.isEmpty())
    {
        QMessageBox::warning(this, tr("File"),
                             tr("File path is empty!"));
        return (QMdiSubWindow*)0;
    }
    //>@查看是否已经存在此子窗口
    if(MdiChildHasCreated(pFilePath))
    {
        QMessageBox::warning(this, tr("Warn"),
                             tr("%1 has opened!").arg(pFilePath));
        return (QMdiSubWindow*)0;
    }
    IDE_TRACE_STR(pFilePath);
    QMdiSubWindow* tmpSubWin = ui->TextMdiArea->addSubWindow(new PreviewForm(pFilePath, codecs));
    if(tmpSubWin == (QMdiSubWindow*)0)
    {
        QMessageBox::warning(this, tr("Eroor"),
                             tr("%1 open failed!").arg(pFilePath));
        return tmpSubWin;
    }
    connect(tmpSubWin, SIGNAL(destroyed(QObject*)), this, SLOT(save(QObject *)));
    return tmpSubWin;
}

void AsciiConvertForm::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    QMdiSubWindow * tmpSubWin = createMdiChild(fileName);
    if(tmpSubWin)
        tmpSubWin->show();
}

void AsciiConvertForm::save(QObject *pObject)
{
    PreviewForm *child = VAR_CAST<PreviewForm *>(pObject);
    if(child)
    {
        QString fileName = QFileDialog::getSaveFileName(this);
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (!file.open(QFile::WriteOnly | QFile::Text)) {
                QMessageBox::warning(this, tr("Codecs"),
                                     tr("Cannot write file %1:\n%2")
                                     .arg(fileName)
                                     .arg(file.errorString()));
                return;
            }

            QByteArray codecName = child->encodingComboBox->currentText().toLatin1();

            QTextStream out(&file);
            out.setCodec(codecName);
            out << child->encodedData;
        }
    }
}

void AsciiConvertForm::save(QObject *pObject, QString fileName)
{
    PreviewForm *child = VAR_CAST<PreviewForm *>(pObject);
    if(child)
    {
        if (!fileName.isEmpty())
        {
            QFile file(fileName);
            if (!file.open(QFile::WriteOnly | QFile::Text)) {
                QMessageBox::warning(this, tr("Codecs"),
                                     tr("Cannot write file %1:\n%2")
                                     .arg(fileName)
                                     .arg(file.errorString()));
                return;
            }

            QByteArray codecName = child->encodingComboBox->currentText().toLatin1();

            //>@用于判断是否可以进行转换
            //QTextCodec *codec = QTextCodec::codecForName(codecName);
            //codec && codec->canEncode(currentText);

            QTextStream out(&file);
            out.setCodec(codecName);
            out << child->encodedData;
        }
    }
}

void AsciiConvertForm::saveAll()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (!fileName.isEmpty())
    {
        QList<QMdiSubWindow *>  tmpList = ui->TextMdiArea->subWindowList();
        for(int i=0;i<tmpList.count();i++)
        {
            save(tmpList.at(i), fileName);
        }
    }
}

void AsciiConvertForm::findCodecs()
{
    QMap<QString, QTextCodec *> codecMap;
    QRegExp iso8859RegExp("ISO[- ]8859-([0-9]+).*");

    foreach (int mib, QTextCodec::availableMibs()) {
        QTextCodec *codec = QTextCodec::codecForMib(mib);

        QString sortKey = codec->name().toUpper();
        int rank;

        if (sortKey.startsWith("UTF-8")) {
            rank = 1;
        } else if (sortKey.startsWith("UTF-16")) {
            rank = 2;
        } else if (iso8859RegExp.exactMatch(sortKey)) {
            if (iso8859RegExp.cap(1).size() == 1)
                rank = 3;
            else
                rank = 4;
        } else {
            rank = 5;
        }
        sortKey.prepend(QChar('0' + rank));

        codecMap.insert(sortKey, codec);
    }
    codecs = codecMap.values();
}


void AsciiConvertForm::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAllAct = new QAction(tr("Save &All..."), this);
    connect(saveAllAct, SIGNAL(triggered()), this, SLOT(saveAll()));

    foreach (QTextCodec *codec, codecs)
    {
        QString text = tr("%1...").arg(QString(codec->name()));
        QAction *action = new QAction(text, this);
        action->setData(codec->name());
        connect(action, SIGNAL(triggered()), this, SLOT(save()));
        saveAsActs.append(action);
    }
}

void AsciiConvertForm::createMenus()
{
    saveAsMenu = new QMenu(tr("&Save As"), this);
    foreach (QAction *action, saveAsActs)
        saveAsMenu->addAction(action);
    connect(saveAsMenu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowSaveAsMenu()));

    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAllAct);
}

void AsciiConvertForm::on_FileCatalog_textEdited(const QString &arg1)
{
    ui->FileTreeWidget->UpdateTreeWidget(arg1, m_FileBrowserSuffixList);
}

void AsciiConvertForm::on_BtnBrowser_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "/",
                                                    QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
        return;
    ui->FileCatalog->setText(dir);
    ui->FileTreeWidget->UpdateTreeWidget(dir, m_FileBrowserSuffixList);
}

void AsciiConvertForm::on_FileTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(item)
    {
        QString tmpString = item->statusTip(0);
        QMdiSubWindow * tmpSubWin = createMdiChild(tmpString);
        if(tmpSubWin)
            tmpSubWin->show();
    }
}

void AsciiConvertForm::on_BtnSaveAll_clicked()
{
    saveAll();
}
