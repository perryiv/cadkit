
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODFLOW_PUMPING_FORM_H__
#define __MODFLOW_PUMPING_FORM_H__

#include "ModflowDelegate/CylinderForm.h"


namespace Modflow {
  
class PumpingForm : public Modflow::CylinderForm
{
public:
  typedef Modflow::CylinderForm BaseClass;
  
  PumpingForm ( QWidget *parent = 0x0 );
  virtual ~PumpingForm();
};
  
}

#endif // __MODFLOW_PUMPING_FORM_H__
