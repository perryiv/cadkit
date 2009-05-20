
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
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

#ifndef __VAPORINTRUSION_SCALAR_EDITOR_DIALOG_H__
#define __VAPORINTRUSION_SCALAR_EDITOR_DIALOG_H__

#include "Usul/Documents/Manager.h"
#include "Usul/Interfaces/IVaporIntrusionGUI.h"

#include "ui_ScalarEditorDialog.h"

#include "QtGui/QDialog"


class ScalarEditorDialog : public QDialog,
                           private Ui::ScalarEditorDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef Usul::Interfaces::IVaporIntrusionGUI::GridPoints GridPoint;
  

  ScalarEditorDialog ( QWidget *parent = 0x0 );
  virtual ~ScalarEditorDialog();


protected:

private:

private slots:
  void on_addButton_clicked();
  void on_removeButton_clicked();
  void on_showButton_clicked();
  
};



#endif // __VAPORINTRUSION_SCALAR_EDITOR_DIALOG_H__
