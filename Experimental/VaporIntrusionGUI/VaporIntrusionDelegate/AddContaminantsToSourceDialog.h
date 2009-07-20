
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

#ifndef __VAPORINTRUSION_ADDCONTAMINANTSTOSOURCE_DIALOG_H__
#define __VAPORINTRUSION_ADDCONTAMINANTSTOSOURCE_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_AddContaminantsToSourceDialog.h"

#include "QtGui/QDialog"

class AddContaminantsToSourceDialog : public QDialog,
                             private Ui::AddContaminantsToSourceDialog
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

  AddContaminantsToSourceDialog ( QWidget *parent = 0x0 );
  virtual ~AddContaminantsToSourceDialog();

  Sources             sources();
  void                sources( Sources s );  

  Contaminants        contaminants();
  void                contaminants( Contaminants c );

  void                initialize();

protected:

  void                _initialize();
  void                _initSource();
  void                _initContaminants();
private:

  Sources             _sources;
  Contaminants        _contaminants;

private slots:
  void on_addButton_clicked();
  void on_removeButton_clicked();

};



#endif // __VAPORINTRUSION_ADDCONTAMINANTSTOSOURCE_DIALOG_H__
