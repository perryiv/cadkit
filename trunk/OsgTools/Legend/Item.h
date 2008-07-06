
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_LEGEND_ITEM_H__
#define __OSG_TOOLS_LEGEND_ITEM_H__

#include "OsgTools/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Pointers/Pointers.h"

namespace osg { class Node; }

namespace OsgTools {
namespace Legend {

class OSG_TOOLS_EXPORT Item : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Math::Vec2ui Size;
  typedef Size::value_type SizeType;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Item );

  /// Set/get the suggested size.
  void                    size ( SizeType width, SizeType height );
  void                    size ( const Size& s );
  Size                    size () const;

  virtual osg::Node*      buildScene() = 0;

protected:
  
  Item();
  virtual ~Item();

private:
  Size _size;
};

}
}


#endif //__OSG_TOOLS_LEGEND_ITEM_H__
