#ifndef COMPLETERTOOL_H
#define COMPLETERTOOL_H

#include "globalfun.h"
#include "fsmodel.h"

typedef enum{
    Mode_UnsortedFS = 0,
    Mode_FullpathFS,
    Mode_Country,
    Mode_Word
} COMPLETER_MODE;

class CompleterLineEdit : public QLineEdit
{
public:
    CompleterLineEdit(QWidget *parent=0, COMPLETER_MODE pMode=Mode_FullpathFS);
    ~CompleterLineEdit();

    QAbstractItemModel *modelFromFile(const QString& fileName);

    void CreateCompleter();
    void ReleaseCompleter();
    void changeModel(COMPLETER_MODE mode);

public:
    QCompleter      *m_Completer;
};

#endif // COMPLETERTOOL_H
