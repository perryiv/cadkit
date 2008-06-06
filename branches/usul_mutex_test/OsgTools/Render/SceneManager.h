
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_RENDER_SCENE_MANAGER_H__
#define __OSG_TOOLS_RENDER_SCENE_MANAGER_H__

#include "OsgTools/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/ref_ptr"
#include "osg/ClipNode"
#include "osg/Group"
#include "osg/Light"
#include "osg/Camera"

#include "osgText/Text"

namespace OsgTools {
namespace Render {

class OSG_TOOLS_EXPORT SceneManager : public Usul::Base::Referenced
{
public:
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SceneManager );

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef osg::Group Group;

  SceneManager();

  /// Add a global light.  This will be placed in an absolute reference frame.
  unsigned int          globalLight ( osg::Light* light );

  /// Get global light at index i.
  osg::Light*           globalLight( unsigned int i );
  const osg::Light*     globalLight( unsigned int i ) const;

  /// Remove global light at index i.
  void                  globalLightRemove ( unsigned int i );

  /// Clear all global lights.
  void                  globalLightClear ();

  /// Get the scene
  osg::Node *           scene();
  const osg::Node *     scene() const;

  /// Set the Model
  void                  model( osg::Node * );

  /// Get the model
  osg::Node *           model();
  const osg::Node *     model () const;

  // Add/Remove group from scene
  osg::Group*           groupGet    ( const std::string& );
  void                  groupRemove ( const std::string& );
  bool                  groupHas    ( const std::string& ) const;

  /// Add/Remove group from projection node
  osg::Group*           projectionGroupGet    ( const std::string& );
  void                  projectionGroupRemove ( const std::string& );
  bool                  projectionGroupHas    ( const std::string& ) const;

  /// Get the camera node
  osg::Camera*          camera();

  /// Get the clip node
  const osg::ClipNode*  clipNode() const;
  osg::ClipNode*        clipNode();

  /// Get text at the (x,y) on the screen.
  osgText::Text*        getText( unsigned int x, unsigned int y );

  /// Set text value.
  void                  setText( unsigned int x, unsigned int y, const std::string& text, const osg::Vec4f& color, const osg::Vec4f& backDropColor = osg::Vec4f ( 0, 0, 0, 0 ) );

  /// Remove text
  void                  removeText( unsigned int x, unsigned int y );

  /// Resize.
  void                  resize ( double x, double width, double y, double height );

protected:

  /// Use reference counting.
  ~SceneManager();

private:

  typedef osg::ref_ptr < Group >                   GroupPtr;
  typedef osg::ref_ptr < osg::ClipNode >           ClipNodePtr;
  typedef osg::ref_ptr < osg::Camera >             CameraPtr;
  typedef osg::ref_ptr < osgText::Text >           TextPtr;
  typedef std::pair < unsigned int, unsigned int > XYPair;
  typedef std::map < XYPair, TextPtr >             TextMap;
  typedef osg::ref_ptr< osg::Light >               LightPtr;
  typedef std::vector < LightPtr >                 Lights;
  typedef std::map < std::string, GroupPtr >       GroupMap;

  GroupPtr _scene;
  ClipNodePtr _clipNode;
  CameraPtr _cameraNode;
  GroupPtr _lightNode;
  Lights   _lights;

  GroupMap _groupMap;
  GroupMap _projectionMap;

  TextMap _textMap;
};


}
}

#endif // __OSG_TOOLS_RENDER_SCENE_MANAGER_H__


