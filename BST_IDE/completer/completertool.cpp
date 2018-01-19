#include "completertool.h"

CompleterLineEdit::CompleterLineEdit(QWidget *parent, COMPLETER_MODE pMode) :
    QLineEdit(parent)
{
    m_Completer = 0;
    changeModel(pMode);
}

void CompleterLineEdit::CreateCompleter()
{
    ReleaseCompleter();
    m_Completer = new QCompleter;
    m_Completer->setCompletionMode(QCompleter::PopupCompletion);
    m_Completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_Completer->setMaxVisibleItems(10);
    m_Completer->setWrapAround(true);
    setCompleter(m_Completer);
}

void CompleterLineEdit::ReleaseCompleter()
{
    if(m_Completer)
        m_Completer->deleteLater();
}

CompleterLineEdit::~CompleterLineEdit()
{
    if(m_Completer)
        m_Completer->deleteLater();
}

QAbstractItemModel *CompleterLineEdit::modelFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return new QStringListModel(m_Completer);
    QStringList words;

    while (!file.atEnd())
    {
        QByteArray line = file.readLine();
        if (!line.isEmpty())
            words << line.trimmed();
    }
    if (!fileName.contains(QLatin1String("countries.txt")))
        return new QStringListModel(words, m_Completer);
    QStandardItemModel *m = new QStandardItemModel(words.count(), 2, m_Completer);
    for (int i = 0; i < words.count(); ++i)
    {
        QModelIndex countryIdx = m->index(i, 0);
        QModelIndex symbolIdx = m->index(i, 1);
        QString country = words[i].mid(0, words[i].length() - 2).trimmed();
        QString symbol = words[i].right(2);
        m->setData(countryIdx, country);
        m->setData(symbolIdx, symbol);
    }

    return m;
}

void CompleterLineEdit::changeModel(COMPLETER_MODE mode)
{
    CreateCompleter();

    switch (mode)
    {
        case Mode_UnsortedFS:
            { // Unsorted QFileSystemModel
                QFileSystemModel *fsModel = new QFileSystemModel(m_Completer);
                fsModel->setRootPath("");
                m_Completer->setModel(fsModel);
            }
            break;
        case Mode_FullpathFS:
            {   // FileSystemModel that shows full paths
                FileSystemModel *fsModel = new FileSystemModel(m_Completer);
                m_Completer->setModel(fsModel);
                fsModel->setRootPath("");
            }
            break;
        case Mode_Country:
            { // Country List
                m_Completer->setModel(modelFromFile(":/resources/countries.txt"));
                QTreeView *treeView = new QTreeView;
                m_Completer->setPopup(treeView);
                treeView->setRootIsDecorated(false);
                treeView->header()->hide();
                treeView->header()->setStretchLastSection(false);
                treeView->header()->setResizeMode(0, QHeaderView::Stretch);
                treeView->header()->setResizeMode(1, QHeaderView::ResizeToContents);
            }
            break;
        case Mode_Word:
            { // Word list
                m_Completer->setModel(modelFromFile(":/resources/wordlist.txt"));
                m_Completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
            }
            break;
        default:
            ReleaseCompleter();
            return;
    }
}
