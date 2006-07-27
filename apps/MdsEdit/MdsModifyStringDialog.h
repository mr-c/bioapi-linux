/****************************************************************************
** Form interface generated from reading ui file 'MdsModifyStringDialog.ui'
**
** Created: Mon Jun 13 00:47:05 2005
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.4   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MDSMODIFYSTRINGDIALOG_H
#define MDSMODIFYSTRINGDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QLineEdit;
class QCheckBox;
class QPushButton;

class MdsModifyStringDialog : public QDialog
{
    Q_OBJECT

public:
    MdsModifyStringDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~MdsModifyStringDialog();

    QLabel* TextLabel1;
    QLineEdit* m_attrName;
    QLabel* TextLabel2;
    QLineEdit* m_attrValue;
    QCheckBox* m_isNull;
    QPushButton* m_cancelButton;
    QPushButton* m_okButton;

protected:

protected slots:
    virtual void languageChange();

};

#endif // MDSMODIFYSTRINGDIALOG_H
