
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_LEGEND_H__
#define __OSG_TOOLS_LEGEND_H__

#include "OsgTools/Export.h"

#include "OsgTools/Legend/LegendObject.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <vector>

namespace OsgTools {
namespace Legend {

class OSG_TOOLS_EXPORT Legend : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Legend );

  Legend();

  enum GrowDirectionMode
  {
    DOWN,
    UP
  };

  /// Clear.
  void                 clear();

  /// Add a legend object.
  void                 addLegendObject ( LegendObject * );

  /// Build the scene.
  osg::Node*           buildScene();

  /// Set the maximium size.
  void                 maximiumSize( unsigned int width, unsigned int height );

  /// Get/Set the height per item
  void                 heightPerItem( unsigned int height );
  unsigned int         heightPerItem () const;

  /// Get/Set the grow direction
  void                 growDirection( GrowDirectionMode mode );
  GrowDirectionMode    growDirection() const;
protected:
  virtual ~Legend();

  osg::Node*          _buildBackground( unsigned int width, unsigned int height );

private:
  typedef std::vector < LegendObject::RefPtr > LegendObjects;

  LegendObjects      _legendObjects;
  unsigned int       _width;
  unsigned int       _height;
  unsigned int       _heightPerItem;
  GrowDirectionMode  _growDirection;
};

}
}


#endif //__OSG_TOOLS_LEGEND_H__
