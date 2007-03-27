
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IQueryArray: Interface for querying an array.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_QUERY_ARRAY_H_
#define _CADKIT_INTERFACE_QUERY_ARRAY_H_

#include "IUnknown.h"
#include "Handles.h"


namespace CadKit
{
template <const unsigned int id, class ArrayType> class IQueryArray : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = id };

  // Inherit from this and define the virtual functions.
  class ArraySetter
  {
  public:
    virtual bool setData ( const unsigned int &index, const ArrayType &data ) = 0;
    virtual bool setSize ( const unsigned int &size ) = 0;
  };
};


}; // namespace CadKit


#endif // _CADKIT_INTERFACE_QUERY_ARRAY_H_
