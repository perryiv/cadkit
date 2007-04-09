
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
#include "Minerva/Core/DataObjects/DataObject.h"
#include "Minerva/Core/DataObjects/PointTime.h"
#include "Minerva/Core/Layers/Layer.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Interfaces/ISceneUpdate.h"

#include "OsgTools/Legend/Legend.h"
#include "OsgTools/Animate/DateGroup.h"

#include "osg/ref_ptr"
#include "osg/Group"
#include "osg/Array"
#include "osg/PrimitiveSet"
#include "osg/Sequence"
#include "osg/Node"
#include "osg/Projection"
#include "osg/MatrixTransform"


#include <map>

namespace Minerva {
namespace Core {
namespace Scene {

class MINERVA_EXPORT SceneManager : public Usul::Base::Referenced,
                                    public Usul::Interfaces::ISceneUpdate
{
public:
  /// Typedefs
  typedef Usul::Base::Referenced         BaseClass;
  typedef Usul::Threads::RecursiveMutex  Mutex;
  typedef Usul::Threads::Guard< Mutex >  Guard;
  typedef Minerva::Core::Layers::Layer   Layer;
 
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SceneManager );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor.
  SceneManager();

  /// Build the scene.
  void                       buildScene( Usul::Interfaces::IUnknown *caller = 0x0);

  /// Get the root.
  osg::Node*                 root() { return _root.get(); }

  // Add/Remove layer.
  void                       addLayer            ( Layer *layer );
  void                       removeLayer         ( const std::string& guid );
  bool                       hasLayer            ( const std::string& guid ) const;
  Layer *                    getLayer            ( const std::string& guid );

  /// Resize
  void                       resize( unsigned int width, unsigned int height );

  /// Show the feedback hud.
  void                       addFeedbackHud( int width, int height );
  void                       removeFeedbackHud( );

  /// Get/Set the dirty flag.
  bool                       dirty() const;
  void                       dirty( bool );

  typedef OsgTools::Animate::Settings Settings;

  void                        timestepType( Settings::TimestepType type );
  Settings::TimestepType      timestepType( ) const;

  void                        animate ( bool animate, bool accumulate, float speed, bool timeWindow, unsigned int numDays );

  /// Clear the internal state.
  void                       clear();

  void                       setDisplayText ( const std::string& text );

  /// Toggle the legend on and off.
  bool                       showLegend() const;
  void                       showLegend( bool b );

  /// Legend width in percentage of screen width.
  void                       legendWidth( float p );
  float                      legendWidth( ) const;

  /// Height of each item showing in the legend.
  void                       legendHeightPerItem( unsigned int height );
  unsigned int               legendHeightPerItem() const;

  /// Legend padding from the sides of the screen.  Value in pixels.
  void                       legendPadding( const osg::Vec2& padding );
  const osg::Vec2&           legendPadding() const;

protected:

  virtual ~SceneManager();

  virtual void sceneUpdate( Usul::Interfaces::IUnknown *caller = 0x0 );
  
private:

  typedef std::map < std::string, Layer::RefPtr >               Layers;

  // The mutex.
  Mutex _mutex;

  // OSG nodes.
  osg::ref_ptr < osg::Group >                    _root;			// The root node of the scene graph
  osg::ref_ptr < osg::Group >                    _static_root;		// The parent node for all static data
  osg::ref_ptr < osg::Projection >               _projectionNode;
  osg::ref_ptr < osg::MatrixTransform >          _updateNode;
  osg::ref_ptr < osg::Geode >                    _displayNode;
  osg::ref_ptr < OsgTools::Animate::DateGroup >  _animateNode;

  Layers _layers;

  bool _dirty;

  /// Screen size members.
  unsigned int _width;
  unsigned int _height;

  /// Legend members.
  OsgTools::Legend::Legend::RefPtr _legend;
  bool _showLegend;
  float _legendWidth;
  unsigned int _legendHeightPerItem;
  osg::Vec2 _legendPadding;
};

}
}
}

#endif // __WNV_SCENE_MANAGER_H__
