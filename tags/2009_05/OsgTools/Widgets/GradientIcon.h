
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_LEGEND_GRADIENT_ICON_H__
#define __OSG_TOOLS_LEGEND_GRADIENT_ICON_H__

#include "OsgTools/Export.h"
#include "OsgTools/Widgets/Item.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Material"

namespace osg { class Node; }

namespace OsgTools {
namespace Widgets {

class OSG_TOOLS_EXPORT GradientIcon : public OsgTools::Widgets::Item
{
public:
  typedef OsgTools::Widgets::Item BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( GradientIcon );

  GradientIcon();

  void                    minColor( const osg::Vec4& min );
  const osg::Vec4 &       minColor() const;

  void                    maxColor( const osg::Vec4& max );
  const osg::Vec4 &       maxColor() const;

  virtual osg::Node*      buildScene ( unsigned int depth = 0 );

protected:
  virtual ~GradientIcon();

private:
  osg::Vec4 _minColor;
  osg::Vec4 _maxColor;
};

}
}


#endif //__OSG_TOOLS_LEGEND_GRADIENT_ICON_H__
