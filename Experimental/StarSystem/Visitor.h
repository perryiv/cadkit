
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
//  Base visitor class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _STAR_SYSTEM_BASE_VISITOR_CLASS_H_
#define _STAR_SYSTEM_BASE_VISITOR_CLASS_H_

#include "StarSystem/Export.h"
#include "StarSystem/Macros.h"

#include "Usul/Base/Object.h"

namespace StarSystem { class Node; class Body; class Group; class System; }


namespace StarSystem {


class STAR_SYSTEM_EXPORT Visitor : public Usul::Base::Object
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;

  // Helper macro for repeated code.
  STAR_SYSTEM_DEFINE_VISITOR_CLASS ( Visitor );

protected:

  // Constructor
  Visitor();

  // Use reference counting.
  virtual ~Visitor();

private:

  // No copying or assignment.
  Visitor ( const Visitor & );
  Visitor &operator = ( const Visitor & );

  void                      _destroy();
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_BASE_CLASS_H_
