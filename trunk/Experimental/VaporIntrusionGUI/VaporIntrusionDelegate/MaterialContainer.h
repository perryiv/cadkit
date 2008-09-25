
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

#include "Usul/Convert/Convert.h"
#include "Usul/Properties/Attribute.h"

#include "QtGui/QWidget"
#include "QtGui/QRadioButton"

#include "osg/Vec4"

#include <vector>
#include <map>
class MaterialContainer : public QWidget,
                          private Ui::MaterialContainer
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;
  typedef Usul::Pointers::Configs::NoRefCountingNullOk Config;
  typedef Usul::Properties::Attribute< MaterialDialog, QObjectUserData, Config > UserData;
  typedef std::map< QRadioButton*, MaterialDialog* > Materials;
  

  MaterialContainer ( QWidget *parent = 0x0 );
  virtual ~MaterialContainer();

  osg::Vec4                     getCurrentColor();
  std::string                   getCurrentName();
  std::string                   getCurrentValue();

private:
  Materials                      _materials;
  QVBoxLayout*                   _layout;
  QButtonGroup*                  _radioButtons;
  osg::Vec4                      _color;
  std::string                    _name;
  std::string                    _value;

private slots:

  void on_addButton_clicked();
  void radioClicked( QAbstractButton * button );
  //void on_groupsAddButton_clicked();
  //void on_groupsRemoveButton_clicked();
  
  
};



#endif // __VAPOR_INTRUSION_MATERIAL_CONTAINER_H__
