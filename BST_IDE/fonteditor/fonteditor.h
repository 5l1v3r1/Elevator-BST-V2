#ifndef FONTEDITOR_H
#define FONTEDITOR_H

#include "tabpage.h"
#include "fontscene.h"

namespace Ui {
class FontEditor;
}

class FontEditor : public TabPage
{
    Q_OBJECT
public:
    explicit FontEditor(QWidget *parent = 0);
    ~FontEditor();

    void CreateSence();

    void RefreshLayListWidget();
    
private slots:
    void FontBtnGroupClicked(int id);
    void ItemBtnGroupClicked(int id);

    void mouseRelease();
    void itemSelected(QGraphicsItem *item);

    QMenu *createColorMenu(const char *slot, QColor defaultColor);
    QIcon createColorToolButtonIcon(const QString &imageFile, QColor color);
    QIcon createColorIcon(QColor color);

    void currentFontChanged(const QFont &font);
    void fontSizeChanged(const QString &size);

    void textColorChanged();
    void textButtonTriggered();

private:
    Ui::FontEditor *ui;
    FontScene      *m_Scene;
    QButtonGroup   *FontBtnGroup;     //文字功能组
    QButtonGroup   *ItemBtnGroup;
    QAction        *textAction;
};

#endif // FONTEDITOR_H
