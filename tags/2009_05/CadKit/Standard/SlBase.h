
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlBase.h: Runtime type info (rtti) and dynamic creation behavior.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_BASE_OBJECT_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_BASE_OBJECT_CLASS_H_

#include "SlType.h"
#include "SlTypedefs.h"
#include "SlBaseMacros.h"


namespace CadKit
{
class SlBase
{
public:

  static const SlType typeSlBase;
  enum { typeIdSlBase = 0x00000001 };

  const char *             getClassName() const;

  static  const SlType *   getBaseClassType();
  static  const SlType *   getClassType();
  virtual const SlType *   getType() const = 0;

  static  SlUint32         getClassTypeId();
  virtual SlUint32         getTypeId() const = 0;

  bool                     isOfType ( const SlType *classType ) const;
  bool                     isOfExactType ( const SlType *classType ) const;

protected:

  #ifdef _DEBUG
  SlBase();
  #else
  SlBase(){}
  #endif
};

}; // Namespace CadKit.

#endif // _CADKIT_STANDARD_LIBRARY_BASE_OBJECT_CLASS_H_
