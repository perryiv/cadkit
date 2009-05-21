
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

#ifndef __VAPORINTRUSION_SCALAR_EDITOR_DIALOG_H__
#define __VAPORINTRUSION_SCALAR_EDITOR_DIALOG_H__

#include "Usul/Documents/Manager.h"
#include "Usul/Interfaces/IVaporIntrusionGUI.h"
#include "Usul/Math/Vector3.h"

#include "ui_ScalarEditorDialog.h"

#include "QtGui/QDialog"


class ScalarEditorDialog : public QDialog,
                           private Ui::ScalarEditorDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef Usul::Interfaces::IVaporIntrusionGUI::GridPoints GridPoint;
  typedef Usul::Interfaces::IVaporIntrusionGUI::GridMaterials GridMaterials;
  typedef Usul::Interfaces::IVaporIntrusionGUI::MaterialsMap MaterialsMap;
  typedef std::pair< Usul::Math::Vec3ui, Usul::Math::Vec3ui > MinMaxPair;
  

  ScalarEditorDialog ( Usul::Math::Vec3ui dimensions, QWidget *parent = 0x0 );
  virtual ~ScalarEditorDialog();

  MaterialsMap        materials();
  void                materials( MaterialsMap m );

  void                onOkClicked();


protected:

   MinMaxPair         _getMinMax();

private:
  MaterialsMap        _materials;
  Usul::Math::Vec3ui  _dimensions;
  GridMaterials       _gridMaterials;

private slots:
  void on_addButton_clicked();
  void on_removeButton_clicked();
  void on_showButton_clicked();
  
};



#endif // __VAPORINTRUSION_SCALAR_EDITOR_DIALOG_H__
