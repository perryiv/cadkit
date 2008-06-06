
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODFLOW_HEAD_LEVEL_OUTPUT_FORM_H__
#define __MODFLOW_HEAD_LEVEL_OUTPUT_FORM_H__

#include "ModflowDelegate/FileInputForm.h"
#include "ModflowDelegate/ui_HeadLevelOutputForm.h"

namespace Modflow {
  
class HeadLevelOutputForm : public Modflow::FileInputForm,
                            private Ui::HeadLevelOutputForm
{
  Q_OBJECT;
public:
  typedef Modflow::FileInputForm BaseClass;
  
  HeadLevelOutputForm ( QWidget *parent = 0x0 );
  virtual ~HeadLevelOutputForm();
  
  // Build xml string.
  virtual std::string buildXml() const;
  
  // Does this form only get added once?
  virtual bool        addOnce() const;
protected slots:
  void on_browseButton_clicked();
  
};

}

#endif // __MODFLOW_HEAD_LEVEL_OUTPUT_FORM_H__
