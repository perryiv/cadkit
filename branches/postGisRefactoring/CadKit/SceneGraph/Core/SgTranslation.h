
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgTranslation.h: A Translation class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_TRANSLATION_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_TRANSLATION_H_

#include "SgTransformation.h"

#include "Standard/SlVec3.h"


namespace CadKit
{
class SG_API SgTranslation : public SgTransformation
{
public:

  SgTranslation();
  SgTranslation ( const SgTranslation &translation );
  SgTranslation ( const SlVec3f &translation );

  const SlVec3f &     getTranslation() const { return _translation; }
  void                setTranslation ( const SlVec3f &translation ) { _translation = translation; }

protected:
  
  SlVec3f _translation;

  virtual ~SgTranslation();

  SG_DECLARE_DYNAMIC_NODE ( SgTranslation, 1033951105 );
};
};

#endif
