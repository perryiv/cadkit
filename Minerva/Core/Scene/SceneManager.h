
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

#include "OsgTools/Legend/Legend.h"

#include "osg/ref_ptr"
#include "osg/Group"
#include "osg/Array"
#include "osg/Node"
#include "osg/Camera"
#include "osg/MatrixTransform"

#include "osgText/Text"

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

  enum LegendPosition
  {
    LEGEND_TOP_LEFT,
    LEGEND_TOP_RIGHT,
    LEGEND_BOTTOM_RIGHT,
    LEGEND_BOTTOM_LEFT
  };

  /// Get/Set the legend position.
  void                       legendPosition ( LegendPosition position );
  LegendPosition             legendPosition ( ) const;

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

  /// Text for updating current date.
  osgText::Text &            dateText ();
protected:

  virtual ~SceneManager();
  
  void                       _buildLegend();

  void                       _setLegendPosition ( unsigned int legendWidth );
private:

  typedef std::map < std::string, Layer::QueryPtr >               Layers;

  // The mutex.
  mutable Mutex _mutex;

  // OSG nodes.
  osg::ref_ptr < osg::Group >      _root;
  osg::ref_ptr < osg::Camera >     _camera;
  osg::ref_ptr < osgText::Text >   _dateText;

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
  LegendPosition _legendPosition;
};

}
}
}

#endif // __MINERVA_SCENE_MANAGER_H__
