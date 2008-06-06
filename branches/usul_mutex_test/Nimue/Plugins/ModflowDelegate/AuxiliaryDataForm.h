
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODFLOW_AUXILIARY_DATA_FORM_H__
#define __MODFLOW_AUXILIARY_DATA_FORM_H__

#include "ModflowDelegate/FileInputForm.h"
#include "ModflowDelegate/ui_AuxiliaryDataForm.h"

#include <memory>

class OGRSpatialReference;

namespace Modflow {
  
class AuxiliaryDataForm : public Modflow::FileInputForm,
                          private Ui::AuxiliaryDataForm
{
  Q_OBJECT;
public:
  typedef Modflow::FileInputForm BaseClass;
  
  AuxiliaryDataForm ( QWidget *parent = 0x0 );
  virtual ~AuxiliaryDataForm();
  
  // Build xml string.
  virtual std::string buildXml() const;
  
  // Does this form only get added once?
  virtual bool        addOnce() const;

protected slots:
  void on_browseButton_clicked();
  void on_textureBrowse_clicked();
  void on_coordinateSystem_clicked();
  
private:
  typedef std::auto_ptr<OGRSpatialReference> CoordinateSystemPtr;
  
  CoordinateSystemPtr _coordinateSystem;
};
  
}

#endif // __MODFLOW_AUXILIARY_DATA_FORM_H__
