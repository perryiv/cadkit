
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

#ifndef __VAPORINTRUSION_GRID_EDITOR_DIALOG_H__
#define __VAPORINTRUSION_GRID_EDITOR_DIALOG_H__

#include "Usul/Documents/Manager.h"
#include "Usul/Interfaces/IVaporIntrusionGUI.h"

#include "ui_GridEditorDialog.h"

#include "QtGui/QDialog"


class GridEditorDialog : public QDialog,
                       private Ui::GridEditorDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef Usul::Interfaces::IVaporIntrusionGUI::GridPoints GridPoint;
  

  GridEditorDialog ( QWidget *parent = 0x0 );
  virtual ~GridEditorDialog();


protected:
  void          _initializeGridData();

private:

  GridPoint     _xValues;
  GridPoint     _yValues;
  GridPoint     _zValues;

private slots:
  void on_resetButton_clicked();
  void on_applyButton_clicked();
  
};



#endif // __VAPORINTRUSION_GRID_EDITOR_DIALOG_H__
