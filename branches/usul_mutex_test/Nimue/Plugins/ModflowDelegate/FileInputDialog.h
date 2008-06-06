
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Input dialog for files.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODFLOW_FILE_INPUT_DIALOG_H__
#define __MODFLOW_FILE_INPUT_DIALOG_H__

#include "ModflowDelegate/ui_FileInputDialog.h"

#include "QtGui/QDialog"

#include <vector>

class QVBoxLayout;

namespace Modflow {

class FileInputForm;

class FileInputDialog : public QDialog,
                        private Ui::FileInputDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;

  FileInputDialog ( QWidget *parent = 0x0 );
  virtual ~FileInputDialog();
  
  std::string buildXml() const;

protected slots:
  
  // Named to get hooked up automatically.
  void on_addButton_clicked();
  void on_coordinateSystem_clicked();
private:
  typedef std::vector< FileInputForm* > Forms;
  typedef std::pair < bool, std::string > CoordinateSystem;

  Forms _forms;
  QVBoxLayout *_layout;
  QWidget *_widget;
  CoordinateSystem _coordinateSystem;
};


} // namespace Modflow


#endif // __MODFLOW_FILE_INPUT_DIALOG_H__
