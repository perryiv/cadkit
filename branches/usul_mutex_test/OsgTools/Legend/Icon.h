
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
#include "Usul/Math/Vector2.h"
#include "Usul/Pointers/Pointers.h"

namespace osg { class Node; }

namespace OsgTools {
namespace Legend {

class OSG_TOOLS_EXPORT Icon : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Math::Vec2ui Size;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Icon );

  Icon();

  /// Get/Set the suggested size.
  void                    sizeHint ( const Size& s );
  Size                    sizeHint () const;

  virtual osg::Node*      buildScene ( unsigned int width, unsigned int height ) = 0;

protected:
  virtual ~Icon();

private:
  Size _size;
};

}
}


#endif //__OSG_TOOLS_LEGEND_ICON_H__
