#include "tabpage.h"

TabPage::TabPage(QWidget *parent) :
    QWidget(parent)
{
    m_IsChanged = false;
    m_UndoStack = 0;
    m_UndoAction = 0;
    m_RedoAction = 0;
    InitUndoStack();
}

TabPage::~TabPage()
{
    ReleaseUndoStack();
}

void TabPage::InitUndoStack()
{
    ReleaseUndoStack();
    m_UndoStack = new QUndoStack(this);
    m_UndoAction = m_UndoStack->createUndoAction(this, tr("&Undo"));
    m_UndoAction->setShortcuts(QKeySequence::Undo);

    m_RedoAction = m_UndoStack->createRedoAction(this, tr("&Redo"));
    m_RedoAction->setShortcuts(QKeySequence::Redo);
}

void TabPage::ReleaseUndoStack()
{
    if(m_UndoStack)
    {
        m_UndoAction->deleteLater();
        m_RedoAction->deleteLater();
        m_UndoStack->clear();
        m_UndoStack->deleteLater();
        m_UndoStack = 0;
    }
}
