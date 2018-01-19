#ifndef IMAGEEFFECTFORM_H
#define IMAGEEFFECTFORM_H

#include "global.h"
#include <QColorDialog>
#include "imageitem.h"
#include "previewlabel.h"

namespace Ui {
class ImageEffectForm;
}

class ImageEffectForm : public QDialog
{
    Q_OBJECT
    
public:
    explicit ImageEffectForm(IMAGEITEMINFO *pItemInfo, QWidget *parent = 0);
    ~ImageEffectForm();

    bool UpdateImage();
    
private slots:
    void on_StartColorBtn_clicked();
    void on_EndColorBtn_clicked();
    void on_TransparentSlider_valueChanged(int value);
    void on_TemplateTree_itemChanged(QTreeWidgetItem *item, int column);
    void accept();
    void reject();
    void UpdatePreview();

    void on_BtnSelect_clicked();
    void slot_ImagePreview(QTreeWidgetItem *pItem, int pColumn);
    void on_BtnAdvanced_clicked(bool checked);

public:
    Ui::ImageEffectForm *ui;
    QColor  m_StartColor, m_EndColor;
    int     m_TransparentValue;
    QPixmap m_EffectPic;
    IMAGEITEMINFO *m_ItemInfo;
};

#endif // IMAGEEFFECTFORM_H
