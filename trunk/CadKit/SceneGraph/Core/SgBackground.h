
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
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
#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlColor.h"
#endif

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

  SG_DECLARE_DYNAMIC_NODE(SgBackground,0x00001001);
};
};

#endif
