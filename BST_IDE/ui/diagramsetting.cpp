#include "diagramsetting.h"
#include "diagramscene.h"
#include "ui_diagramsetting.h"

DiagramSetting::DiagramSetting(GraphicsOperate *pComponent, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DiagramSetting)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/Menu/rc/Menu/setting.png"));
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    if(!ui->EffectEditTree->RefreshTree(pComponent))
        return;
    connect(ui->EffectEditTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(slot_PreviewRC(QTreeWidgetItem*,int)));
    setWindowTitle(tr("%1 Test Case").arg(pComponent->m_ComPath));
}

DiagramSetting::~DiagramSetting()
{
    delete ui;
}

void DiagramSetting::slot_PreviewRC(QTreeWidgetItem *pItem, int pColumn)
{
    int tmpTreeType = pItem->type();
    if(tmpTreeType == SUBRCTREEITEM_TYPE)
    {
        QString tmpFile = pItem->data(1, RC_FILEPATH).toString();
        //>@Ô¤ÀÀ´Ë×ÊÔ´
        FILEPATH_TYPE tmpFileType = ui->EffectEditTree->mComponet->m_UiScene->m_UiManager->RegulatRcFile(tmpFile);
        if(tmpFileType != PATH_TYPENONE)
        {
            ui->LabelPreview->Preview(tmpFile);
        }
    }
}
