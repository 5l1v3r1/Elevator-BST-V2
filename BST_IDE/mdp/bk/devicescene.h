#ifndef DEVICESCENE_H
#define DEVICESCENE_H

#include "math.h"
#include "deviceitem.h"
#include "deviceparaform.h"
#include <QGraphicsScene>
#include <QState>
#include <QStateMachine>

#define D_Radius        200
#define D_InitColor     QColor(170,90,45,230)
#define D_ModifyColor   QColor(180,160,100,230)
#define D_ShadowColor   QColor(150,130,100,120)
#define D_SelectColor   QColor(100,0,0,150)

class ItemAnim;
class DeviceScene : public QGraphicsScene
{
    Q_OBJECT
    friend class ItemAnim;
public:
    DeviceScene(QString pMdpPath, QRectF pRect, QObject *parent = 0);
    ~DeviceScene();

    void InitNodeList(QStringList pNodeList);

//>@Effect
    void SetFuzzy(QGraphicsItem *pItem, qreal pBlurRadius)
    {
        QGraphicsBlurEffect *effect = new QGraphicsBlurEffect(this);
        effect->setBlurRadius(pBlurRadius);
        pItem->setGraphicsEffect(effect);
    }
    void SetColor(QGraphicsItem *pItem, QColor pColor)
    {
        QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect(this);
        effect->setColor(pColor);
        pItem->setGraphicsEffect(effect);
    }
    void SetShadow(QGraphicsItem *pItem, QColor pColor, qreal pBlurRadius, QPointF pPointF)
    {
        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
        effect->setColor(pColor);
        effect->setBlurRadius(pBlurRadius);
        effect->setOffset(pPointF);
        pItem->setGraphicsEffect(effect);
    }
    void SetOpacity(QGraphicsItem *pItem, qreal pOpacity)
    {
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);
        effect->setOpacity(pOpacity);
        pItem->setGraphicsEffect(effect);
    }
    void ClearFuzzy(QGraphicsItem *pItem)
    {
        QGraphicsBlurEffect* effect = VAR_CAST<QGraphicsBlurEffect*>(pItem->graphicsEffect());
        if(effect)effect->deleteLater();
    }
    void ClearColor(QGraphicsItem *pItem)
    {
        QGraphicsColorizeEffect* effect = VAR_CAST<QGraphicsColorizeEffect*>(pItem->graphicsEffect());
        if(effect)effect->deleteLater();
    }
    void ClearShadow(QGraphicsItem *pItem)
    {
        QGraphicsDropShadowEffect* effect = VAR_CAST<QGraphicsDropShadowEffect*>(pItem->graphicsEffect());
        if(effect)effect->deleteLater();
    }
    void ClearOpacity(QGraphicsItem *pItem)
    {
        QGraphicsOpacityEffect* effect = VAR_CAST<QGraphicsOpacityEffect*>(pItem->graphicsEffect());
        if(effect)effect->deleteLater();
    }
    void OpenNodeParaForm(QGraphicsItem* pItem);
    bool FindSelectItem(QGraphicsItem*& pItem);

    ItemAnim*                       mItemAnim;
    DeviceParaForm*                 mDeviceParaForm;
private:
//>@
    QMap<QString, DeviceItem*>      mPHNodeMap;
    QFont                           mItemFont;
protected:
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
signals:
    void sAddNode(QString);
    void sDelNodes(QStringList);
public slots:
};

class ItemAnim : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal turn READ Turn WRITE SetTurn)
public:
    ItemAnim(DeviceScene *parent=0):
        QObject(parent),
        mScene(0),
        mMutex(0),
        mStateMachine(0),
        mRootState(0),
        mNodePreAddState(0),
        mNodeAddState(0),
        mNodePreDelState(0),
        mNodeDelState(0),
        mCurDelNameList(QStringList()),
        mTurnAnim(this, "turn", this),
        mTurnIndex(0),
        mAutoTurn(false)
    {
        mScene = parent;
        mMutex = new QMutex(QMutex::Recursive);
        mTurnAnim.setDuration(2000);
        mTurnAnim.setEasingCurve(QEasingCurve::Linear);
        connect(&mTurnAnim, SIGNAL(finished()), this, SLOT(slot_NodeTurnAnimFinished()));
    }
    ~ItemAnim()
    {
        if(mStateMachine)
        {
            mStateMachine->stop();
            ReleasePreAddState();
            ReleaseAddState();
            ReleasePreDelState();
            ReleaseDelState();
            ReleaseRootState();
            mStateMachine->deleteLater();
            mStateMachine = 0;
        }
    }
