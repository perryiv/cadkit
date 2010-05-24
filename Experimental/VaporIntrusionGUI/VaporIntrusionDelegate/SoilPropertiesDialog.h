
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

#ifndef __VAPORINTRUSION_SOIL_PROPERTIES_DIALOG_H__
#define __VAPORINTRUSION_SOIL_PROPERTIES_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_SoilPropertiesDialog.h"

#include "QtGui/QDialog"

class SoilPropertiesDialog : public QDialog,
                             private Ui::SoilPropertiesDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;
  typedef IVPI::Soils Soils;
  typedef IVPI::Soil Soil;
  typedef IVPI::SoilLibrary SoilLibrary;

  // Useful typedefs  

  SoilPropertiesDialog ( QWidget *parent = 0x0 );
  virtual ~SoilPropertiesDialog();

  Soil                soil();
  void                soil( Soil s ); 
  SoilLibrary         library();
  void                library( SoilLibrary l );

  void                initialize();

  void                finalize();

protected:

  void                _initialize();
  void                _initSoil();
  void                _updateSoil( unsigned int index );

  void                _updateLibrary();
  void                _updateSoil( Soil s );
  
  void                _clearTable();
private:

  Soil                _soil;
  SoilLibrary         _library;
  unsigned int        _currentSoil;

private slots:
  void on_color_selected ( const QColor & color );
  void on_applyButton_clicked();
};



#endif // __VAPORINTRUSION_SOIL_PROPERTIES_DIALOG_H__
