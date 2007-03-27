
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbPfArraySetters: Implementations of the abstract array-setting class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_PERFORMER_LIBRARY_ARRAY_SETTERS_H_
#define _CADKIT_DATABASE_PERFORMER_LIBRARY_ARRAY_SETTERS_H_

#ifdef _MSC_VER
# ifndef _CADKIT_COMPILING_DATABASE_PERFORMER_LIBRARY
#  error "This header file is intended for internal Database/Performer code only."
# endif
#endif

#include "DbPfFunctions.h"

#include "Interfaces/IQueryVertices.h"
#include "Interfaces/IQueryNormals.h"
#include "Interfaces/IQueryColors.h"
#include "Interfaces/IQueryTexCoords.h"

#include "Standard/SlVec2.h"
#include "Standard/SlVec3.h"
#include "Standard/SlVec4.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <vector>
#endif

struct pfVec4;
struct pfVec3;
struct pfVec2;


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  DbPfVertexSetter.
//
///////////////////////////////////////////////////////////////////////////////

class DbPfVertexSetter : public IQueryShapeVerticesVec3f::VertexSetter
{
public:

  DbPfVertexSetter ( const VertexSetType &type ) : 
    _type ( CadKit::getPrimitiveType ( type ) ), 
    _numVertices ( 0 ), 
    _vertices ( NULL ){}

  pfVec3 *              getVertices()               { return _vertices; }
  int                   getNumPrimitives()    const { return (int) _ranges.size(); }
  int *                 getPrimitiveLengths() const; // Allocates memory.
  int                   getPrimitiveType()    const { return (int) _type; }

  virtual bool          setData ( const unsigned int &index, const SlVec3f &vec );
  virtual bool          setSize ( const unsigned int &size );

  virtual bool          setNumPrimitives  ( const unsigned int &num );
  virtual bool          setPrimitiveRange ( const unsigned int &index, 
                                            const unsigned int &start, 
                                            const unsigned int &length );

protected:

  int _type;
  unsigned int _numVertices;
  pfVec3 *_vertices;
  std::vector<SlVec2i> _ranges;
};


///////////////////////////////////////////////////////////////////////////////
//
//  DbPfNormalSetter.
//
///////////////////////////////////////////////////////////////////////////////

class DbPfNormalSetter : public IQueryShapeNormalsVec3f::NormalSetter
{
public:
  
  DbPfNormalSetter ( const VertexBinding &binding ) : 
    _binding ( CadKit::getBinding ( binding ) ), 
    _numNormals ( 0 ), 
    _normals ( NULL ){}

  pfVec3 *              getNormals()       { return _normals; }
  int                   getBinding() const { return _binding; }

  virtual bool          setData ( const unsigned int &index, const SlVec3f &vec );
  virtual bool          setSize ( const unsigned int &size );

protected:

  int _binding;
  unsigned int _numNormals;
  pfVec3 *_normals;
};


///////////////////////////////////////////////////////////////////////////////
//
//  DbPfColorSetter.
//
///////////////////////////////////////////////////////////////////////////////

class DbPfColorSetter : public IQueryShapeColorsVec4f::ColorSetter
{
public:
  
  DbPfColorSetter ( const VertexBinding &binding ) : 
    _binding ( CadKit::getBinding ( binding ) ), 
    _numColors ( 0 ), 
    _colors ( NULL ){}

  pfVec4 *              getColors()        { return _colors; }
  int                   getBinding() const { return _binding; }

  virtual bool          setData ( const unsigned int &index, const SlVec4f &vec );
  virtual bool          setSize ( const unsigned int &size );

protected:

  int _binding;
  unsigned int _numColors;
  pfVec4 *_colors;
};


///////////////////////////////////////////////////////////////////////////////
//
//  DbPfTextureCoordSetter.
//
///////////////////////////////////////////////////////////////////////////////

class DbPfTextureCoordSetter : public IQueryShapeTexCoordsVec2f::TexCoordSetter
{
public:
  
  DbPfTextureCoordSetter() : 
    _binding ( CadKit::getBinding ( CadKit::BINDING_PER_VERTEX ) ), 
    _numTexCoords ( 0 ), 
    _texCoords ( NULL ){}

  pfVec2 *              getTextureCoords()       { return _texCoords; }
  int                   getBinding()       const { return _binding; }

  virtual bool          setData ( const unsigned int &index, const SlVec2f &vec );
  virtual bool          setSize ( const unsigned int &size );

protected:

  int _binding;
  unsigned int _numTexCoords;
  pfVec2 *_texCoords;
};


}; // namespace CadKit


#endif // _CADKIT_DATABASE_PERFORMER_LIBRARY_ARRAY_SETTERS_H_
