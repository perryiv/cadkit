
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

#ifndef __VAPORINTRUSION_SOIL_DIALOG_H__
#define __VAPORINTRUSION_SOIL_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_SoilDialog.h"

#include "QtGui/QDialog"


class SoilDialog : public QDialog,
                             private Ui::SoilDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;
  typedef IVPI::Soil  Soil;
  typedef IVPI::Soils Soils;

  // Useful typedefs  

  SoilDialog ( QWidget *parent = 0x0 );
  virtual ~SoilDialog();

  void                initialize();
  Soils               soils();
  void                soils( Soils s );

protected:

  void                _initialize();
  void                _clearTable();

private:
  Soils               _soils;

private slots:
  void on_addButton_clicked();
  void on_removeButton_clicked();

};



#endif // __VAPORINTRUSION_SOIL_DIALOG_H__
