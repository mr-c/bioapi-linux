/****************************************************************************
** Form interface generated from reading ui file 'MdsModifyMultiUintDialog.ui'
**
** Created: Mon Jun 13 00:47:05 2005
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.4   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MDSMODIFYMULTIUINTDIALOG_H
#define MDSMODIFYMULTIUINTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLineEdit;
class QPushButton;
class QLabel;

class MdsModifyMultiUintDialog : public QDialog
{
    Q_OBJECT

public:
    MdsModifyMultiUintDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~MdsModifyMultiUintDialog();

    QLineEdit* m_attrName;
    QPushButton* m_cancelButton;
    QLineEdit* m_attrValue3;
    QLineEdit* m_attrValue4;
    QLineEdit* m_attrValue5;
    QLineEdit* m_attrValue6;
    QLineEdit* m_attrValue7;
    QLineEdit* m_attrValue8;
    QLineEdit* m_attrValue9;
    QLineEdit* m_attrValue1;
    QLineEdit* m_attrValue2;
    QLineEdit* m_attrValue0;
    QPushButton* m_okButton;
    QLabel* TextLabel2;
    QLabel* TextLabel1;
    QLabel* TextLabel3;
    QLabel* TextLabel3_2;
    QLabel* TextLabel3_3;
    QLabel* TextLabel3_4;
    QLabel* TextLabel3_5;
    QLabel* TextLabel3_6;
    QLabel* TextLabel3_7;
    QLabel* TextLabel3_8;
    QLabel* TextLabel3_9;
    QLabel* TextLabel3_10;

protected:

protected slots:
    virtual void languageChange();

};

#endif // MDSMODIFYMULTIUINTDIALOG_H
