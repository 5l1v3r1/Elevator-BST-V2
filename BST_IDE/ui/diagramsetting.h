#ifndef DIAGRAMSETTING_H
#define DIAGRAMSETTING_H

#include "propertytree.h"

namespace Ui {
class DiagramSetting;
}

class DiagramSetting : public QDialog
{
    Q_OBJECT
public:
    explicit DiagramSetting(GraphicsOperate *pComponent, QWidget *parent = 0);
    ~DiagramSetting();

public slots:
    void slot_PreviewRC(QTreeWidgetItem *pItem, int pColumn);

public:
    Ui::DiagramSetting *ui;
};

#endif // DIAGRAMSETTING_H
