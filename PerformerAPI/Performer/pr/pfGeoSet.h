#ifndef _PF_GEO_SET_H_
#define _PF_GEO_SET_H_

#include "Performer/pr/pfGeoState.h"
#include "Performer/pr/pfLinMath.h"

#include "Standard/SlArrayPtr.h"

#define PFGS_LINESTRIPS     1034807718
#define PFGS_POINTS         1034807719
#define PFGS_POLYS          1034807720
#define PFGS_TRIFANS        1034807721
#define PFGS_TRISTRIPS      1034807722

#define PFGS_COORD3         1034807723
#define PFGS_NORMAL3        1034807724
#define PFGS_COLOR4         1034807725
#define PFGS_TEXCOORD2      1034807726

#define PFGS_OFF            1034807727
#define PFGS_OVERALL        1034807728
#define PFGS_PER_PRIM       1034807729
#define PFGS_PER_VERTEX     1034807730


class pfGeoSet : public pfObject
{
public:

  pfGeoSet();

  virtual const char *          className() const { return "pfGeoSet"; }

  void                          setAttr ( int attribute, int binding, void *attributeList, unsigned short *indexList );
  void                          setGState ( pfGeoState *state ) { _state = state; }
  void                          setNumPrims ( int numPrims ) { _numPrims = numPrims; }
  void                          setPrimType ( int type ) { _type = type; }
  void                          setPrimLengths ( int *lengths ) { _lengths = lengths; }

  virtual void                  write ( std::ostream &out ) const;

protected:

  int _numPrims;
  int _type;
  CadKit::SlArrayPtr<int> _lengths;
  CadKit::SlArrayPtr<pfVec3> _vertices;
  CadKit::SlArrayPtr<pfVec3> _normals;
  CadKit::SlArrayPtr<pfVec4> _colors;
  CadKit::SlArrayPtr<pfVec2> _texCoords;
  CadKit::SlRefPtr<pfGeoState> _state;

  virtual ~pfGeoSet(){}

  std::string       _getTypeAsString() const;
};


inline pfGeoSet::pfGeoSet() : pfObject(),
  _numPrims ( 0 ),
  _type ( 0 ),
  _lengths ( 0x0 ),
  _vertices ( 0x0 ),
  _normals ( 0x0 ),
  _colors ( 0x0 ),
  _texCoords ( 0x0 ),
  _state ( 0x0 )
{
  // Empty.
}


inline void pfGeoSet::setAttr ( int attribute, int binding, void *attributeList, unsigned short *indexList )
{
  SL_ASSERT ( 0x0 == indexList );

  switch ( attribute )
  {
  case PFGS_COORD3:
    _vertices = ((pfVec3 *) attributeList);
    SL_ASSERT ( PFGS_PER_VERTEX == binding );
    break;
  case PFGS_NORMAL3:
    _normals = ((pfVec3 *) attributeList);
    SL_ASSERT ( PFGS_PER_VERTEX == binding || PFGS_PER_PRIM == binding || PFGS_OVERALL == binding );
    break;
  case PFGS_COLOR4:
    _colors = ((pfVec4 *) attributeList);
    SL_ASSERT ( PFGS_PER_VERTEX == binding || PFGS_PER_PRIM == binding || PFGS_OVERALL == binding );
    break;
  case PFGS_TEXCOORD2:
    _texCoords = ((pfVec2 *) attributeList);
    SL_ASSERT ( PFGS_PER_VERTEX == binding );
    break;
  default:
    SL_ASSERT ( 0 ); // Heads up.
    break;
  }
}


namespace CadKit
{
template<class ArrayType> inline void write1d ( const int &size, const ArrayType &array, std::ostream &out )
{
  for ( int i = 0; i < size; ++i )
    out << array[i] << " " << std::flush;
}
};


inline void pfGeoSet::write ( std::ostream &out ) const
{
  out << _indent << this->className() << "::_numPrims = " << _numPrims << std::endl;
  out << _indent << this->className() << "::_type     = " << this->_getTypeAsString() << std::endl;

//  out << _indent << this->className() << "::_lengths  = " << std::flush;
//  CadKit::write1d ( _numPrims, _lengths, out );
//  out << std::endl;

  if ( _state.isValid() )
    _state->write ( out );

  pfObject::write ( out );
}


inline std::string pfGeoSet::_getTypeAsString() const
{
  switch ( _type )
  {
  case PFGS_LINESTRIPS: return "LineStrip";
  case PFGS_POINTS:     return "Points";
  case PFGS_POLYS:      return "Polygons";
  case PFGS_TRIFANS:    return "TriFans";
  case PFGS_TRISTRIPS:  return "TriStrips";
  default:
    SL_ASSERT ( 0 ); // Heads up.
    return "Undefined";
  }
}


#endif
