
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
#include "OsgTools/Legend/Icon.h"
#include "OsgTools/Legend/Text.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <vector>

namespace OsgTools {
namespace Legend {

class OSG_TOOLS_EXPORT LegendObject : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( LegendObject );

  LegendObject();

  /// Get/Set the icon.
  void                   icon( Icon* );
  Icon*                  icon();
  const Icon*            icon() const;

  /// Get the text.
  Text*                  operator[] ( unsigned int i );
  Text*                  at ( unsigned int i );
  const Text*            at ( unsigned int i ) const;

  /// Add a text item.
  unsigned int           addText ( Text* );

  /// Get/Set the number of columns.
  void                   columns( unsigned int value );
  unsigned int           columns() const;

  /// Get the percentage at given column.
  float&                 percentage( unsigned int i );

  osg::Node*             buildScene();

  /// Set the size.
  void                   size( unsigned int width, unsigned int height );

protected:
  virtual ~LegendObject();

private:
  Icon::RefPtr _icon;

  typedef std::vector < Text::RefPtr > TextVector;
  TextVector _texts;

  std::vector< float > _percentages;

  unsigned int _width;
  unsigned int _height;
};

}
}


#endif //__OSG_TOOLS_LEGEND_OBJECT_H__
