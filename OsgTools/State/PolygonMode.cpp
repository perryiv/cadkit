///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for working with polygon modes.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/State/PolygonMode.h"

#include "osg/StateSet"
#include "osg/PolygonMode"

using namespace OsgTools;
using namespace OsgTools::State;


///////////////////////////////////////////////////////////////////////////////
//
//  Set the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonMode::set ( osg::PolygonMode::Face face, osg::PolygonMode::Mode mode, osg::StateSet *ss )
{
  if ( ss )
  {
    // Make a polygon-mode.
    osg::ref_ptr<osg::PolygonMode> pm ( new osg::PolygonMode );
    pm->setMode ( face, mode );

    // Set the state. Make it override any other similar states.
    typedef osg::StateAttribute Attribute;
    ss->setAttributeAndModes ( pm.get(), Attribute::OVERRIDE | Attribute::ON );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonMode::remove ( osg::StateSet *ss )
{
  if ( ss )
  {
    // This will delete any existing attribute in the state-set.
    ss->removeAttribute ( osg::StateAttribute::POLYGONMODE );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonMode::toggle ( osg::PolygonMode::Face face, osg::PolygonMode::Mode mode, osg::StateSet *ss )
{
  if ( ss )
  {
    if ( PolygonMode::has ( face, mode, ss ) )
      PolygonMode::remove ( ss );
    else
      PolygonMode::set ( face, mode, ss );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

bool PolygonMode::has ( osg::PolygonMode::Face face, osg::PolygonMode::Mode mode, const osg::StateSet *ss )
{
  // Handle no state-set.
  if ( 0x0 == ss )
    return false;

  // Get the polygon-mode attribute, if any.
  const osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::POLYGONMODE );
  if ( !sa )
    return false;

  // Should be true.
  const osg::PolygonMode *pm = dynamic_cast < const osg::PolygonMode * > ( sa );
  if ( !pm )
    return false;

  // Is the mode the same?
  return ( pm->getMode ( face ) == mode );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

bool PolygonMode::has ( osg::PolygonMode::Face face, const osg::StateSet *ss )
{
  // See if there is any polygon-mode for the given face.
  const bool filled ( PolygonMode::has ( face, osg::PolygonMode::FILL,  ss  ) );
  const bool lines  ( PolygonMode::has ( face, osg::PolygonMode::LINE,  ss ) );
  const bool points ( PolygonMode::has ( face, osg::PolygonMode::POINT, ss ) );
  return ( filled || lines || points );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

bool PolygonMode::has ( const osg::StateSet *ss )
{
  // See if there is any polygon-mode at all.
  const bool front ( PolygonMode::has ( osg::PolygonMode::FRONT,          ss ) );
  const bool back  ( PolygonMode::has ( osg::PolygonMode::BACK,           ss ) );
  const bool both  ( PolygonMode::has ( osg::PolygonMode::FRONT_AND_BACK, ss ) );
  return ( front || back || both );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert some IPolygonMode::Mode values to an osg::PolygonMode::Mode.
//
///////////////////////////////////////////////////////////////////////////////

osg::PolygonMode::Mode PolygonMode::mode ( Usul::Interfaces::IPolygonMode::Mode m )
{
  switch ( m )
  {
    case Usul::Interfaces::IPolygonMode::POINTS:
      return osg::PolygonMode::POINT;

    case Usul::Interfaces::IPolygonMode::WIRE_FRAME:
      return osg::PolygonMode::LINE;
      
    case Usul::Interfaces::IPolygonMode::FILLED:
      return osg::PolygonMode::FILL;

    default:
      throw std::runtime_error ( "Error 3148945400: Invalid polygon mode." );
  }
}
