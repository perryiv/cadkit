
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

#ifndef __MODELPRESENTATION_ADD_MODEL_DIALOG_H__
#define __MODELPRESENTATION_ADD_MODEL_DIALOG_H__

#include "ui_AddModelDialog.h"

#include "QtGui/QDialog"
#include "QtTools/StringsView.h"


class AddModelDialog : public QDialog,
                       private Ui::AddModelDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;

  AddModelDialog ( QWidget *parent = 0x0 );
  virtual ~AddModelDialog();

  QtTools::StringsView::Items     getItems() const;

private slots:

  void on_browseButton_clicked();
  
  
};



#endif // __MODELPRESENTATION_ADD_MODEL_DIALOG_H__
