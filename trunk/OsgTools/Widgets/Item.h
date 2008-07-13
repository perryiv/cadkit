
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_WIDGETS_ITEM_H__
#define __OSG_TOOLS_WIDGETS_ITEM_H__

#include "OsgTools/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Vec4"

namespace osg { class Node; }

namespace OsgTools {
namespace Widgets {

class OSG_TOOLS_EXPORT Item : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Math::Vec2ui Size;
  typedef Size::value_type SizeType;
  typedef osg::Vec4 Color;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Item );
  
  /// Set/get the background color.
  void                    backgroundColor ( const Color& );
  Color                   backgroundColor() const;
  
  /// Set/get the border color.
  void                    borderColor ( const Color& );
  Color                   borderColor() const;

  /// Build the scene.
  virtual osg::Node*      buildScene() = 0;
  
  /// Estimate the needed size for the item.  By default it returns the suggested size.
  virtual Size            estimateSize() const;
  
  /// Set the position.
  void                    position ( SizeType x, SizeType y );
  
  /// Set/get the suggested size.
  void                    size ( SizeType width, SizeType height );
  void                    size ( const Size& s );
  Size                    size () const;

  /// Get x and y position.
  SizeType                x() const;
  SizeType                y() const;
  
protected:
  
  Item();
  virtual ~Item();

  osg::Node*              _buildBackground ( unsigned int width, unsigned int height );
  
private:
  Size _size;
  Size _position;
  Color _backgroundColor;
  Color _borderColor;
};

}
}


#endif //__OSG_TOOLS_LEGEND_ITEM_H__
