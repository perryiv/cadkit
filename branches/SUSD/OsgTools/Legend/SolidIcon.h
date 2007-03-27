
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_LEGEND_SOLID_ICON_H__
#define __OSG_TOOLS_LEGEND_SOLID_ICON_H__

#include "OsgTools/Export.h"
#include "OsgTools/Legend/Icon.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Material"

namespace osg { class Node; }

namespace OsgTools {
namespace Legend {

class OSG_TOOLS_EXPORT SolidIcon : public OsgTools::Legend::Icon
{
public:
  typedef OsgTools::Legend::Icon BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SolidIcon );

  SolidIcon();

  void                    material ( osg::Material * );
  osg::Material *         material();
  const osg::Material *   material() const;

  virtual osg::Node*      buildScene();

protected:
  virtual ~SolidIcon();

private:
  osg::ref_ptr < osg::Material > _material;
};

}
}


#endif //__OSG_TOOLS_LEGEND_SOLID_ICON_H__
