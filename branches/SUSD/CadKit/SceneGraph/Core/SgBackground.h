
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgBackground.h: Background class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_BACKGROUND_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_BACKGROUND_H_

#include "SgAttribute.h"

#include "Standard/SlColor.h"


namespace CadKit
{
class SG_API SgBackground : public SgAttribute
{
public:

  SgBackground();
  SgBackground ( const SgBackground &background );

  const SlColorf &      getTopLeftCorner()      const { return _topLeftCorner; }
  const SlColorf &      getTopRightCorner()     const { return _topRightCorner; }
  const SlColorf &      getBottomLeftCorner()   const { return _bottomLeftCorner; }
  const SlColorf &      getBottomRightCorner()  const { return _bottomRightCorner; }
  const SlFloat32 &     getFarDistance()        const { return _farDistance; }

  void                  setTopLeftCorner      ( const SlColorf &color ) { _topLeftCorner.setValue ( color ); }
  void                  setTopRightCorner     ( const SlColorf &color ) { _topRightCorner.setValue ( color ); }
  void                  setBottomLeftCorner   ( const SlColorf &color ) { _bottomLeftCorner.setValue ( color ); }
  void                  setBottomRightCorner  ( const SlColorf &color ) { _bottomRightCorner.setValue ( color ); }
  void                  setFarDistance        ( const SlFloat32 &dist ) { _farDistance = dist; }

  void                  setValue ( const SgBackground &background );

protected:

  virtual ~SgBackground();

  SlColorf _topLeftCorner;
  SlColorf _topRightCorner;
  SlColorf _bottomLeftCorner;
  SlColorf _bottomRightCorner;
  SlFloat32 _farDistance; // A positive value.

  SG_DECLARE_DYNAMIC_NODE ( SgBackground, 1033858464 );
};
};

#endif
