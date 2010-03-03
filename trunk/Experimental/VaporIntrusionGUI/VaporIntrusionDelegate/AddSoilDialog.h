
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

#ifndef __VAPORINTRUSION_ADD_SOIL_DIALOG_H__
#define __VAPORINTRUSION_ADD_SOIL_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_AddSoilDialog.h"

#include "QtGui/QDialog"


class AddSoilDialog : public QDialog,
                             private Ui::AddSoilDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;
  typedef IVPI::Soil  Soil;
  typedef IVPI::Soils Soils;

  // Useful typedefs  

  AddSoilDialog ( QWidget *parent = 0x0 );
  virtual ~AddSoilDialog();

  void                initialize();
  Soil                soil();
  void                soil( Soil s );
  Soil                createSoil();

protected:

  void                _initialize();
  void                _clearTable();

private:
  Soil                _soil;

private slots:

};



#endif // __VAPORINTRUSION_ADD_SOIL_DIALOG_H__
