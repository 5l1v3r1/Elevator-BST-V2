#include "imageeditor.h"
#include "ui_imageeditor.h"

ImageEditor::ImageEditor(QWidget *parent) :
    TabPage(parent),
    ui(new Ui::ImageEditor)
{
    ui->setupUi(this);

    ui->TextSetGroup->setShown(false);

    CreateSence();

    //ComboxFont;
    connect(ui->ComboxFont, SIGNAL(currentFontChanged(QFont)), this, SLOT(currentFontChanged(QFont)));

    //ComboxFontSize
    for(int i=1;i<100;i++)
        ui->ComboxFontSize->addItem(QString("%1").arg(i), 0);
    ui->ComboxFontSize->setCurrentIndex(9);
    connect(ui->ComboxFontSize, SIGNAL(currentIndexChanged(QString)), this, SLOT(fontSizeChanged(QString)));

    FontBtnGroup = new QButtonGroup(this);
    FontBtnGroup->setExclusive(false);
    connect(FontBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(FontBtnGroupClicked(int)));
    FontBtnGroup->addButton(ui->BtnBold, 0);
    FontBtnGroup->addButton(ui->BtnItalic, 1);

    //ToolBtnFontColor
    ui->ToolBtnFontColor->setPopupMode(QToolButton::MenuButtonPopup);
    ui->ToolBtnFontColor->setMenu(createColorMenu(SLOT(textColorChanged()),Qt::black));
    lineAction = ui->ToolBtnFontColor->menu()->defaultAction();
    connect(ui->ToolBtnFontColor, SIGNAL(clicked()), this, SLOT(textButtonTriggered()));

    //ToolBtnFillColor
    ui->ToolBtnFillColor->setPopupMode(QToolButton::MenuButtonPopup);
    ui->ToolBtnFillColor->setMenu(createColorMenu(SLOT(itemColorChanged()),Qt::white));
    fillAction = ui->ToolBtnFillColor->menu()->defaultAction();
    connect(ui->ToolBtnFillColor, SIGNAL(clicked()),this, SLOT(fillButtonTriggered()));

    //建立画图工具按钮的信号插槽连接
    connect(ui->BtnToFront, SIGNAL(clicked()), this, SLOT(bringToFront()));
    connect(ui->BtnToBack, SIGNAL(clicked()), this, SLOT(sendToBack()));

    //connect(ui->BtnBold, SIGNAL(toggled(bool)), m_Scene, SLOT(itemSelected(QGraphicsItem*)));
    //connect(ui->BtnItalic, SIGNAL(toggled(bool)), m_Scene, SLOT(itemSelected(QGraphicsItem*)));

    ItemBtnGroup = new QButtonGroup(this);
    ItemBtnGroup->setExclusive(false);
    connect(ItemBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(ItemBtnGroupClicked(int)));
    ItemBtnGroup->addButton(ui->BtnTextItem, int(ITEM_TEXT));
    ItemBtnGroup->addButton(ui->BtnPicItem, int(ITEM_LAY));

    m_ImageList.clear();

    ui->ImageTable->setEditTriggers(QTableView::NoEditTriggers);
    ui->ImageTable->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    ui->ImageTable->horizontalHeader()->setStretchLastSection(true);  //列宽度自适应
    ui->ImageTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    ui->LayTable->setEditTriggers(QTableView::NoEditTriggers);
    ui->LayTable->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    ui->LayTable->horizontalHeader()->setStretchLastSection(true);  //列宽度自适应
    ui->LayTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    connect(m_Scene, SIGNAL(itemInserted()), this, SLOT(RefreshLayListWidget()));
}

ImageEditor::~ImageEditor()
{
    delete ui;
}

void ImageEditor::CreateSence()
{
    //视图区域
    m_Scene = new ImageScene(this);

    connect(m_Scene, SIGNAL(mouseRelease()), this, SLOT(mouseRelease()));
    connect(m_Scene, SIGNAL(itemSelected(QGraphicsItem*)), this, SLOT(itemSelected(QGraphicsItem*)));

    ui->ImageView->setScene(m_Scene);
    ui->ImageView->setRenderHint(QPainter::Antialiasing);
    //ui->ImageView->setBackgroundBrush(QPixmap(":/Page/rc/Page/background.png"));
    ui->ImageView->setCacheMode(QGraphicsView::CacheBackground);
    ui->ImageView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    ui->ImageView->setDragMode(QGraphicsView::RubberBandDrag);
}

