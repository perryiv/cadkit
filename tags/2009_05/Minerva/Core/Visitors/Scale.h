
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

#ifndef _MINERVA_CORE_SCALE_SCENE_VISITOR_CLASS_H_
#define _MINERVA_CORE_SCALE_SCENE_VISITOR_CLASS_H_

#include "Minerva/Core//Visitor.h"


namespace Minerva {
namespace Core {
namespace Visitors {


class MINERVA_EXPORT Scale : public Minerva::Core::Visitor
{
public:

  // Useful typedefs.
  typedef Minerva::Core::Visitor BaseClass;

  // Reference pointers.
  USUL_DECLARE_REF_POINTERS ( Scale );

  // Constructor
  Scale ( double scale );

protected:

  // Visit the body.
  virtual void visit ( Minerva::Core::TileEngine::Body & );

  // Use reference counting.
  virtual ~Scale();

private:

  // No copying or assignment.
  Scale ( const Scale & );
  Scale &operator = ( const Scale & );

  double _scale;
};


} // namespace Visitors
} // namespace Core
} // namespace Minerva


#endif // _MINERVA_CORE_SCALE_SCENE_VISITOR_CLASS_H_
