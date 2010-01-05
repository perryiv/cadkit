
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

#ifndef __VAPORINTRUSION_ADD_CHEMICAL_DIALOG_H__
#define __VAPORINTRUSION_ADD_CHEMICAL_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_AddChemicalDialog.h"

#include "QtGui/QDialog"


class AddChemicalDialog : public QDialog,
                             private Ui::AddChemicalDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;
  typedef IVPI::Chemical  Chemical;
  typedef IVPI::Chemicals Chemicals;

  // Useful typedefs  

  AddChemicalDialog ( QWidget *parent = 0x0 );
  virtual ~AddChemicalDialog();

  void                initialize();
  Chemical            chemical();
  void                chemical( Chemical c );
  Chemical                createChemical();

protected:

  void                _initialize();
  void                _clearTable();

private:
  Chemical           _chemical;

private slots:

};



#endif // __VAPORINTRUSION_ADD_CHEMICAL_DIALOG_H__
