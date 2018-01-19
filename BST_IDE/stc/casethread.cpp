#include "casethread.h"

CaseThread::CaseThread(const QScriptValue &Value, const int &StepCount) : QThread()
{
    m_ScriptValue = Value;
    m_StepCount = StepCount;
    stopThreads = false;
}

void CaseThread::stopThread(bool isStop)
{
    stopThreads = isStop;
}

void CaseThread::ProcessLoop(int x)
{
    QScriptValueList args;
    args << x << 2;
    QScriptValue result = m_ScriptValue.call(QScriptValue(), args);
//    IDE_TRACE_STR(result.toString());
}

void CaseThread::run() {
    for(int i = 0; (i<m_StepCount) && (!stopThreads); i++)
    {
        ProcessLoop(i);
    }
}

