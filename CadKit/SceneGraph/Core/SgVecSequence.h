
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
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
