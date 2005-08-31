///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for working with shade models.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/State/ShadeModel.h"

#include "osg/StateSet"
#include "osg/ShadeModel"

using namespace OsgTools;
using namespace OsgTools::State;


///////////////////////////////////////////////////////////////////////////////
//
//  Set the shade model.
//
///////////////////////////////////////////////////////////////////////////////

void ShadeModel::set ( osg::ShadeModel::Mode mode, osg::StateSet *ss )
{
  if ( ss )
  {
    // Make a shade-model.
    osg::ref_ptr<osg::ShadeModel> sm ( new osg::ShadeModel );
    sm->setMode ( mode );

    // Set the state. Make it override any other similar states.
    typedef osg::StateAttribute Attribute;
    ss->setAttributeAndModes ( sm.get(), Attribute::OVERRIDE | Attribute::ON );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the shade model.
//
///////////////////////////////////////////////////////////////////////////////

void ShadeModel::remove ( osg::StateSet *ss )
{
  if ( ss )
  {
    // This will delete any existing attribute in the state-set.
    ss->removeAttribute ( osg::StateAttribute::SHADEMODEL );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the shade model.
//
///////////////////////////////////////////////////////////////////////////////

void ShadeModel::toggle ( osg::ShadeModel::Mode mode, osg::StateSet *ss )
{
  if ( ss )
  {
    if ( ShadeModel::has ( mode, ss ) )
      ShadeModel::remove ( ss );
    else
      ShadeModel::set ( mode, ss );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the shade model.
//
///////////////////////////////////////////////////////////////////////////////

bool ShadeModel::has ( osg::ShadeModel::Mode mode, const osg::StateSet *ss )
{
  // Handle no state-set.
  if ( 0x0 == ss )
    return false;

  // Get the shade-model attribute, if any.
  const osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::SHADEMODEL );
  if ( !sa )
    return false;

  // Should be true.
  const osg::ShadeModel *sm = dynamic_cast < const osg::ShadeModel * > ( sa );
  if ( !sm )
    return false;

  // Is the mode the same?
  return ( sm->getMode() == mode );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the shade model.
//
///////////////////////////////////////////////////////////////////////////////

bool ShadeModel::has ( const osg::StateSet *ss )
{
  // See if there is any polygon-mode for the given face.
  const bool smooth ( ShadeModel::has ( osg::ShadeModel::SMOOTH, ss ) );
  const bool flat   ( ShadeModel::has ( osg::ShadeModel::FLAT,   ss ) );
  return ( smooth || flat );
}
