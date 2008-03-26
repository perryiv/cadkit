
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WMS_OPTIONS_DIALOG_H__
#define __WMS_OPTIONS_DIALOG_H__

#include "Minerva/Plugins/WmsLayerQt/CompileGuard.h"
#include "ui_OptionsDialog.h"

#include "QtGui/QDialog"

#include <map>
#include <string>

class OptionsDialog : public QDialog,
private Ui::OptionsDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef std::map<std::string, std::string> Options;
  
  OptionsDialog ( const Options& options, QWidget *parent = 0x0 );
  virtual ~OptionsDialog();
  
private:
  Options _options;
};

#endif // __WMS_OPTIONS_DIALOG_H__
