#include "fonteditor.h"
#include "ui_fonteditor.h"

FontEditor::FontEditor(QWidget *parent) :
    TabPage(parent),
    ui(new Ui::FontEditor)
{
    ui->setupUi(this);
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

    ItemBtnGroup = new QButtonGroup(this);
    ItemBtnGroup->setExclusive(false);
    connect(ItemBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(ItemBtnGroupClicked(int)));
    ItemBtnGroup->addButton(ui->BtnTextItem, int(ITEM_TEXT));

    ui->LayTable->setEditTriggers(QTableView::NoEditTriggers);
    ui->LayTable->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    ui->LayTable->horizontalHeader()->setStretchLastSection(true);  //列宽度自适应
    ui->LayTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    connect(m_Scene, SIGNAL(itemInserted()), this, SLOT(RefreshLayListWidget()));
}

FontEditor::~FontEditor()
{
    delete ui;
}

void FontEditor::CreateSence()
{
    //视图区域
    m_Scene = new FontScene(this);

    connect(m_Scene, SIGNAL(mouseRelease()), this, SLOT(mouseRelease()));
    connect(m_Scene, SIGNAL(itemSelected(QGraphicsItem*)), this, SLOT(itemSelected(QGraphicsItem*)));

    ui->FontView->setScene(m_Scene);
    ui->FontView->setRenderHint(QPainter::Antialiasing);
    //ui->FontView->setBackgroundBrush(QPixmap(":/Page/rc/Page/background.png"));
    ui->FontView->setCacheMode(QGraphicsView::CacheBackground);
    ui->FontView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    ui->FontView->setDragMode(QGraphicsView::RubberBandDrag);
}
void FontEditor::ItemBtnGroupClicked(int id)
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
    IDE_TRACE();
}

void FontEditor::FontBtnGroupClicked(int id)
{
    fontSizeChanged(QString());
}

QMenu *FontEditor::createColorMenu(const char *slot, QColor defaultColor)
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

QIcon FontEditor::createColorToolButtonIcon(const QString &imageFile, QColor color)
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

QIcon FontEditor::createColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);
    return QIcon(pixmap);
}


void FontEditor::mouseRelease()
{
    m_Scene->SetMode(CodeSelect);
    QList<QAbstractButton *> buttons = ItemBtnGroup->buttons();
    foreach (QAbstractButton *button, buttons)
    {
       button->setChecked(false);
    }
    RefreshLayListWidget();
}

void FontEditor::RefreshLayListWidget()
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
        TEXTINFO* tmpTextItem = m_Scene->m_LayList.at(i);
        if(tmpTextItem)
        {
            ui->LayTable->insertRow(i);
            QTableWidgetItem *tmpItem = new QTableWidgetItem(tmpTextItem->mName,0);
            //tmpItem->setTextAlignment(Qt::AlignCenter);//Text居中
            ui->LayTable->setItem(i, 0, tmpItem);
        }
    }
}

void FontEditor::itemSelected(QGraphicsItem *item)
{
    TextItem *textItem = qgraphicsitem_cast<TextItem *>(item);
    if(textItem)
    {
        //重新选中某个控件后会自动将这个控件的Font信息同步到几个控制按钮，
        QFont font = textItem->font();
        QColor color = textItem->defaultTextColor();
        ui->ComboxFont->setCurrentFont(font);
        ui->ComboxFontSize->setEditText(QString().setNum(font.pointSize()));

        QMenu* tmpMenu = ui->ToolBtnFontColor->menu();
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

void FontEditor::currentFontChanged(const QFont &)
{
    QFont font = ui->ComboxFont->currentFont();
    font.setPointSize(ui->ComboxFontSize->currentText().toInt());
    font.setWeight(ui->BtnBold->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(ui->BtnItalic->isChecked());

    m_Scene->setFont(font);
}

void FontEditor::fontSizeChanged(const QString &)
{
    QFont font = ui->ComboxFont->currentFont();
    font.setPointSize(ui->ComboxFontSize->currentText().toInt());
    font.setWeight(ui->BtnBold->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(ui->BtnItalic->isChecked());

    m_Scene->setFont(font);
}

void FontEditor::textColorChanged()
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


void FontEditor::textButtonTriggered()
{
    if(textAction)
        m_Scene->setTextColor(qVariantValue<QColor>(textAction->data()));
}