//>@Init
    void ReleasePreAddState(){if(mNodePreAddState){disconnect(mNodePreAddState, SIGNAL(propertiesAssigned()), 0, 0);mNodePreAddState->deleteLater();mNodePreAddState = 0;}}
    void ReleaseAddState(){if(mNodeAddState){disconnect(mNodeAddState, SIGNAL(propertiesAssigned()), 0, 0);mNodeAddState->deleteLater();mNodeAddState = 0;}}
    void ReleasePreDelState(){if(mNodePreDelState){disconnect(mNodePreDelState, SIGNAL(propertiesAssigned()), 0, 0);mNodePreDelState->deleteLater();mNodePreDelState = 0;}}
    void ReleaseDelState(){if(mNodeDelState){disconnect(mNodeDelState, SIGNAL(propertiesAssigned()), 0, 0);mNodeDelState->deleteLater();mNodeDelState = 0;}}
    void ReleaseRootState(){if(mRootState){mRootState->deleteLater();mRootState = 0;}}
    void ReleaseAnim()
    {
        StopTurn();
        if(mStateMachine)
        {
            mStateMachine->stop();
            StopAdd();
            StopDel();
            mNodeAddAnimGroup.clear();
            mNodeDelAnimGroup.clear();
            ReleasePreAddState();
            ReleaseAddState();
            ReleasePreDelState();
            ReleaseDelState();
            ReleaseRootState();
            mStateMachine->deleteLater();
            mStateMachine = 0;
        }
    }
    void InitAnim()
    {
        ReleaseAnim();
        mStateMachine = new QStateMachine();
        mRootState = new QState(mStateMachine);
        mNodePreAddState = new QState(mRootState);
        mNodeAddState = new QState(mRootState);
        connect(mNodeAddState, SIGNAL(propertiesAssigned()), this, SLOT(slot_NodeAddStateFinished()));
        mNodePreDelState = new QState(mRootState);
        mNodeDelState = new QState(mRootState);
        connect(mNodeDelState, SIGNAL(propertiesAssigned()), this, SLOT(slot_NodeDelStateFinished()));
        mStateMachine->setInitialState(mRootState);
    }
//>@Fun
    void StopTurn()
    {
        mTurnAnim.stop();
        do{QtSleep(1);}while(mTurnAnim.state() == QPropertyAnimation::Running);
    }
    void StopAdd()
    {
        mNodeAddAnimGroup.stop();
        do{QtSleep(1);}while(mNodeAddAnimGroup.state() == QPropertyAnimation::Running);
    }
    void StopDel()
    {
        mNodeDelAnimGroup.stop();
        do{QtSleep(1);}while(mNodeDelAnimGroup.state() == QPropertyAnimation::Running);
    }
    bool AddAnimIsRunning()
    {
        if(mNodeAddAnimGroup.state() == QParallelAnimationGroup::Running)
            return true;
        return false;
    }
    bool TurnAnimIsRunning()
    {
        if(mTurnAnim.state() == QParallelAnimationGroup::Running)
            return true;
        return false;
    }
    bool DelAnimIsRunning()
    {
        if(mNodeDelAnimGroup.state() == QParallelAnimationGroup::Running)
            return true;
        return false;
    }
