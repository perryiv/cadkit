
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach, Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Core/SharedVector.h"
#include "VRV/Core/VprIO.h"

#include "vpr/IO/ObjectReader.h"
#include "vpr/IO/ObjectWriter.h"

using namespace VRV::Core;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template < class T >
SharedVector<T>::SharedVector() : 
  BaseClass(),
  _vector( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////
template < class T >
SharedVector<T>::~SharedVector()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the matrix.
//
///////////////////////////////////////////////////////////////////////////////

template < class T >
RETURN_TYPE SharedVector<T>::readObject ( vpr::ObjectReader *reader )
{
  // Clear what we have.
  _vector.clear();

  // Get how many elements there are.
  SizeType size ( 0 );
  ReaderWriter<SizeType>::read ( reader, size );

  // Make enough room.
  _vector.reserve ( size );

  // Read the values.
  for ( SizeType i = 0; i < size; ++ i )
  {
    ValueType value;
    ReaderWriter<T>::read ( writer, value );
    _vector.push_back ( value );
  }
  

#if __VPR_version < 1001005
  return vpr::ReturnStatus::Succeed;
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the matrix.
//
///////////////////////////////////////////////////////////////////////////////

template < class T >
RETURN_TYPE SharedVector<T>::writeObject ( vpr::ObjectWriter *writer )
{
  // Write the size.
  ReaderWriter<SizeType>::write ( writer, _vector.size() );

  for ( ConstIterator iter = _vector.begin(); iter != _vector.end(); ++iter )
    ReaderWriter<T>::write ( writer, *iter );

#if __VPR_version < 1001005
  return vpr::ReturnStatus::Succeed;
#endif
}
