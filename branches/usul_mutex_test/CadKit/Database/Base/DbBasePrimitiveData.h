
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

  const TokenType &       getToken() const { return _token; }
  void                    setToken ( const TokenType &token ) { _token = token; }

  const Origins &         getOrigins()  const { return _origins; }
  const Parameters &      getParameters()   const { return _parameters; }
  const Colors &          getColors()    const { return _colors; }

  Origins &               getOrigins()  { return _origins; }
  Parameters &            getParameters()   { return _parameters; }
  Colors &                getColors()    { return _colors; }

  void                    init ( const TokenType &token );

protected:

  TokenType _token;
  
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
  _token         ( token )
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

  _origins.clear();
  _parameters.clear();
  _colors.clear();
}

}; // namespace CadKit

#endif // _CADKIT_DATABASE_BASE_LIBRARY_PRIMITIVE_DATA_CLASS_H_
