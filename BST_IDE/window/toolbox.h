#ifndef TOOLBOX_H
#define TOOLBOX_H

#include "global.h"
#include "upitem.h"

class ToolBox : public UpQTreeWidget
{
    Q_OBJECT
public:
    ToolBox(QWidget *parent = 0);
    void InitToolBox(FILE_TYPE pType);
    void AddTools(QString ToolName, QString ToolIcon);

public slots:
    void slot_ToolTreeStartDrag(Qt::DropActions pAction);

public:
    FILE_TYPE  m_CurToolType;
};

#endif // TOOLBOX_H
