
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Visitor class for scaling the scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _STAR_SYSTEM_SCALE_SCENE_VISITOR_CLASS_H_
#define _STAR_SYSTEM_SCALE_SCENE_VISITOR_CLASS_H_

#include "StarSystem/Visitor.h"


namespace StarSystem {


class STAR_SYSTEM_EXPORT Scale : public Visitor
{
public:

  // Useful typedefs.
  typedef Visitor BaseClass;

  // Reference pointers.
  USUL_DECLARE_REF_POINTERS ( Scale );

  // Constructor
  Scale ( double scale );

protected:

  // Visit the body.
  virtual void visit ( Body & );

  // Use reference counting.
  virtual ~Scale();

private:

  // No copying or assignment.
  Scale ( const Scale & );
  Scale &operator = ( const Scale & );

  double _scale;
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_SCALE_SCENE_VISITOR_CLASS_H_
