
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODFLOW_CYLINDER_FORM_H__
#define __MODFLOW_CYLINDER_FORM_H__

#include "ModflowDelegate/FileInputForm.h"
#include "ModflowDelegate/ui_CylinderForm.h"

namespace Modflow {
  
class CylinderForm : public Modflow::FileInputForm,
                     private Ui::CylinderForm
{
  Q_OBJECT;
public:
  typedef Modflow::FileInputForm BaseClass;
  
  CylinderForm ( const std::string& type, const std::string& section, const std::string& filters, QWidget *parent = 0x0 );
  virtual ~CylinderForm();
  
  // Build xml string.
  virtual std::string buildXml() const;
  
  // Does this form only get added once?
  virtual bool        addOnce() const;

protected slots:
  void on_browseButton_clicked();
  
private:
  std::string _type;
  std::string _section;
  std::string _filters;
  
};
  
}

#endif // __MODFLOW_CYLINDER_FORM_H__