void ImageEditor::ItemBtnGroupClicked(int id)
{
    QList<QAbstractButton *> buttons = ItemBtnGroup->buttons();
    foreach (QAbstractButton *button, buttons)
    {
        if (ItemBtnGroup->button(id) != button)
        {
            button->setChecked(false);
        }
    }
    m_Scene->SetType((IMAGEITEM_TYPE)id);
    m_Scene->SetMode(CodeInsert);
    if(m_Scene->myItemType != ITEM_TEXT)
    {
        ui->TextSetGroup->setShown(false);
    }
}

void ImageEditor::FontBtnGroupClicked(int id)
{
    fontSizeChanged(QString());
}

QMenu *ImageEditor::createColorMenu(const char *slot, QColor defaultColor)
{
    QList<QColor> colors;
    colors << Qt::black << Qt::white << Qt::gray << Qt::red << Qt::green << Qt::blue << Qt::yellow;
    QStringList names;
    names << tr("black") << tr("white") << tr("gray") << tr("red") << tr("green") << tr("blue") << tr("yellow");

    QMenu *colorMenu = new QMenu(this);
    for (int i = 0; i < colors.count(); ++i)
    {
        QAction *action = new QAction(names.at(i), this);
        action->setData(colors.at(i));
        action->setIcon(createColorIcon(colors.at(i)));
        connect(action, SIGNAL(triggered()), this, slot);
        colorMenu->addAction(action);
        if (colors.at(i) == defaultColor)
        {
            colorMenu->setDefaultAction(action);
        }
    }
    return colorMenu;
}

QIcon ImageEditor::createColorToolButtonIcon(const QString &imageFile, QColor color)
{
    QPixmap pixmap(50, 80);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPixmap image(imageFile);
    QRect target(0, 0, 50, 60);
    QRect source(0, 0, 42, 42);
    painter.fillRect(QRect(0, 60, 50, 80), color);
    painter.drawPixmap(target, image, source);

    return QIcon(pixmap);
}

QIcon ImageEditor::createColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);
    return QIcon(pixmap);
}

void ImageEditor::RefreshLayListWidget()
{
    int num = ui->LayTable->rowCount();
    if(num>0)
    {
        for(int i=num-1;i>=0;i--)
        {
            ui->LayTable->removeRow(i);
        }
    }
    for(int i=0;i<m_Scene->m_LayList.count();i++)
    {
        IMAGEITEMINFO* tmpImage = m_Scene->m_LayList.at(i);
        if(tmpImage)
        {
            ui->LayTable->insertRow(i);
            QTableWidgetItem *tmpItem = new QTableWidgetItem(tmpImage->mName,0);
            //tmpItem->setTextAlignment(Qt::AlignCenter);//Text居中
            ui->LayTable->setItem(i, 0, tmpItem);
            if(tmpImage->mSelectFlag)
            {
                ui->LayTable->setCurrentCell(i, 0);
            }
        }
    }
}

void ImageEditor::on_LayTable_activated(const QModelIndex &index)
{
    if(index.row()>0)
    {
        IMAGEITEMINFO* tmpImage = m_Scene->m_LayList.at(index.row());
        if(tmpImage)
        {
            m_Scene->ItemOperate(tmpImage, CodeSelect);
        }
    }
}

void ImageEditor::mouseRelease()
{
    m_Scene->SetMode(CodeSelect);
    QList<QAbstractButton *> buttons = ItemBtnGroup->buttons();
    foreach (QAbstractButton *button, buttons)
    {
       button->setChecked(false);
    }
    ui->TextSetGroup->setShown(false);
    RefreshLayListWidget();
}

