
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbBaseInline: Inline functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_BASE_INLINE_FUNCTIONS_DATABASE_H_
#define _CADKIT_DATABASE_BASE_INLINE_FUNCTIONS_DATABASE_H_


#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <map>
#endif


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Find the data associated with the key.
//
///////////////////////////////////////////////////////////////////////////////

template <class KeyType, class MapType> 
inline const void *find ( const KeyType key, const MapType &theMap )
{
  // See if it's in our map.
	MapType::const_iterator i = theMap.find ( key );

  // Return the result (which may be null).
  return ( i != theMap.end() ) ? i->second : NULL;
}


}; // namespace CadKit


#endif // _CADKIT_DATABASE_BASE_INLINE_FUNCTIONS_DATABASE_H_
