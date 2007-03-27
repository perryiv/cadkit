
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbBaseShapeData: Holds data that defines a shape. Associates this data
//  with a generic token.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_BASE_LIBRARY_SHAPE_DATA_CLASS_H_
#define _CADKIT_DATABASE_BASE_LIBRARY_SHAPE_DATA_CLASS_H_

#include "Interfaces/Enum.h"

#include "Standard/SlPartitionedVector.h"
#include "Standard/SlAssert.h"

// To help shorten up the lines.
#define SDTA class VertexType, class NormalType, class ColorType, class TexCoordType, class TokenType
#define SDTP VertexType, NormalType, ColorType, TexCoordType, TokenType


namespace CadKit
{
template <SDTA> class DbBaseShapeData
{
public:

  typedef SlPartitionedVector < unsigned int, unsigned int, VertexType >   Vertices;
  typedef SlPartitionedVector < unsigned int, unsigned int, NormalType >   Normals;
  typedef SlPartitionedVector < unsigned int, unsigned int, ColorType >    Colors;
  typedef SlPartitionedVector < unsigned int, unsigned int, TexCoordType > TexCoords;

  DbBaseShapeData ( const TokenType &token );
  ~DbBaseShapeData(){}

  bool                    calculateBindings();

  const TokenType &       getToken() const { return _token; }
  void                    setToken ( const TokenType &token ) { _token = token; }

  const Vertices &        getVertices()  const { return _vertices; }
  const Normals &         getNormals()   const { return _normals; }
  const Colors &          getColors()    const { return _colors; }
  const TexCoords &       getTexCoords() const { return _texCoords; }

  Vertices &              getVertices()  { return _vertices; }
  Normals &               getNormals()   { return _normals; }
  Colors &                getColors()    { return _colors; }
  TexCoords &             getTexCoords() { return _texCoords; }

  const VertexBinding &   getNormalBinding() const { return _normalBinding; }
  const VertexBinding &   getColorBinding()  const { return _colorBinding; }

  void                    init ( const TokenType &token );

protected:

  TokenType _token;
  
  VertexBinding _normalBinding;
  VertexBinding _colorBinding;

  Vertices  _vertices;
  Normals   _normals;
  Colors    _colors;
  TexCoords _texCoords;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template <SDTA> inline DbBaseShapeData<SDTP>::DbBaseShapeData ( const TokenType &token ) : 
  _token         ( token ),
  _normalBinding ( BINDING_UNDETERMINED ),
  _colorBinding  ( BINDING_UNDETERMINED )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

template <SDTA> inline void DbBaseShapeData<SDTP>::init ( const TokenType &token )
{
  _token = token;

  _normalBinding = BINDING_UNDETERMINED;
  _colorBinding  = BINDING_UNDETERMINED;

  _vertices.clear();
  _normals.clear();
  _colors.clear();
  _texCoords.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the bindings.
//
///////////////////////////////////////////////////////////////////////////////

template <class Binding, class PartitionedVector> 
inline bool calculateBindings ( const PartitionedVector &vertices, 
                                const PartitionedVector &attribute, 
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

  // If we have per-vertex binding...
  else if ( vertices.getData().size() == attribute.getData().size() )
  {
    binding = BINDING_PER_VERTEX;
    return true;
  }

  // If we have per-primitive binding...
  else if ( vertices.getSizes().size() == attribute.getData().size() )
  {
    binding = BINDING_PER_PRIMITIVE;
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

template <SDTA> inline bool DbBaseShapeData<SDTP>::calculateBindings()
{
  // Initialize.
  _normalBinding = BINDING_UNDETERMINED;
  _colorBinding  = BINDING_UNDETERMINED;

  // If there aren't any vertices...
  if ( _vertices.getData().empty() )
    return false;

  // Calculate the normal binding.
  if ( false == CadKit::calculateBindings ( _vertices, _normals, _normalBinding ) )
    return false;

  // Calculate the color binding.
  if ( false == CadKit::calculateBindings ( _vertices, _colors, _colorBinding ) )
    return false;

  // It worked.
  return true;
}


}; // namespace CadKit


#endif // _CADKIT_DATABASE_BASE_LIBRARY_SHAPE_DATA_CLASS_H_
