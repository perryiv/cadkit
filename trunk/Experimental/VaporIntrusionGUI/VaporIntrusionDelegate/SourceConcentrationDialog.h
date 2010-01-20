
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

#ifndef __VAPORINTRUSION_SOURCE_CONCENTRATION_DIALOG_H__
#define __VAPORINTRUSION_SOURCE_CONCENTRATION_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_SourceConcentrationDialog.h"

#include "QtGui/QDialog"

class SourceConcentrationDialog : public QDialog,
                             private Ui::SourceConcentrationDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;
  typedef IVPI::Source Source;
  typedef IVPI::Sources Sources;
  typedef IVPI::Chemical Chemical;
  typedef IVPI::Chemicals Chemicals;

  // Useful typedefs  

  SourceConcentrationDialog ( QWidget *parent = 0x0 );
  virtual ~SourceConcentrationDialog();

  Sources             sources();
  void                sources( Sources s );  

  void                initialize();

  void                finalize();

protected:

  void                _initialize();
  void                _initContaminants();
  void                _initSources();
  void                _updateSource( unsigned int index );
  
  void                _clearTable();
private:

  Sources             _sources;
  unsigned int        _currentSource;

private slots:
  void on_sourceDropDown_activated( int index );
  void on_color_selected ( const QColor & color );
  void on_sourceName_editingFinished();
  void on_applyButton_clicked();
};



#endif // __VAPORINTRUSION_SOURCE_CONCENTRATION_DIALOG_H__
