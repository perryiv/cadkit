
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to manage add, deleting, hiding, and showing in the scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_SCENE_MANAGER_H__
#define __MINERVA_SCENE_MANAGER_H__

#include "Minerva/Core/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Interfaces/ILayer.h"

#include "osg/ref_ptr"
#include "osg/Group"
#include "osg/Array"
#include "osg/Node"
#include "osg/MatrixTransform"

#include <map>

namespace Minerva {
namespace Core {
namespace Scene {

class MINERVA_EXPORT SceneManager : public Usul::Base::Referenced
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced         BaseClass;
  typedef Usul::Threads::RecursiveMutex  Mutex;
  typedef Usul::Threads::Guard< Mutex >  Guard;
  typedef Usul::Interfaces::ILayer       Layer;
 
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SceneManager );

  /// Constructor.
  SceneManager();

  /// Build the scene.
  void                       buildScene( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Get the root.
  osg::Node*                 root() { return _root.get(); }

  // Add/Remove layer.
  void                       addLayer            ( Layer *layer );
  void                       removeLayer         ( const std::string& guid );
  bool                       hasLayer            ( const std::string& guid ) const;
  Layer *                    getLayer            ( const std::string& guid );

  /// Resize
  void                       resize ( unsigned int width, unsigned int height );

  /// Get/Set the dirty flag.
  bool                       dirty() const;
  void                       dirty( bool );

  /// Clear the internal state.
  void                       clear();

protected:

  virtual ~SceneManager();
  
private:

  typedef std::map < std::string, Layer::QueryPtr >               Layers;

  // The mutex.
  mutable Mutex _mutex;

  // OSG nodes.
  osg::ref_ptr < osg::Group >      _root;

  Layers _layers;

  bool _dirty;
};

}
}
}

#endif // __MINERVA_SCENE_MANAGER_H__
