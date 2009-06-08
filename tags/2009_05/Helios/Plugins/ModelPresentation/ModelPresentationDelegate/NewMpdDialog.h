
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

#ifndef __MODELPRESENTATION_NEW_MPD_DIALOG_H__
#define __MODELPRESENTATION_NEW_MPD_DIALOG_H__

#include "ui_NewMpdDialog.h"
#include "MpdDialogDefinitions.h"
#include "Helios/Plugins/ModelPresentation/ModelPresentationLib/ModelPresentationLib.h"

#include "QtGui/QDialog"


class NewMpdDialog : public QDialog,
                     private Ui::NewMpdDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  

  NewMpdDialog ( QWidget *parent = 0x0 );
  virtual ~NewMpdDialog();

  MpdDialogDefinitions::SetList           getSets();
  MpdDialogDefinitions::ModelList         getModels();

private:
  MpdDialogDefinitions::ModelList         _models;
  MpdDialogDefinitions::SetList           _sets;
  
  unsigned int                            _currentSetNumber;


private slots:

  void on_modelsAddButton_clicked();
  void on_modelsRemoveButton_clicked();

  void on_setsAddButton_clicked();
  void on_setsRemoveButton_clicked();



  
  
};



#endif // __MODELPRESENTATION_NEW_MPD_DIALOG_H__
