
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

#ifndef __VAPORINTRUSION_SETTINGS_DIALOG_H__
#define __VAPORINTRUSION_SETTINGS_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_SettingsDialog.h"

#include "QtGui/QDialog"


class SettingsDialog : public QDialog,
                             private Ui::SettingsDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;

  // Useful typedefs
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;
  typedef IVPI::ColorVec ColorVec;
  typedef IVPI::FloatVec FloatVec;

  SettingsDialog ( QWidget *parent = 0x0 );
  virtual ~SettingsDialog();

  void                initialize();
  void                finalize();

  ColorVec            colors();
  void                colors( ColorVec cv );

  FloatVec            transparencies();
  void                transparencies( FloatVec fv );


protected:
  QColor              _convertColor ( Usul::Math::Vec4f c );
  Usul::Math::Vec4f   _convertColor ( QColor c );

  void                _setValues( float value, QLineEdit* edit, QSlider* slider );

private:
  ColorVec _colors;
  FloatVec _transparencies;

private slots:
  void  on_bSlider_sliderMoved( int value );
  void  on_fSlider_sliderMoved( int value );
  void  on_cSlider_sliderMoved( int value );
  void  on_gSlider_sliderMoved( int value );
  void  on_pSlider_sliderMoved( int value );
	void  on_soilSlider_sliderMoved( int value );
	void  on_sourceSlider_sliderMoved( int value );


  void on_bTrans_editingFinished();
  void on_fTrans_editingFinished();
  void on_cTrans_editingFinished();
  void on_gTrans_editingFinished();
  void on_pTrans_editingFinished();

};



#endif // __VAPORINTRUSION_SETTINGS_DIALOG_H__
