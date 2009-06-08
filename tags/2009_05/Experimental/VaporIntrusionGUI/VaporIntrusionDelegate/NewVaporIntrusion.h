
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Input dialog for files.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VAPOR_INTRUSION_NEW_DIALOG_H__
#define __VAPOR_INTRUSION_NEW_DIALOG_H__

#include "ui_NewVaporIntrusion.h"

#include "QtGui/QWidget"

#include "osg/Vec4"

#include <string>

class NewVaporIntrusion : public QDialog,
                       private Ui::NewVaporIntrusion
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  

  NewVaporIntrusion ( QWidget *parent = 0x0 );
  virtual ~NewVaporIntrusion();

  std::string         name();
  unsigned int        x();
  unsigned int        y();
  unsigned int        z();

private:
    /*
    QDialogButtonBox *okCancelButtons;
    QGroupBox *groupBox;
    QLineEdit *xValue;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *yValue;
    QLabel *label_3;
    QLineEdit *zValue;
    QLabel *label_4;
    QLineEdit *name;
    */


private slots:
  
};



#endif // __VAPOR_INTRUSION_NEW_DIALOG_H__
