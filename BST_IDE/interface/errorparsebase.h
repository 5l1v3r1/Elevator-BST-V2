#ifndef ERRORPARSEBASE_H
#define ERRORPARSEBASE_H

#include "globalfun.h"

class ErrorParseBase : public QObject
{
    Q_OBJECT
public:
    explicit ErrorParseBase(QObject *parent = 0);
    
signals:
    
public slots:
    
public:
    QQueue<QByteArray>      mFrameBuffer;
};

#endif // ERRORPARSEBASE_H
