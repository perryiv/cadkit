
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

#ifndef __VAPORINTRUSION_SOIL_LAYER_DIALOG_H__
#define __VAPORINTRUSION_SOIL_LAYER_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_SoilLayerDialog.h"

#include "QtGui/QDialog"

class SoilLayerDialog : public QDialog,
                             private Ui::SoilLayerDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;
  typedef IVPI::Soils Soils;
  typedef IVPI::Soil Soil;
  typedef IVPI::SoilLibrary SoilLibrary;

  // Useful typedefs  

  SoilLayerDialog ( QWidget *parent = 0x0 );
  virtual ~SoilLayerDialog();

  Soils               soils();
  void                soils( Soils s ); 
  SoilLibrary         library();
  void                library( SoilLibrary l );

  void                initialize();

  void                finalize();

protected:

  void                _initialize();
  void                _initSoils();
  void                _updateSoil( unsigned int index );

  void                _updateLibrary();
  void                _updateSoils( Soil s );
  
  void                _clearTable();
private:

  Soils               _soils;
  SoilLibrary         _library;
  unsigned int        _currentSoil;

private slots:
  void on_soilDropDown_activated( int index );
  void on_color_selected ( const QColor & color );
  void on_applyButton_clicked();
  void on_addButton_clicked();
  void on_removeButton_clicked();
};



#endif // __VAPORINTRUSION_SOIL_LAYER_DIALOG_H__
