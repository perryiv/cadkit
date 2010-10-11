
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbBasePrimitiveData: Holds data that defines a shape primitive. 
//  Associates this data with a generic token.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_BASE_LIBRARY_PRIMITIVE_DATA_CLASS_H_
#define _CADKIT_DATABASE_BASE_LIBRARY_PRIMITIVE_DATA_CLASS_H_

#include "Interfaces/Enum.h"

#include "Standard/SlPartitionedVector.h"
#include "Standard/SlAssert.h"

// To help shorten up the lines.
#define PDTA class OriginType, class ColorType, class ParameterType, class TokenType
#define PDTP OriginType, ColorType, ParameterType, TokenType


namespace CadKit
{
template <PDTA> class DbBasePrimitiveData
{
public:
  
  // TODO: Choose the correct data types.

  typedef SlPartitionedVector < unsigned int, unsigned int, OriginType >      Origins;
  typedef std::vector < ParameterType >   Parameters;
  typedef SlPartitionedVector < unsigned int, unsigned int, ColorType >       Colors;

  DbBasePrimitiveData ( const TokenType &token );
  ~DbBasePrimitiveData(){}

  bool                    calculateBindings();

  const TokenType &       getToken() const { return _token; }
  void                    setToken ( const TokenType &token ) { _token = token; }

  const Origins &         getOrigins()  const { return _origins; }
  const Parameters &      getParameters()   const { return _parameters; }
  const Colors &          getColors()    const { return _colors; }

  Origins &               getOrigins()  { return _origins; }
  Parameters &            getParameters()   { return _parameters; }
  Colors &                getColors()    { return _colors; }

  const VertexBinding &   getColorBinding()  const { return _colorBinding; }

  void                    init ( const TokenType &token );

protected:

  TokenType _token;

  VertexBinding _colorBinding;
  
  Origins     _origins;
  Parameters  _parameters;
  Colors      _colors;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template <PDTA> inline DbBasePrimitiveData<PDTP>::DbBasePrimitiveData ( const TokenType &token ) : 
  _token         ( token ),
  _colorBinding  ( BINDING_UNDETERMINED )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

template <PDTA> inline void DbBasePrimitiveData<PDTP>::init ( const TokenType &token )
{
  _token = token;

  _colorBinding  = BINDING_UNDETERMINED;

  _origins.clear();
  _parameters.clear();
  _colors.clear();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the bindings.
//
///////////////////////////////////////////////////////////////////////////////

template <class Binding, class PartitionedVector> 
inline bool calculateBindings ( const PartitionedVector &attribute, 
                                Binding &binding )
{
  // Initialize.
  binding = BINDING_UNDETERMINED;

  // If we don't have any attributes...
  if ( attribute.getData().empty() )
  {
    binding = BINDING_OFF;
    return true;
  }

  // If we have overall binding...
  else if ( 1 == attribute.getData().size() )
  {
    binding = BINDING_OVERALL;
    return true;
  }

  // Otherwise...
  else
  {
    SL_ASSERT ( 0 ); // Why did this happen?
    return false;
  }

  // Did it work?
  return BINDING_UNDETERMINED != binding;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the bindings.
//
///////////////////////////////////////////////////////////////////////////////

template <PDTA> inline bool DbBasePrimitiveData<PDTP>::calculateBindings()
{
  // Initialize.
  _colorBinding  = BINDING_UNDETERMINED;

  if ( false == CadKit::calculateBindings ( _colors, _colorBinding ) )
    return false;

  // It worked.
  return true;
}

}; // namespace CadKit

#endif // _CADKIT_DATABASE_BASE_LIBRARY_PRIMITIVE_DATA_CLASS_H_
