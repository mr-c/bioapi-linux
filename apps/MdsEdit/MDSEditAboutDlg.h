/****************************************************************************
** Form interface generated from reading ui file 'MDSEditAboutDlg.ui'
**
** Created: Sun Oct 21 22:51:44 2012
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MDSEDITABOUTDLG_H
#define MDSEDITABOUTDLG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QGroupBox;
class QPushButton;

class MdsEditAboutDlg : public QDialog
{
    Q_OBJECT

public:
    MdsEditAboutDlg( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~MdsEditAboutDlg();

    QLabel* m_pixmapLabel1;
    QGroupBox* m_groupBox;
    QLabel* PixmapLabel3;
    QLabel* PixmapLabel4;
    QLabel* PixmapLabel5;
    QLabel* PixmapLabel2;
    QLabel* TextLabel4;
    QLabel* TextLabel3;
    QLabel* m_label2;
    QLabel* TextLabel5;
    QPushButton* m_okButton;
    QLabel* m_label1;
    QLabel* TextLabel6;
    QLabel* TextLabel1;

protected:

protected slots:
    virtual void languageChange();

};

#endif // MDSEDITABOUTDLG_H
