
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

#include "Minerva/Qt/Widgets/Export.h"

#include "ui_OptionsDialog.h"

#include "QtGui/QDialog"

#include <map>
#include <string>

namespace Minerva {
namespace Widgets {
      
      
class MINERVA_QT_WIDGETS_EXPORT OptionsDialog : public QDialog,
                                                private Ui::OptionsDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef std::map<std::string, std::string> Options;
  
  OptionsDialog ( const Options& options, QWidget *parent = 0x0 );
  virtual ~OptionsDialog();
  
  Options options() const;
  
protected slots:
  
  void on_addRowButton_clicked();
  void on_removeRowButton_clicked();
};


}
}

#endif // __WMS_OPTIONS_DIALOG_H__
