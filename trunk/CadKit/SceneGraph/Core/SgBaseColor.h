
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgBaseColor.h: Color class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_BASE_COLOR_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_BASE_COLOR_H_

#include "SgAttribute.h"

#include "Standard/SlColor.h"


namespace CadKit
{
class SG_API SgBaseColor : public SgAttribute
{
public:

  SgBaseColor();
  SgBaseColor ( const SgBaseColor &baseColor );
  SgBaseColor ( const SlVec4f &color );

  const SlColorf &      getColor() const { return _color; }

  virtual bool          isEqualVisibleProperties ( const SgNode &node ) const;

  void                  setColor ( const SlVec4f &color ) { _color.setValue ( color ); }
  void                  setValue ( const SgBaseColor &baseColor ) { _color.setValue ( baseColor._color ); }

protected:

  SlColorf _color;

  virtual ~SgBaseColor();

  SG_DECLARE_DYNAMIC_NODE ( SgBaseColor, 1033858463 );
};

}; // namespace CadKit

#endif
