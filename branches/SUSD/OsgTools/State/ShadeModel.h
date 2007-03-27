
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

#ifndef _OSG_TOOLS_SHADE_MODEL_CLASS_H_
#define _OSG_TOOLS_SHADE_MODEL_CLASS_H_

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"

#include "Usul/Interfaces/IShadeModel.h"

#include "osg/ShadeModel"


namespace OsgTools {
namespace State {


struct OSG_TOOLS_EXPORT ShadeModel
{
  typedef Usul::Interfaces::IShadeModel IShadeModel;

  static bool                         has    ( osg::ShadeModel::Mode mode, const osg::StateSet * );
  static bool                         has    ( const osg::StateSet * );

  static osg::ShadeModel::Mode        mode   ( IShadeModel::Mode m );
  static IShadeModel::Mode            mode   ( osg::ShadeModel::Mode m );

  static void                         remove ( osg::StateSet * );

  static void                         set    ( osg::ShadeModel::Mode mode, osg::StateSet * );

  static void                         toggle ( osg::ShadeModel::Mode mode, osg::StateSet * );
};


}; // namespace State
}; // namespace OsgTools


#endif // _OSG_TOOLS_SHADE_MODEL_CLASS_H_
