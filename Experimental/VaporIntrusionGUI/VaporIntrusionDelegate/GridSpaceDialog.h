
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

#ifndef __VAPOR_INTRUSION_GRID_SPACE_DIALOG_H__
#define __VAPOR_INTRUSION_GRID_SPACE_DIALOG_H__

#include "ui_GridSpaceDialog.h"

#include "QtGui/QWidget"

#include "osg/Vec4"

#include <string>

class GridSpaceDialog : public QDialog,
                       private Ui::GridSpaceDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  

  GridSpaceDialog ( QWidget *parent = 0x0 );
  virtual ~GridSpaceDialog();

  std::string         name();
  float								x();
  float								y();
  float               depth();

private:
 

  unsigned int _defaultValue;


private slots:
  
};



#endif // __VAPOR_INTRUSION_GRID_SPACE_DIALOG_H__
