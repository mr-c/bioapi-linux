/****************************************************************************
** Form interface generated from reading ui file 'MdsFindDialog.ui'
**
** Created: Mon Jun 13 00:47:05 2005
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.4   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MDSFINDDIALOG_H
#define MDSFINDDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QGroupBox;
class QCheckBox;
class QLineEdit;
class QLabel;

class MdsFindDialog : public QDialog
{
    Q_OBJECT

public:
    MdsFindDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~MdsFindDialog();

    QPushButton* m_cancelButton;
    QPushButton* m_okButton;
    QGroupBox* m_scopeFlags;
    QCheckBox* m_useAttrName;
    QCheckBox* m_useAttrValue;
    QCheckBox* m_matchCase;
    QLineEdit* m_searchString;
    QLabel* TextLabel1;
    QCheckBox* m_matchWholeString;

protected:

protected slots:
    virtual void languageChange();

};

#endif // MDSFINDDIALOG_H
