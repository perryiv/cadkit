
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_LEGEND_OBJECT_H__
#define __OSG_TOOLS_LEGEND_OBJECT_H__

#include "OsgTools/Export.h"
#include "OsgTools/Widgets/Item.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <vector>

namespace OsgTools {
namespace Widgets {

class OSG_TOOLS_EXPORT LegendObject : public Item
{
public:
  typedef Item BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( LegendObject );

  LegendObject();

  /// Get the item.
  Item*                  operator[] ( unsigned int i );
  Item*                  at ( unsigned int i );
  const Item*            at ( unsigned int i ) const;

  /// Add a text item.
  unsigned int           addItem ( Item* );

  /// Build the scene.
  osg::Node*             buildScene ( unsigned int depth = 0 );
  
  /// Get/Set the number of columns.
  void                   columns( unsigned int value );
  unsigned int           columns() const;
  
  /// Estimate the needed size for the item.
  virtual Size           estimateSizeForWidth ( unsigned int w ) const;

  /// Get the percentage at given column.
  float&                 percentage( unsigned int i );

protected:
  virtual ~LegendObject();

private:

  typedef std::vector < Item::RefPtr > TextVector;
  TextVector _texts;

  std::vector< float > _percentages;
};

}
}


#endif //__OSG_TOOLS_LEGEND_OBJECT_H__
