
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

#ifndef _MINERVA_CORE_TILE_ENGINE_BASE_NODE_CLASS_H_
#define _MINERVA_CORE_TILE_ENGINE_BASE_NODE_CLASS_H_

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Extents.h"
#include "Minerva/Core/Macros.h"

#include "Serialize/XML/Macros.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IUnknown.h"

#include <map>
#include <string>

namespace Minerva { namespace Core { class Visitor; } }
namespace osg { class Node; }


namespace Minerva {
namespace Core {
namespace TileEngine {


class MINERVA_EXPORT Node : public Usul::Base::Object
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef std::map<std::string,std::string> BuildOptions;

  // Helper macro for repeated code.
  MINERVA_DEFINE_NODE_CLASS ( Node );

  // Pre- and post-render notifications.
  virtual void              preRender  ( Usul::Interfaces::IUnknown *caller );
  virtual void              postRender ( Usul::Interfaces::IUnknown *caller );

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

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( Node );
};


} // namespace TileEngine
} // namespace Core
} // namespace Minerva


#endif // _MINERVA_CORE_TILE_ENGINE_BASE_NODE_CLASS_H_
