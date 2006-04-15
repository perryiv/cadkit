
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ADD_TRIANGLE_H__
#define __ADD_TRIANGLE_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IFoxEvent.h"
#include "Usul/Interfaces/ISetCursor.h"
#include "Usul/Interfaces/ICleanUp.h"

namespace FX { class FXCursor; }
namespace osg { class Node; class Group; }
namespace osgUtil { class Hit; }
namespace OsgTools { namespace Triangles { class SharedVertex; } }

#include "osg/Vec3f"
#include "osg/Vec4f"

#include <list>


namespace Helios {
namespace Plugins {
namespace EditPolygons {


class AddTriangle : public Usul::Base::Referenced,
                    public Usul::Interfaces::IFoxEvent,
                    public Usul::Interfaces::ISetCursor,
                    public Usul::Interfaces::ICleanUp
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef OsgTools::Triangles::SharedVertex SharedVertex;
  typedef USUL_VALID_ACCESS_REF_POINTER ( SharedVertex ) SharedVertexPtr;
  typedef std::pair < osg::Vec3f, SharedVertexPtr > VertexPair;
  typedef std::list < VertexPair > Vertices;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( AddTriangle );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  AddTriangle();

  // Usul::Interfaces::IFoxEvent
  virtual bool            execute ( Usul::Interfaces::IUnknown *caller, bool left, bool middle, bool right, bool motion, float x, float y, float z );

  // Usul::Interfaces::ISetCursor
  virtual FX::FXCursor*   getCursor();

  // Usul::Interfaces::ICleanUp
  virtual void            cleanUp ( Usul::Interfaces::IUnknown* caller );

protected:

  virtual ~AddTriangle();

  osg::Group *            _buildNeighborTriangles ( const SharedVertex *sv, Usul::Interfaces::IUnknown *caller ) const;

  VertexPair              _closestVertex ( const osgUtil::Hit& hit, Usul::Interfaces::IUnknown *caller );

  osg::Node *             _makeDecoration ( const osg::Vec3f &v0, const osg::Vec3f &v1, const osg::Vec3f &v2, const osg::Vec3f &n ) const;
  osg::Node *             _makeSphere   ( const osg::Vec3f& center, double radius, const osg::Vec4f &color ) const;

  void                    _processLeftRelease ( const osgUtil::Hit &hit, Usul::Interfaces::IUnknown *caller );
  void                    _processMouseMotion ( const osgUtil::Hit &hit, Usul::Interfaces::IUnknown *caller );
  void                    _processVertices    ( Usul::Interfaces::IUnknown *caller );

private:

  Vertices _vertices;
  FX::FXCursor *_cursor; // Raw pointer allows forward declaration above.
};


} //namespace EditPolygons
} //namespace Plugins
} //namespace Helios


#endif //__ADD_TRIANGLE_H__


