
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

#ifndef __VAPORINTRUSION_CHEMICAL_DIALOG_H__
#define __VAPORINTRUSION_CHEMICAL_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_ChemicalDialog.h"

#include "QtGui/QDialog"


class ChemicalDialog : public QDialog,
                       private Ui::ChemicalsDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;
  typedef IVPI::Chemical  Chemical;
  typedef IVPI::Chemicals Chemicals;

  // Useful typedefs  

  ChemicalDialog ( QWidget *parent = 0x0 );
  virtual ~ChemicalDialog();

  void                initialize();
  Chemicals           chemicals();
  void                chemicals( Chemicals c );
  void                library( Chemicals l );
  Chemicals           library();
  Chemicals           getSelectedChemicals();


protected:

  void                _initialize();
  void                _clearTable();

private:
  Chemicals        _chemicals;
  Chemicals        _chemicalLibrary;

private slots:
  void on_addButton_clicked();
  void on_removeButton_clicked();

};



#endif // __VAPORINTRUSION_CHEMICAL_DIALOG_H__