//>@Anim
    void AddNode()
    {
        InitAnim();
        mTurnIndex = 0;
        mCurDelNameList.clear();
        QStringList keyList = mScene->mPHNodeMap.keys();
        keyList.removeOne("Cpu");
        keyList.removeOne("Mem");
        keyList.removeOne("Net");
        int cnt = keyList.count();
        for(int i=0;i<cnt;i++)
        {
            QString key = keyList.at(i);
            DeviceItem* item = mScene->mPHNodeMap.value(key);
            if(!item)continue;
            mNodePreAddState->assignProperty(item, "pos", item->scenePos());
            double left = cos(2*M_PI*(0+i)/cnt)*D_Radius;
            double top = sin(2*M_PI*(0+i)/cnt)*D_Radius;
            mNodeAddState->assignProperty(item, "geometry", QRectF(left,top,30,30));
            QPropertyAnimation* geometryAnim = new QPropertyAnimation(item, "geometry", this);
            geometryAnim->setDuration(3000);
            geometryAnim->setEasingCurve(QEasingCurve::Linear);
            mNodeAddAnimGroup.addAnimation(geometryAnim);
            mNodeAddState->assignProperty(item, "rotation", qreal(360));
            QPropertyAnimation* rotationAnim = new QPropertyAnimation(item, "rotation", this);
            rotationAnim->setDuration(3000);
            rotationAnim->setEasingCurve(QEasingCurve::Linear);
            mNodeAddAnimGroup.addAnimation(rotationAnim);
        }
        mRootState->addTransition(mNodePreAddState, SIGNAL(propertiesAssigned()), mNodeAddState)->addAnimation(&mNodeAddAnimGroup);
        mRootState->setInitialState(mNodePreAddState);
        mStateMachine->start();
    }
    void DelNode(QString pName)
    {
        InitAnim();
        mTurnIndex = 0;
        if(!mCurDelNameList.contains(pName))
            mCurDelNameList.append(pName);
        for(int i=0;i<mCurDelNameList.count();i++)
        {
            QString delName = mCurDelNameList.at(i);
            DeviceItem* item = mScene->mPHNodeMap.value(delName);
            if(!item)continue;
            mNodePreDelState->assignProperty(item, "pos", item->scenePos());
            MDPConfigInfo *configInfo = GlobalPara.m_ConfigInfoGroup.value(delName);
            mNodeDelState->assignProperty(item, "geometry", configInfo->mRectF);
            QPropertyAnimation* geometryAnim = new QPropertyAnimation(item, "geometry", this);
            geometryAnim->setDuration(2000);
            geometryAnim->setEasingCurve(QEasingCurve::InCubic);
            mNodeDelAnimGroup.addAnimation(geometryAnim);
            mNodeDelState->assignProperty(item, "rotation", qreal(1080));
            QPropertyAnimation* rotationAnim = new QPropertyAnimation(item, "rotation", this);
            rotationAnim->setDuration(2000);
            rotationAnim->setEasingCurve(QEasingCurve::InQuart);
            mNodeDelAnimGroup.addAnimation(rotationAnim);
        }
        mRootState->addTransition(mNodePreDelState, SIGNAL(propertiesAssigned()), mNodeDelState)->addAnimation(&mNodeDelAnimGroup);
        mRootState->setInitialState(mNodePreDelState);
        mStateMachine->start();
    }
    void TurnNode(int pDex)
    {
        if(mTurnAnim.state()==QPropertyAnimation::Running)
            return;
        mTurnAnim.setEndValue(pDex);
        mTurnAnim.start();
    }

    DeviceScene*                    mScene;
    QMutex*                         mMutex;
    QParallelAnimationGroup         mNodeAddAnimGroup,
                                    mNodeDelAnimGroup;
    QStateMachine                   *mStateMachine;
    QState                          *mRootState,
                                    *mNodePreAddState,
                                    *mNodeAddState,
                                    *mNodePreDelState,
                                    *mNodeDelState;
    QPropertyAnimation              mTurnAnim;
    qreal                           mTurnIndex;
    bool                            mAutoTurn;
    QStringList                     mCurDelNameList;
private:
    qreal Turn(){return mTurnIndex;}
    void SetTurn(qreal pIndex)
    {
        mTurnIndex = pIndex;
        QStringList keyList = mScene->mPHNodeMap.keys();
        keyList.removeOne("Cpu");
        keyList.removeOne("Mem");
        keyList.removeOne("Net");
        int cnt = keyList.count();
        for(int i=0;i<cnt;i++)
        {
            QString key = keyList.at(i);
            if((!key.compare("Cpu", Qt::CaseInsensitive))||
               (!key.compare("Mem", Qt::CaseInsensitive))||
               (!key.compare("Net", Qt::CaseInsensitive)))
                continue;
            DeviceItem* item = mScene->mPHNodeMap.value(key);
            if(!item)continue;
            double rx = cos((2*M_PI*(mTurnIndex+i))/cnt)*D_Radius;
            double ry = sin((2*M_PI*(mTurnIndex+i))/cnt)*D_Radius;
            item->setPos(QPointF(rx, ry));
        }
        mScene->update();
    }
protected:
signals:
    void sDelNodes(QStringList);
public slots:
    void slot_NodeAddStateFinished()
    {
        IDE_DEBUG("NodeAdd");
        mAutoTurn = true;
        slot_NodeTurnAnimFinished();
    }
    void slot_NodeTurnAnimFinished()
    {
        if(mAutoTurn)
        {
            QtSleep(300);
            int i = mTurnIndex;
            TurnNode(++i);
        }
    }
    void slot_NodeDelStateFinished()
    {
        IDE_DEBUG("NodeDel");
        for(int i=0;i<mCurDelNameList.count();i++)
        {
            QString delName = mCurDelNameList.at(i);
            DeviceItem* delItem = mScene->mPHNodeMap.take(delName);
            mScene->removeItem(delItem);
            delItem->deleteLater();
        }
        emit sDelNodes(mCurDelNameList);
        mCurDelNameList.clear();
        AddNode();
    }
};

#endif // DEVICESCENE_H
