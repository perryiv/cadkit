
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

#ifndef __VAPOR_INTRUSION_PREV_SAVE_LOAD_H__
#define __VAPOR_INTRUSION_PREV_SAVE_LOAD_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "ui_PrefSaveLoadDialog.h"

#include "QtGui/QWidget"

#include "osg/Vec4"

#include <string>

class PrefSaveLoadDialog : public QDialog,
                       private Ui::PrefSaveLoadDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;
  

  PrefSaveLoadDialog ( QWidget *parent = 0x0 );
  virtual ~PrefSaveLoadDialog();

  


private:

private slots:
  void on_saveButton_clicked();
  void on_loadButton_clicked();
  
};



#endif // __VAPOR_INTRUSION_PREV_SAVE_LOAD_H__
