
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

#ifndef __VAPORINTRUSION_SOURCES_PROPERTIES_DIALOG_H__
#define __VAPORINTRUSION_SOURCES_PROPERTIES_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_SourcePropertiesDialog.h"

#include "QtGui/QDialog"


class SourcePropertiesDialog : public QDialog,
                             private Ui::SourcePropertiesDialog
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

  SourcePropertiesDialog ( QWidget *parent = 0x0 );
  virtual ~SourcePropertiesDialog();

  Source              source();
  void                source( Source s );  
  

  void                initialize();

protected:

  void                _initialize();
  void                _clearTable();

private:
  Source             _source;

private slots:

};



#endif // __VAPORINTRUSION_SOURCES_PROPERTIES_DIALOG_H__
