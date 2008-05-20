
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

#include "QtGui/QDialog"


class NewGroupDialog : public QDialog,
                       private Ui::NewGroupDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;

  NewGroupDialog ( QWidget *parent = 0x0 );
  virtual ~NewGroupDialog();

//private slots:
//
//  void on_groupsAddButton_clicked();
//  void on_groupsRemoveButton_clicked();
  
  
};



#endif // __MODELPRESENTATION_NEW_GROUP_DIALOG_H__
