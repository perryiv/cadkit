
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Dragger callback class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_DRAGGER_CALLBACK_CLASS_H_
#define _OSG_TOOLS_DRAGGER_CALLBACK_CLASS_H_

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"

#include <osg/Referenced>
#include <osg/ref_ptr>


namespace OsgTools {
namespace Draggers {

class Dragger;

class OSG_TOOLS_EXPORT Callback : public osg::Referenced
{
public:

  // Useful typedefs
  typedef osg::Referenced BaseClass;
  typedef osg::ref_ptr<Callback> Ptr;

  // Constructors.
  Callback();
  Callback ( const Callback & );

  // Execute the callback.
  virtual void operator() ( Dragger &d ) = 0;

protected:

  // Use reference counting.
  virtual ~Callback();
};


}; // namespace Draggers
}; // namespace OsgTools


#endif // _OSG_TOOLS_DRAGGER_CALLBACK_CLASS_H_
