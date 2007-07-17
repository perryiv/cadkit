
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
//  Node class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _STAR_SYSTEM_BASE_NODE_CLASS_H_
#define _STAR_SYSTEM_BASE_NODE_CLASS_H_

#include "StarSystem/Export.h"
#include "StarSystem/Macros.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IUnknown.h"

#include <map>
#include <string>

namespace StarSystem { class Visitor; }
namespace osg { class Node; }


namespace StarSystem {


class STAR_SYSTEM_EXPORT Node : public Usul::Base::Object
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef std::map<std::string,std::string> BuildOptions;

  // Helper macro for repeated code.
  STAR_SYSTEM_DEFINE_NODE_CLASS ( Node );

  // Build the scene.
  virtual osg::Node *       buildScene ( const BuildOptions &options, Usul::Interfaces::IUnknown *caller ) = 0;

protected:

  // Constructor
  Node();

  // Use reference counting.
  virtual ~Node();

private:

  // No copying or assignment.
  Node ( const Node & );
  Node &operator = ( const Node & );

  void                      _destroy();
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_BASE_NODE_CLASS_H_
