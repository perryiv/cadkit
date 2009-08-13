
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

#ifndef __VAPORINTRUSION_REFINE_GRID_DIALOG_H__
#define __VAPORINTRUSION_REFINE_GRID_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_RefineGridDialog.h"

#include "QtGui/QDialog"


class RefineGridDialog : public QDialog,
                             private Ui::RefineGridDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;
  typedef IVPI::GridRefinement GridRefinement;
  typedef IVPI::GridRefinements GridRefinements;

  // Useful typedefs  

  RefineGridDialog ( QWidget *parent = 0x0 );
  virtual ~RefineGridDialog();

  void                initialize();

  void                refinements( GridRefinements g );
  GridRefinements     refinements();

protected:

  void                _initialize();
  void                _clearTable();
  std::string         _getAxis();

private:
  GridRefinements     _refinements;

private slots:
  void on_addButton_clicked();
  void on_removeButton_clicked();
  void on_updateButton_clicked();

};



#endif // __VAPORINTRUSION_REFINE_GRID_DIALOG_H__