void ImageEditor::itemSelected(QGraphicsItem *item)
{
    TextItem *textItem = qgraphicsitem_cast<TextItem *>(item);
    if(textItem)
    {
        //重新选中某个控件后会自动将这个控件的Font信息同步到几个控制按钮，
        QFont font = textItem->font();
        QColor color = textItem->defaultTextColor();
        ui->ComboxFont->setCurrentFont(font);
        ui->ComboxFontSize->setEditText(QString().setNum(font.pointSize()));

        QMenu* tmpMenu = ui->ToolBtnFillColor->menu();
        QList<QAction*> tmpActionList = tmpMenu->actions();
        for(int i=0;i<tmpActionList.count();i++)
        {
            QAction *tmpAction = tmpActionList.at(i);
            if(qVariantValue<QColor>(tmpAction->data()) == color)
            {
                IDE_TRACE();
                tmpMenu->setDefaultAction(tmpAction);
                break;
            }
            IDE_TRACE();
        }

        ui->BtnBold->setChecked(font.weight() == QFont::Bold);
        ui->BtnItalic->setChecked(font.italic());
    }
}

void ImageEditor::currentFontChanged(const QFont &)
{
    QFont font = ui->ComboxFont->currentFont();
    font.setPointSize(ui->ComboxFontSize->currentText().toInt());
    font.setWeight(ui->BtnBold->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(ui->BtnItalic->isChecked());

    m_Scene->setFont(font);
}

void ImageEditor::fontSizeChanged(const QString &)
{
    QFont font = ui->ComboxFont->currentFont();
    font.setPointSize(ui->ComboxFontSize->currentText().toInt());
    font.setWeight(ui->BtnBold->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(ui->BtnItalic->isChecked());

    m_Scene->setFont(font);
}

void ImageEditor::textColorChanged()
{
    textAction = VAR_CAST<QAction *>(sender());
    if(textAction)
    {
        ui->ToolBtnFontColor->setIcon(createColorToolButtonIcon(
                    ":/Menu/rc/Menu/textpointer.png",
                    qVariantValue<QColor>(textAction->data())));
        textButtonTriggered();
    }
}

void ImageEditor::itemColorChanged()
{
    fillAction = VAR_CAST<QAction *>(sender());
    if(fillAction)
    {
        ui->ToolBtnFillColor->setIcon(createColorToolButtonIcon(
                     ":/Menu/rc/Menu/floodfill.png",
                     qVariantValue<QColor>(fillAction->data())));
        fillButtonTriggered();
    }
}

void ImageEditor::textButtonTriggered()
{
    if(textAction)
        m_Scene->setTextColor(qVariantValue<QColor>(textAction->data()));
}

void ImageEditor::fillButtonTriggered()
{
    if(fillAction)
        m_Scene->setItemColor(qVariantValue<QColor>(fillAction->data()));
}


void ImageEditor::bringToFront()
{
    if (m_Scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = m_Scene->selectedItems().first();
    if(selectedItem->type() == ITEM_BASE)
        return;

    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems)
    {
        if (item->zValue() >= zValue && ((item->type() > ITEM_LAY) && (item->type() < ITEM_TEXT)))
            zValue = item->zValue() + 0.1;
    }
    selectedItem->setZValue(zValue);
}

void ImageEditor::sendToBack()
{
    if (m_Scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = m_Scene->selectedItems().first();
    if(selectedItem->type() == ITEM_BASE)
        return;

    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems)
    {
        if (item->zValue() <= zValue && ((item->type() > ITEM_LAY) && (item->type() < ITEM_TEXT)))
            zValue = item->zValue() - 0.1;
    }
    selectedItem->setZValue(zValue);
}


void ImageEditor::on_ZoomSlider_valueChanged(int value)
{
    if(value < 10 || value >= 100)
        return;
    double newScale = (double)(value+1) * 2.0 / 100.0;
    QMatrix oldMatrix = ui->ImageView->matrix();
    ui->ImageView->resetMatrix();
    ui->ImageView->translate(oldMatrix.dx(), oldMatrix.dy());
    ui->ImageView->scale(newScale, newScale);
}

void ImageEditor::on_ZoomOut_clicked()
{
    int pos = ui->ZoomSlider->value();
    if(pos>=10)
        ui->ZoomSlider->setValue(pos-10);
    else
        ui->ZoomSlider->setValue(0);
    on_ZoomSlider_valueChanged(ui->ZoomSlider->value());
}

void ImageEditor::on_ZoomIn_clicked()
{
    int pos = ui->ZoomSlider->value();
    if(pos<90)
        ui->ZoomSlider->setValue(pos+10);
    else
        ui->ZoomSlider->setValue(99);
    on_ZoomSlider_valueChanged(ui->ZoomSlider->value());
}

void ImageEditor::RefreshImageListWidget()
{
    int num = ui->ImageTable->rowCount();
    if(num>0)
    {
        for(int i=num-1;i>=0;i--)
        {
            ui->ImageTable->removeRow(i);
        }
    }
    for(int i=0;i<m_ImageList.count();i++)
    {
        IMAGELIST *tmpImage = m_ImageList.at(i);
        if(tmpImage)
        {
            ui->ImageTable->insertRow(i);
            QTableWidgetItem *tmpItem = new QTableWidgetItem(QIcon(tmpImage->mPic), tmpImage->mName,0);
            //tmpItem->setTextAlignment(Qt::AlignCenter);//Text居中
            ui->ImageTable->setItem(i, 0, tmpItem);
        }
    }
}

void ImageEditor::on_ImageAdd_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(0, tr("Add Image File"),
                                         m_CurRcDir,
                                         QString("*.bmp *.png *.jpg *.svg"));
    if (fileName.isEmpty() || !isFileExist(fileName))
    {
        IDE_TRACE();
        return;
    }
    m_CurRcDir = getFileDirectory(fileName);

    IMAGELIST *tmpImage = new IMAGELIST(fileName);
    if(tmpImage->isValid() == false)
    {
        QMessageBox::warning(this, tr("Error"),
                                   tr("%1 is damaged,Please check it!"),
                             QMessageBox::Yes,
                             QMessageBox::Yes);
        delete tmpImage;
        return;
    }
    m_ImageList.append(tmpImage);
    RefreshImageListWidget();
    ui->ImageTable->setCurrentCell(m_ImageList.count()-1, 0);
}

void ImageEditor::on_ImageDel_clicked()
{
    int row = ui->ImageTable->currentRow();
    if(row<0)
        return;
    IMAGELIST *tmpImage = m_ImageList.at(row);
    if(tmpImage)
    {
        delete tmpImage;
        m_ImageList.removeAt(row);
        RefreshImageListWidget();
        if(row>0)
            ui->ImageTable->setCurrentCell(row-1, 0);
    }
}

void ImageEditor::on_ImageClear_clicked()
{
    for(int i=0;i<m_ImageList.count();i++)
    {
        IMAGELIST *tmpImage = m_ImageList.at(i);
        if(tmpImage)
            delete tmpImage;
    }
    m_ImageList.clear();
    RefreshImageListWidget();
}

void ImageEditor::on_ImageUp_clicked()
{
    int row = ui->ImageTable->currentRow();
    if(row<0)
        return;
    IMAGELIST *tmpImage = m_ImageList.at(row);
    if(tmpImage && row>0)
    {
        m_ImageList.move(row, row-1);
        RefreshImageListWidget();
        ui->ImageTable->setCurrentCell(row-1, 0);
    }
}

void ImageEditor::on_ImageDown_clicked()
{
    int row = ui->ImageTable->currentRow();
    if(row<0)
        return;
    IMAGELIST *tmpImage = m_ImageList.at(row);
    if(tmpImage && row < m_ImageList.count()-1)
    {
        m_ImageList.move(row, row+1);
        RefreshImageListWidget();
        ui->ImageTable->setCurrentCell(row+1, 0);
    }
}

void ImageEditor::on_ImageTable_doubleClicked(const QModelIndex &index)
{
    IMAGELIST *tmpImage = m_ImageList.at(index.row());
    if(tmpImage)
    {
        IDE_TRACE();
        m_Scene->UpdateScene(tmpImage->mPic);
    }
}
