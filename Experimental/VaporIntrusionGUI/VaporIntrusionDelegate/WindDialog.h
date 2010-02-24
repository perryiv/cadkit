
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

#ifndef __VAPORINTRUSION_WIND_DIALOG_H__
#define __VAPORINTRUSION_WIND_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_WindDialog.h"

#include "QtGui/QDialog"


class WindDialog : public QDialog,
                             private Ui::WindDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;

  // Useful typedefs  

  WindDialog ( QWidget *parent = 0x0 );
  virtual ~WindDialog();

  

  void                initialize();

  std::string         direction();

protected:

  void                _initialize();

private:
  std::string         _wind;

private slots:

};



#endif // __VAPORINTRUSION_WIND_DIALOG_H__
