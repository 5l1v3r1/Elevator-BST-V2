#ifndef MAPSFORM_H
#define MAPSFORM_H

#include "tabpage.h"
#include "lightmaps.h"
#include <QButtonGroup>

namespace Ui {
class MapsForm;
}

class devicelistform;

class MapsForm : public TabPage
{
    Q_OBJECT
public:
    explicit MapsForm(QWidget *parent = 0);
    ~MapsForm();

private slots:
    void slot_ChangeMaps(int index);

    void sessionOpened();
    void chooseOslo();
    void chooseBerlin();
    void chooseJakarta();
    void aboutOsm();

    void on_BtnGo_clicked();

private:
    QNetworkSession *networkSession;

    QMenu           *m_Menu;

private:
    Ui::MapsForm *ui;

    QButtonGroup *m_RadioBtnGroup;
};

#endif // MAPSFORM_H
