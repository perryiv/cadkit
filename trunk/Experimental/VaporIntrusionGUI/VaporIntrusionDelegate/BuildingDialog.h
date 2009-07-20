
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

#ifndef __VAPORINTRUSION_BUILDING_DIALOG_H__
#define __VAPORINTRUSION_BUILDING_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_BuildingDialog.h"

#include "QtGui/QDialog"


class BuildingDialog : public QDialog,
                             private Ui::BuildingDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;

  // Useful typedefs
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;
  typedef IVPI::Building Building;
  typedef IVPI::Crack Crack;
  typedef IVPI::Cracks Cracks;

  BuildingDialog ( QWidget *parent = 0x0 );
  virtual ~BuildingDialog();

  Building building();
  void     building( Building b );
  bool     useBuilding();

  void     initialize();


protected:
  void        _initialize();
  void        _clearTable();

private:
  Building    _building;

private slots:
  void on_addButton_clicked();
  void on_removeButton_clicked();

};



#endif // __VAPORINTRUSION_BUILDING_DIALOG_H__
