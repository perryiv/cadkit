
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

#ifndef __VAPORINTRUSION_CONTAMINANT_DIALOG_H__
#define __VAPORINTRUSION_CONTAMINANT_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_ContaminantDialog.h"

#include "QtGui/QDialog"


class ContaminantDialog : public QDialog,
                             private Ui::ContaminantDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;
  typedef IVPI::Chemical  Chemical;
  typedef IVPI::Chemicals Chemicals;

  // Useful typedefs  

  ContaminantDialog ( QWidget *parent = 0x0 );
  virtual ~ContaminantDialog();

  void                initialize();
  Chemicals        contaminants();
  void                contaminants( Chemicals c );

protected:

  void                _initialize();
  void                _clearTable();

private:
  Chemicals        _contaminants;

private slots:
  void on_addButton_clicked();
  void on_removeButton_clicked();
  void on_updateButton_clicked();

};



#endif // __VAPORINTRUSION_CONTAMINANT_DIALOG_H__
