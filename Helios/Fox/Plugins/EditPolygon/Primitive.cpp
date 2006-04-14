
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Primitive.h"

#include "OsgTools/Visitor.h"

#include "FoxTools/Cursors/Factory.h"
#include "FoxTools/Headers/Cursor.h"
#include "FoxTools/Headers/App.h"

#include "Usul/Interfaces/ISceneIntersect.h"
#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/IDeletePrimitive.h"
#include "Usul/Interfaces/ISendMessage.h"

#include "osg/Geometry"
#include "osg/Geode"

#include "osgUtil/IntersectVisitor"

using namespace OsgFox::Plugins::EditPolygons;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Primitive , Primitive::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Primitive::Primitive (  ) : BaseClass(),
  _cursor ( FoxTools::Cursors::Factory::create ( FoxTools::Cursors::Factory::CURSOR_GUN_SIGHT ) )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Primitive::~Primitive()
{
  delete _cursor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Primitive::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IFoxEvent::IID:
    return static_cast<IFoxEvent*>(this);
  case Usul::Interfaces::ISetCursor::IID:
    return static_cast< Usul::Interfaces::ISetCursor*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  If click intersects the scene, remove primitive
//
///////////////////////////////////////////////////////////////////////////////

bool Primitive::execute ( Usul::Interfaces::IUnknown *caller,  bool left, bool middle, bool right, bool motion, float x, float y, float z )
{
  // Only respond to left mouse button clicks.
  if( left )
  {
    // Get needed interfaces.
    Usul::Interfaces::ISceneIntersect::ValidQueryPtr sceneIntersect ( caller );
    Usul::Interfaces::IGetDocument::ValidQueryPtr    getDocument    ( caller );
    Usul::Interfaces::IDocument::ValidQueryPtr       document       ( getDocument->getDocument() );

    osgUtil::Hit hit;

    // See if the mouse click intersects the scene.
    if( sceneIntersect->intersect( x, y, hit ) )
    {
      Usul::Interfaces::IDeletePrimitive::QueryPtr deletePrimitive ( document );

      // Use the document if it supports deleting, if it doesn't use the scene.
      if( deletePrimitive.valid() )
        this->_deleteUsingDocument( caller, hit );
      else
        this->_deleteUsingScene( caller, hit );
      
      return true;
    }
  }

  //Returns false if the event was not a left mouse button click or if the click did not intersect the scene.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove primitive using the document
//
///////////////////////////////////////////////////////////////////////////////

void Primitive::_deleteUsingDocument( Usul::Interfaces::IUnknown *caller, osgUtil::Hit& hit )
{
  // Get needed interfaces
  Usul::Interfaces::IGetDocument::ValidQueryPtr       getDocument     ( caller );
  Usul::Interfaces::IDocument::ValidQueryPtr          document        ( getDocument->getDocument() );
  Usul::Interfaces::IDeletePrimitive::ValidQueryPtr   deletePrimitive ( document );
  Usul::Interfaces::ISendMessage::ValidQueryPtr       sendMessage     ( document );

  // Delete the primitive
  deletePrimitive->deletePrimitive( hit );

  // Redraw the scene
  sendMessage->sendMessage ( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );

  // Document is now modifed
  document->modified( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove primitive using the scene
//
///////////////////////////////////////////////////////////////////////////////

void Primitive::_deleteUsingScene   ( Usul::Interfaces::IUnknown *caller, osgUtil::Hit& hit )
{
  // Get the geode we need to fix
  osg::ref_ptr< osg::Geode> geode ( hit._geode.get() );

  // Get the geometry we need to fix
  osg::Geometry *geometry = hit._drawable->asGeometry();
  if ( !geometry )
    return;

  // Make sure this geometry contains a single primitive set
  // TODO handle cases with more than one primitive set
  const osg::Geometry::PrimitiveSetList &prims = geometry->getPrimitiveSetList();
  if ( 1 != prims.size() )
    return;

  // If we get to here then we are dealing with a single geometry
  osg::ref_ptr< osg::Vec3Array > vertices ( dynamic_cast < osg::Vec3Array* > ( geometry->getVertexArray() ) );
  osg::ref_ptr< osg::Vec3Array > normals  ( dynamic_cast < osg::Vec3Array* > ( geometry->getNormalArray() ) );
  osg::ref_ptr< osg::DrawArrays > draw    ( dynamic_cast < osg::DrawArrays* > ( geometry->getPrimitiveSet ( 0 ) ) );

  // Check and make sure we have valid pointers...
  if( 0x0 == vertices.get() )
    throw std::runtime_error ( "Error 1024532594: Vertices are not valid.\n" );

  if( 0x0 == normals.get() )
    throw std::runtime_error ( "Error 1024532595: Normals are not valid.\n" );

  if( 0x0 == draw.get() )
    throw std::runtime_error ( "Error 1024532594: Draw arrays are not valid.\n" );
  
  const unsigned int mode ( geometry->getPrimitiveSet( 0 )->getMode() );

  unsigned int primitiveIndexMultiplier ( 0 );

  switch ( mode )
  {
  case osg::PrimitiveSet::LINE_LOOP:
  case osg::PrimitiveSet::LINE_STRIP:
  case osg::PrimitiveSet::LINES:
    primitiveIndexMultiplier = 2;
    break;
  case osg::PrimitiveSet::TRIANGLE_FAN:
  case osg::PrimitiveSet::TRIANGLE_STRIP:
  case osg::PrimitiveSet::TRIANGLES:
    primitiveIndexMultiplier = 3;
    break;
  case osg::PrimitiveSet::QUAD_STRIP:
  case osg::PrimitiveSet::QUADS:
    primitiveIndexMultiplier = 4;
    break;
  case osg::PrimitiveSet::POINTS:
    primitiveIndexMultiplier = 1;
    break;
  case osg::PrimitiveSet::POLYGON:
  default:
    break;
  }

  const unsigned int primitiveIndex ( hit._primitiveIndex * primitiveIndexMultiplier );

  //Erase the vertices for the primitive and the normal
  vertices->erase( vertices->begin() += primitiveIndex, vertices->begin() += primitiveIndex + primitiveIndexMultiplier );

  //Handle the normals
  switch ( geometry->getNormalBinding ( ) )
  {
  case osg::Geometry::BIND_PER_VERTEX:
    normals->erase( normals->begin() += primitiveIndex, normals->begin() += primitiveIndex + primitiveIndexMultiplier );
    break;
  case osg::Geometry::BIND_PER_PRIMITIVE:
    normals->erase( normals->begin() += ( primitiveIndex / primitiveIndexMultiplier ) );
    break;
  default:
    break;
  }

  //Handle colors if there are any
  if( osg::Vec4Array *colors = dynamic_cast <osg::Vec4Array*> ( geometry->getColorArray() ) )
  {
    switch ( geometry->getColorBinding() )
    {
    case osg::Geometry::BIND_PER_VERTEX:
      colors->erase( colors->begin() += primitiveIndex, colors->begin() += primitiveIndex + primitiveIndexMultiplier );
      break;
    case osg::Geometry::BIND_PER_PRIMITIVE:
      colors->erase( colors->begin() += ( primitiveIndex / primitiveIndexMultiplier ) );
      break;
    default:
      break;
    }
  }

  //set the new draw arrays count
  draw->setCount ( vertices->size() );

  //Get needed interfaces
  Usul::Interfaces::IGetDocument::ValidQueryPtr  getDocument  ( caller );
  Usul::Interfaces::IDocument::ValidQueryPtr     document     ( getDocument->getDocument() );
  Usul::Interfaces::ISendMessage::ValidQueryPtr  sendMessage  ( document );

  //Document is now modifed
  document->modified( true );

  // Redraw the scene
  sendMessage->sendMessage ( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Get the cursor
//
///////////////////////////////////////////////////////////////////////////////

FX::FXCursor* Primitive::getCursor()
{
  return _cursor;
}

