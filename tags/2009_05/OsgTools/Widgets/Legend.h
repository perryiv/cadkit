
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

#include "OsgTools/Widgets/Item.h"
#include "OsgTools/Widgets/LegendObject.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <vector>

namespace OsgTools {
namespace Widgets {

class OSG_TOOLS_EXPORT Legend : public OsgTools::Widgets::Item
{
public:
  typedef OsgTools::Widgets::Item BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Legend );

  Legend();

  enum GrowDirectionMode
  {
    GROW_DIRECTION_DOWN,
    GROW_DIRECTION_UP
  };

  /// Clear.
  void                 clear();

  /// Add a legend object.
  void                 addRow ( LegendObject * );

  /// Build the scene.
  osg::Node*           buildScene ( unsigned int depth = 0 );

  /// Get/Set the grow direction
  void                 growDirection( GrowDirectionMode mode );
  GrowDirectionMode    growDirection() const;
  
  /// Get the number of rows.
  unsigned int         numRows() const;
  
protected:
  virtual ~Legend();

private:
  typedef std::vector < LegendObject::RefPtr > LegendObjects;

  LegendObjects      _legendObjects;
  GrowDirectionMode  _growDirection;
};

}
}


#endif //__OSG_TOOLS_LEGEND_H__
