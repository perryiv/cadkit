
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

#ifndef __MODELPRESENTATION_NEW_SET_DIALOG_H__
#define __MODELPRESENTATION_NEW_SET_DIALOG_H__

#include "ui_NewSetDialog.h"
#include "MpdDialogDefinitions.h"

#include "QtGui/QDialog"


class NewSetDialog : public QDialog,
                     private Ui::NewSetDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  

  NewSetDialog ( QWidget *parent = 0x0 );
  virtual ~NewSetDialog();

  void                                    setModels( MpdDialogDefinitions::ModelList models );
  MpdDialogDefinitions::Set               getSet();
  void                                    setName( const std::string &name );
  void                                    setMenuName( const std::string &menuName );

private:

  MpdDialogDefinitions::ModelList         _models;
  MpdDialogDefinitions::Set               _set;
  unsigned int                            _currentGroupNumber;

private slots:

  void on_groupsAddButton_clicked();
  void on_groupsRemoveButton_clicked();
  
};



#endif // __MODELPRESENTATION_NEW_SET_DIALOG_H__
