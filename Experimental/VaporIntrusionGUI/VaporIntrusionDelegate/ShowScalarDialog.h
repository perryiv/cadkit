
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

#ifndef __VAPORINTRUSION_SHOW_SCALAR_DIALOG_H__
#define __VAPORINTRUSION_SHOW_SCALAR_DIALOG_H__

#include "Usul/Documents/Manager.h"
#include "Usul/Interfaces/IVaporIntrusionGUI.h"

#include "ui_ShowScalarDialog.h"

#include "QtGui/QDialog"


class ShowScalarDialog : public QDialog,
                             private Ui::ShowScalarDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;

  // Useful typedefs
  

  ShowScalarDialog ( QWidget *parent = 0x0 );
  virtual ~ShowScalarDialog();


protected:
  

private:

private slots:

};



#endif // __VAPORINTRUSION_SHOW_SCALAR_DIALOG_H__
