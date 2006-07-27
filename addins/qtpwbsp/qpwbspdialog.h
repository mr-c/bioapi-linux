/****************************************************************************
** Form interface generated from reading ui file 'qpwbspdialog.ui'
**
** Created: Thu Oct 31 10:22:41 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef QPWBSPDIALOG_H
#define QPWBSPDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QLineEdit;
class QPushButton;

class QpwbspDialog : public QDialog
{ 
    Q_OBJECT

public:
    QpwbspDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~QpwbspDialog();

    QLabel* TextLabel1;
    QPushButton* m_okButton;
    QLineEdit* m_password;


};

#endif // QPWBSPDIALOG_H
