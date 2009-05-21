
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

#ifndef __VAPORINTRUSION_SHOW_SCALAR_DIALOG_H__
#define __VAPORINTRUSION_SHOW_SCALAR_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_ShowScalarDialog.h"

#include "QtGui/QDialog"


class ShowScalarDialog : public QDialog,
                             private Ui::ShowScalarDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;

  // Useful typedefs
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::GridMaterials GridMaterials;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::MaterialsMap MaterialsMap;
  typedef Usul::Math::Vec3ui Vec3ui;
  

  ShowScalarDialog ( Vec3ui min, Vec3ui max, Vec3ui dim, GridMaterials gm, QWidget *parent = 0x0 );
  virtual ~ShowScalarDialog();

  GridMaterials         gridMaterials();
  void                  gridMaterials( GridMaterials gm );


protected:
  void                  _initialize();

private:
  GridMaterials      _gridMaterials;
  Vec3ui            _min;
  Vec3ui            _max;
  Vec3ui            _dimensions;

private slots:

};



#endif // __VAPORINTRUSION_SHOW_SCALAR_DIALOG_H__
