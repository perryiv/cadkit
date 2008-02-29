
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
//  Visitor class for building the scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _STAR_SYSTEM_BUILD_SCENE_VISITOR_CLASS_H_
#define _STAR_SYSTEM_BUILD_SCENE_VISITOR_CLASS_H_

#include "Minerva/Core/Visitor.h"

#include "Usul/Interfaces/IUnknown.h"

#include <map>
#include <string>

namespace osg { class Group; }


namespace Minerva {
namespace Core {
namespace Visitors {


class MINERVA_EXPORT BuildScene : public Minerva::Core::Visitor
{
public:

  // Useful typedefs.
  typedef Visitor BaseClass;
  typedef std::map<std::string,std::string> BuildOptions;
  typedef Usul::Interfaces::IUnknown IUnknown;

  // Helper macro for repeated code.
  MINERVA_DEFINE_VISITOR_CLASS ( BuildScene );

  // Constructor
  BuildScene ( const BuildOptions &options = BuildOptions(), IUnknown *caller = 0x0 );

  // Get the scene.
  osg::Group *              scene();

  virtual void visit ( Minerva::Core::TileEngine::System & );
  virtual void visit ( Minerva::Core::TileEngine::Group & );
  virtual void visit ( Minerva::Core::TileEngine::Body & );
  virtual void visit ( Minerva::Core::TileEngine::Node & );
  
protected:

  // Use reference counting.
  virtual ~BuildScene();

private:

  // No copying or assignment.
  BuildScene ( const BuildScene & );
  BuildScene &operator = ( const BuildScene & );

  void                      _destroy();

  osg::Group *_scene;
  BuildOptions _options;
  IUnknown::RefPtr _caller;
};


} // namespace Visitors
} // namespace Core
} // namespace Minerva



#endif // _STAR_SYSTEM_BUILD_SCENE_VISITOR_CLASS_H_
