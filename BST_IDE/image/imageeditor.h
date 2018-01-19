#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include "tabpage.h"
#include "imagescene.h"

namespace Ui {
class ImageEditor;
}

class IMAGELIST
{
public:
    IMAGELIST(QString pImage)
    {
        mValid = false;
        if(mPic.load(pImage))
        {
            mName = getFileName(pImage);
            mPath = getFileDirectory(pImage);
            mType = getRcType(mName);
            mValid = true;
        }
    }
    ~IMAGELIST()
    {
    }
    bool            mValid;
    QPixmap         mPic;
    QString         mName;
    QString         mPath;
    RC_TYPE         mType;
    bool isValid() {return mValid;}
public:
    bool operator==(const IMAGELIST &s)
    {
        if(mValid != s.mValid)
            return false;
        if(mName.compare(s.mName, Qt::CaseInsensitive))
            return false;
        if(mPath.compare(s.mPath, Qt::CaseInsensitive))
            return false;
        return true;
    }
    bool operator!=(const IMAGELIST &s)  { return !operator==(s); }
};

class ImageEditor : public TabPage
{
    Q_OBJECT
public:
    explicit ImageEditor(QWidget *parent = 0);
    ~ImageEditor();

    void CreateSence();
    
private slots:
    void FontBtnGroupClicked(int id);
    void ItemBtnGroupClicked(int id);

    void mouseRelease();
    void itemSelected(QGraphicsItem *item);

    QMenu *createColorMenu(const char *slot, QColor defaultColor);
    QIcon createColorToolButtonIcon(const QString &imageFile, QColor color);
    QIcon createColorIcon(QColor color);

    void bringToFront();
    void sendToBack();

    void currentFontChanged(const QFont &font);
    void fontSizeChanged(const QString &size);

    void textColorChanged();
    void itemColorChanged();

    void textButtonTriggered();
    void fillButtonTriggered();

    void on_ZoomSlider_valueChanged(int value);
    void on_ZoomOut_clicked();
    void on_ZoomIn_clicked();

    void on_ImageAdd_clicked();
    void on_ImageDel_clicked();
    void on_ImageClear_clicked();
    void on_ImageUp_clicked();
    void on_ImageDown_clicked();
    void RefreshImageListWidget();
    void on_ImageTable_doubleClicked(const QModelIndex &index);
    void RefreshLayListWidget();
    void on_LayTable_activated(const QModelIndex &index);

private:
    Ui::ImageEditor *ui;
    ImageScene      *m_Scene;

    QButtonGroup *FontBtnGroup;     //文字功能组
    QButtonGroup *ItemBtnGroup;     //图像元素组

    QAction *textAction;
    QAction *fillAction;
    QAction *lineAction;

    QList<IMAGELIST*>  m_ImageList;

    QString            m_CurRcDir;
};

#endif // IMAGEEDITOR_H
