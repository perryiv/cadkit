
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

#ifndef __VAPOR_INTRUSION_MATERIAL_DIALOG_H__
#define __VAPOR_INTRUSION_MATERIAL_DIALOG_H__

#include "ui_MaterialDialog.h"

#include "QtGui/QWidget"

#include "osg/Vec4"

#include <string>

class MaterialDialog : public QWidget,
                       private Ui::MaterialDialog
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;
  

  MaterialDialog ( QWidget *parent = 0x0 );
  virtual ~MaterialDialog();

  std::string         name();
  std::string         type();
  osg::Vec4           color();

private:
    /*
    QVBoxLayout *vboxLayout;
    QGridLayout *gridLayout;
    QLineEdit *_name;
    QLineEdit *_type;
    QLabel *_nameLabel;
    QLabel *_valueLabel;
    QtTools::ColorButton *color;
    */

private slots:
  
};



#endif // __VAPOR_INTRUSION_MATERIAL_DIALOG_H__
