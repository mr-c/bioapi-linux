/****************************************************************************
** Form interface generated from reading ui file 'mainwindow.ui'
**
** Created: Mon Jun 13 00:09:33 2005
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.4   edited Nov 24 2003 $)
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
