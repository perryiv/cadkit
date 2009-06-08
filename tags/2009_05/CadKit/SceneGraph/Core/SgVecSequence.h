
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgVecSequence.h: Template class used for managing a sequence of vectors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_VECTOR_SEQUENCE_TEMPLATE_CLASS_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_VECTOR_SEQUENCE_TEMPLATE_CLASS_H_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <vector>
#endif

#define SG_VSTA class Vec, class Index
#define SG_VSFA Vec, Index


namespace CadKit
{
template<SG_VSTA> class SgVecSequence
{
public:

  typedef std::vector<Vec> Vectors;

  SgVecSequence(){}

  void                  clear() { _vectors.clear(); }
  Index                 getNumElements() const { return static_cast<Index> ( _vectors.size() ); }
  bool                  isEqual ( const SgVecSequence &sequence ) const { return _vectors == sequence._vectors; }

  bool					        setNumElements ( const Index &numElements );
	bool					        setElement ( const Vec &vec, const Index &index );

protected:

  Vectors _vectors;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of elements.
//
///////////////////////////////////////////////////////////////////////////////

template<SG_VSTA> inline bool SgVecSequence<SG_VSFA>::setNumElements ( const Index &numElements )
{
  // The docs say that resize() throws an exception if it fails.
  try
  {
    _vectors.resize ( numElements );
    return true;
  }

  catch ( ... )
  {
    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the vector. If the index is out of range then it fails.
//
///////////////////////////////////////////////////////////////////////////////

template<SG_VSTA> inline bool SgVecSequence<SG_VSFA>::setElement ( const Vec &vec, const Index &index )
{
  // If we are in range...
  if ( index < this->getNumElements() )
  {
    // Set the vector and return.
    _vectors[index] = vec;
    return true;
  }

  // Out of range.
  return false;
}


}; // namespace CadKit;

#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_VECTOR_SEQUENCE_TEMPLATE_CLASS_H_
