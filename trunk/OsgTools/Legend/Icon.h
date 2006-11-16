
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_LEGEND_ICON_H__
#define __OSG_TOOLS_LEGEND_ICON_H__

#include "OsgTools/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

namespace osg { class Node; }

namespace OsgTools {
namespace Legend {

class OSG_TOOLS_EXPORT Icon : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Icon );

  Icon();

  /// Get/Set the width.
  void                    width( unsigned int w );
  unsigned int            width() const;

  /// Get/Set the height
  void                    height( unsigned int h );
  unsigned int            height() const;

  virtual osg::Node*      buildScene() = 0;

protected:
  virtual ~Icon();

private:
  unsigned int _width;
  unsigned int _height;
};

}
}


#endif //__OSG_TOOLS_LEGEND_ICON_H__
