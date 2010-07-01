
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

#ifndef __VAPORINTRUSION_GLOBAL_PARAMETERS_DIALOG_H__
#define __VAPORINTRUSION_GLOBAL_PARAMETERS_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_GlobalParametersDialog.h"

#include "QtGui/QDialog"


class GlobalParametersDialog : public QDialog,
															 private Ui::GlobalParametersDialog
{
  Q_OBJECT;
public:
  typedef QDialog																							BaseClass;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI		IVPI;
  typedef IVPI::Chemical																			Chemical;
  typedef IVPI::ChemicalPair																	ChemicalPair;

  // Useful typedefs  

  GlobalParametersDialog ( QWidget *parent = 0x0 );
  virtual ~GlobalParametersDialog();

  void                initialize();
  ChemicalPair        chemicals();
  void                chemicals( ChemicalPair c );
	void								finalize();

protected:

  void                _initialize();
  void                _clearTable();

private:
  ChemicalPair        _chemicals;

private slots:

};



#endif // __VAPORINTRUSION_GLOBAL_PARAMETERS_DIALOG_H__
