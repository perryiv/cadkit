
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODFLOW_DISCRETIZATION_INPUT_FORM_H__
#define __MODFLOW_DISCRETIZATION_INPUT_FORM_H__

#include "ModflowDelegate/FileInputForm.h"
#include "ModflowDelegate/ui_DiscretizationInputForm.h"

#include <map>
#include <memory>
#include <string>

class OGRSpatialReference;

namespace Modflow {

class DiscretizationInputForm : public Modflow::FileInputForm,
                                private Ui::DiscretizationInputForm
{
  Q_OBJECT;
  
public:
  typedef Modflow::FileInputForm BaseClass;
  
  DiscretizationInputForm ( QWidget *parent = 0x0 );
  virtual ~DiscretizationInputForm();
  
  // Build xml string.
  virtual std::string buildXml() const;

  // Does this form only get added once?
  virtual bool        addOnce() const;

protected slots:
  void _unitsComboBoxChanged ( int );
  void on_browseButton_clicked();
  void on_coordinateSystem_clicked();

private:
  typedef std::map<std::string, double> Conversions;
  typedef std::auto_ptr<OGRSpatialReference> CoordinateSystemPtr;
  
  Conversions _conversions;
  CoordinateSystemPtr _coordinateSystem;
};

}


#endif // __MODFLOW_DISCRETIZATION_INPUT_FORM_H__
