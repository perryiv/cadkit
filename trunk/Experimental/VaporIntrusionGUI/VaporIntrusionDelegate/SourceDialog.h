
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

#ifndef __VAPORINTRUSION_SOURCES_DIALOG_H__
#define __VAPORINTRUSION_SOURCES_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_SourceDialog.h"

#include "QtGui/QDialog"


class SourceDialog : public QDialog,
                             private Ui::SourceDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;
  typedef IVPI::Source Source;
  typedef IVPI::Sources Sources;
  typedef IVPI::Contaminant Contaminant;
  typedef IVPI::Contaminants Contaminants;

  // Useful typedefs  

  SourceDialog ( QWidget *parent = 0x0 );
  virtual ~SourceDialog();

  Sources        sources();
  void                sources( Sources c );  

  void                initialize();

protected:

  void                _initialize();
  void                _clearTable();

private:
  Sources _sources;

private slots:
  void on_addButton_clicked();
  void on_removeButton_clicked();

};



#endif // __VAPORINTRUSION_SOURCES_DIALOG_H__
