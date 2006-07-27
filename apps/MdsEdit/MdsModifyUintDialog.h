/****************************************************************************
** Form interface generated from reading ui file 'MdsModifyUintDialog.ui'
**
** Created: Mon Jun 13 00:47:05 2005
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.4   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MDSMODIFYUINTDIALOG_H
#define MDSMODIFYUINTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QLineEdit;
class QPushButton;

class MdsModifyUintDialog : public QDialog
{
    Q_OBJECT

public:
    MdsModifyUintDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~MdsModifyUintDialog();

    QLabel* TextLabel1;
    QLineEdit* m_attrName;
    QLabel* TextLabel2;
    QPushButton* m_cancelButton;
    QLineEdit* m_attrValue;
    QPushButton* m_okButton;
    QLabel* TextLabel3;

protected:

protected slots:
    virtual void languageChange();

};

#endif // MDSMODIFYUINTDIALOG_H
