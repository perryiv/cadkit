
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

#ifndef _OSG_TOOLS_POLYGON_MODE_CLASS_H_
#define _OSG_TOOLS_POLYGON_MODE_CLASS_H_

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"

#include "Usul/Interfaces/IPolygonMode.h"

#include "osg/PolygonMode"


namespace OsgTools {
namespace State {


struct OSG_TOOLS_EXPORT PolygonMode
{
  typedef Usul::Interfaces::IPolygonMode IPolygonMode;

  static osg::PolygonMode::Mode       mode   ( IPolygonMode::Mode m );

  static bool                         has    ( osg::PolygonMode::Face face, osg::PolygonMode::Mode mode, const osg::StateSet * );
  static bool                         has    ( osg::PolygonMode::Face face, const osg::StateSet * );
  static bool                         has    ( const osg::StateSet * );

  static void                         remove ( osg::StateSet * );

  static void                         set    ( osg::PolygonMode::Face face, osg::PolygonMode::Mode mode, osg::StateSet * );

  static void                         toggle ( osg::PolygonMode::Face face, osg::PolygonMode::Mode mode, osg::StateSet * );
};


}; // namespace State
}; // namespace OsgTools


#endif // _OSG_TOOLS_POLYGON_MODE_CLASS_H_
