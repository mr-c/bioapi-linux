/****************************************************************************
** Form interface generated from reading ui file 'mainwindow.ui'
**
** Created: Sun Oct 21 22:51:36 2012
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qvariant.h>
#include <qmainwindow.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class QGroupBox;
class QLabel;
class QComboBox;
class QPushButton;
class QLineEdit;

class QSampleMainWnd : public QMainWindow
{
    Q_OBJECT

public:
    QSampleMainWnd( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~QSampleMainWnd();

    QGroupBox* GroupBox2;
    QLabel* TextLabel1;
    QLabel* TextLabel2;
    QLabel* m_handleLabel;
    QComboBox* m_bspCombo;
    QPushButton* m_okButton;
    QGroupBox* GroupBox3;
    QLabel* TextLabel4;
    QLineEdit* m_userID;
    QPushButton* m_enrollButton;
    QPushButton* m_verifyButton;

public slots:
    virtual void slotVerifyButtonPressed();
    virtual void slotEnrollButtonPressed();
    virtual void slotSelectedBSPChanged( int );

protected:

protected slots:
    virtual void languageChange();

};

#endif // MAINWINDOW_H
