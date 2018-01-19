#ifndef SYSZUXPINYIN_H
#define SYSZUXPINYIN_H

#include "globalfun.h"
#include <QWSInputMethod>

class SyszuxIM;
class SyszuxPinyin : public QWidget
{
    Q_OBJECT
public:
    SyszuxPinyin(QWSInputMethod* im);
    ~SyszuxPinyin();

    void InstallKeyboard() {}

    void changePage(int index);
    void matching(QString gemfield);
    void initGb();
    void selectHanziPre();
    void selectHanziNext();
    void changeInputMethod();
    void changeLowerUpper();
    void clearString();

public slots:
    void buttonClickResponse(int gemfield);
signals:
    void sendPinyin(QString gemfield);
    void ResponseKeyEvent(QString keyevent);

public :
    int upper_letter;

private:
    QFile pinyin_file;
    QKeyEvent *event;
    QRegExp regExp;
    QButtonGroup *button_group;
    QMultiMap<QString,QString> pinyin_map;
    QList<QString> pinyin_list;
    QVector<QPushButton*> button_vector;
    int input_method,lower_upper,page_count;
};

#endif

