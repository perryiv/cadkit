
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

#ifndef __VAPORINTRUSION_SEND_DIALOG_H__
#define __VAPORINTRUSION_SEND_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_SendDialog.h"

#include "QtGui/QDialog"


class SendDialog : public QDialog,
                             private Ui::SendDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;

  // Useful typedefs  

  SendDialog ( QWidget *parent = 0x0 );
  virtual ~SendDialog();

  void                initialize();

protected:

  void                _initialize();

private:

private slots:
  void on_sendButton_clicked();

};



#endif // __VAPORINTRUSION_SEND_DIALOG_H__
