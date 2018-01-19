#ifndef CASETHREAD_H
#define CASETHREAD_H

#include "global.h"
#include <QThread>

class CaseThread : public QThread
{
public:
    CaseThread(const QScriptValue &Value, const int &StepCount);
    void stopThread(bool isStop);
protected:
    void ProcessLoop(int x);
    void run();
private:
    QScriptValue m_ScriptValue;
    int m_StepCount ;
    bool stopThreads ;
};

#endif // CASETHREAD_H
