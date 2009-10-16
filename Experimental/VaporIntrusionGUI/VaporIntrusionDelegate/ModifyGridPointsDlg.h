
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

#ifndef __VAPORINTRUSION_MODIFY_GRID_POINTS_DLG_H__
#define __VAPORINTRUSION_MODIFY_GRID_POINTS_DLG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_ModifyGridPointsDlg.h"

#include "QtGui/QDialog"


class ModifyGridPointsDlg : public QDialog,
                             private Ui::ModifyGridPointsDlg
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;
  typedef IVPI::GridAxisPoint GridAxisPoint;
  typedef IVPI::GridAxisPoints GridAxisPoints;

  // Useful typedefs  

  ModifyGridPointsDlg ( QWidget *parent = 0x0 );
  virtual ~ModifyGridPointsDlg();

  void                initialize();

  void                gridAxisPoints( GridAxisPoints ap );
  GridAxisPoints      gridAxisPoints();

protected:

  void                _initialize();
  void                _clearTable();
  std::string         _getAxis();

private:
  GridAxisPoints      _axisPoints;

private slots:
  void on_addButton_clicked();
  void on_removeButton_clicked();
  void on_updateButton_clicked();

};



#endif // __VAPORINTRUSION_MODIFY_GRID_POINTS_DLG_H__
