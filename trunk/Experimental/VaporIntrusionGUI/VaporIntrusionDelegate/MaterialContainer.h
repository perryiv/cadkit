
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
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

#ifndef __VAPOR_INTRUSION_MATERIAL_CONTAINER_H__
#define __VAPOR_INTRUSION_MATERIAL_CONTAINER_H__

#include "ui_MaterialContainer.h"
#include "MaterialDialog.h"

#include "QtGui/QWidget"

#include "osg/Vec4"

#include <vector>
class MaterialContainer : public QWidget,
                          private Ui::MaterialContainer
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;
  

  MaterialContainer ( QWidget *parent = 0x0 );
  virtual ~MaterialContainer();

  osg::Vec4                     getCurrentColor();

private:
  std::vector< MaterialDialog* > _materials;
  QVBoxLayout*                   _layout;
  QButtonGroup*                  _radioButtons;
  osg::Vec4                      _color;

private slots:

  void on_addButton_clicked();
  void radioClicked( QAbstractButton * button );
  //void on_groupsAddButton_clicked();
  //void on_groupsRemoveButton_clicked();
  
  
};



#endif // __VAPOR_INTRUSION_MATERIAL_CONTAINER_H__
