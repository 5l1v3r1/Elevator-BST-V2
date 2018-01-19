#ifndef TABPAGE_H
#define TABPAGE_H

#include <global.h>
#include <QUndoCommand>

class TabPage : public QWidget
{
    Q_OBJECT
public:
    explicit TabPage(QWidget *parent = 0);
    ~TabPage();
    
    void InitUndoStack();
    void ReleaseUndoStack();

    bool isSaved() {return !m_IsChanged;}

public slots:
    virtual void slot_Save() {}
    
public:
    QUndoStack      *m_UndoStack;
    QAction         *m_UndoAction,
                    *m_RedoAction;

    bool             m_IsChanged;
};

#endif // TABPAGE_H
