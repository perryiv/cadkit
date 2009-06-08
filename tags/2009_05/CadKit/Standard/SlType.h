
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlType.h: For runtime type info (rtti).
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TYPE_INFO_STRUCT_H_
#define _CADKIT_STANDARD_LIBRARY_TYPE_INFO_STRUCT_H_


namespace CadKit
{
class SlBase;

struct SlType
{
   const char *_className;
   SlBase *(* _createFunction)(); // This can be NULL.
   const SlType *_baseClass;

   SlBase *                 createObject() const;
   bool                     isDerivedFrom ( const SlType *baseClass ) const;
};

}; // Namespace CadKit.

#endif // _CADKIT_STANDARD_LIBRARY_TYPE_INFO_STRUCT_H_
