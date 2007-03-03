
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DRAW_CALLBACK_H__
#define __DRAW_CALLBACK_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"


namespace Minerva {
  namespace Core {

class DrawCallback : public Usul::Base::Referenced
{
public:
  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( DrawCallback );

  DrawCallback ( ) {}

  /// Draw.
  virtual void draw() = 0;

protected:
  virtual ~DrawCallback() {}

};

  }
}

#endif // __DRAW_CALLBACK_H__

