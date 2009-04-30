
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

#ifndef __VAPORINTRUSION_INPUT_PARAMETER_DIALOG_H__
#define __VAPORINTRUSION_INPUT_PARAMETER_DIALOG_H__

#include "Usul/Documents/Manager.h"
#include "Usul/Interfaces/IVaporIntrusionGUI.h"

#include "ui_InputParameterDialog.h"

#include "QtGui/QDialog"


class InputParameterDialog : public QDialog,
                             private Ui::InputParameterDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  

  InputParameterDialog ( Usul::Interfaces::IVaporIntrusionGUI::Category category, QWidget *parent = 0x0 );
  virtual ~InputParameterDialog();


protected:
  
  void            _initializeList();

private:
  Usul::Interfaces::IVaporIntrusionGUI::Category _category;

private slots:

  void on_resetButton_clicked();
  void on_applyButton_clicked();
  
};



#endif // __VAPORINTRUSION_INPUT_PARAMETER_DIALOG_H__
