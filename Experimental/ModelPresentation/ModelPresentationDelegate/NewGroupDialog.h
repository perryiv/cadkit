
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

#ifndef __MODELPRESENTATION_NEW_GROUP_DIALOG_H__
#define __MODELPRESENTATION_NEW_GROUP_DIALOG_H__

#include "ui_NewGroupDialog.h"
#include "MpdDialogDefinitions.h"

#include "QtGui/QDialog"


class NewGroupDialog : public QDialog,
                       private Ui::NewGroupDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  

  NewGroupDialog ( QWidget *parent = 0x0 );
  virtual ~NewGroupDialog();

  void                              setModels( MpdDialogDefinitions::ModelList models );
  void                              setName( const std::string &name );
  std::string                       getName();
  QTreeWidget*                      getGroupModels();

  MpdDialogDefinitions::Group       getGroup();


private:

  MpdDialogDefinitions::ModelList   _models;
  
                         
  

private slots:

  void on_modelsAddButton_clicked();
  void on_groupsRemoveButton_clicked();
  
  
};



#endif // __MODELPRESENTATION_NEW_GROUP_DIALOG_H__
