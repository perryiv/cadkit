
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Algorithms to split extents.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_CORE_ALGORITHMS_SPLIT_EXTENTS_H_
#define _MINERVA_CORE_ALGORITHMS_SPLIT_EXTENTS_H_


namespace Minerva {
namespace Core {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Split once.
//
///////////////////////////////////////////////////////////////////////////////

template < class ExtentsType >
inline void split ( const ExtentsType &extents, ExtentsType &ll, ExtentsType &lr, ExtentsType &ul, ExtentsType &ur )
{
  typedef ExtentsType Extents;
  typedef typename Extents::Vertex Vertex;
  typedef typename Extents::ValueType ValueType;

  const Vertex &mn ( extents.minimum() );
  const Vertex &mx ( extents.maximum() );
  const Vertex md ( ( mx + mn ) * static_cast < ValueType > ( 0.5 ) );

  ll = Extents ( Vertex ( mn[0], mn[1] ), Vertex ( md[0], md[1] ) );
  lr = Extents ( Vertex ( md[0], mn[1] ), Vertex ( mx[0], md[1] ) );
  ul = Extents ( Vertex ( mn[0], md[1] ), Vertex ( md[0], mx[1] ) );
  ur = Extents ( Vertex ( md[0], md[1] ), Vertex ( mx[0], mx[1] ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Split extents one or more times.
//
///////////////////////////////////////////////////////////////////////////////

template < class ExtentsType, class ContainerType >
inline void split ( const ExtentsType &extents, ContainerType &answer, unsigned int numTimesToSplit )
{
  typedef ExtentsType Extents;
  typedef typename Extents::Vertex Vertex;
  typedef typename Extents::ValueType ValueType;
  typedef ContainerType Container;

  switch ( numTimesToSplit )
  {
    case 0:

      // This is done to support algorithms that call this function.
      answer.insert ( answer.end(), extents );
      break;

    case 1:

      // Just split once.
      {
        Extents ll, lr, ul, ur;
        Minerva::Core::Algorithms::split ( extents, ll, lr, ul, ur );

        answer.insert ( answer.end(), ll );
        answer.insert ( answer.end(), lr );
        answer.insert ( answer.end(), ul );
        answer.insert ( answer.end(), ur );
      }
      break;

    default:

      {
        // Split once.
        Container quarters;
        Minerva::Core::Algorithms::split ( extents, quarters, 1 );

        // Loop through the extents from a single split.
        for ( typename Container::const_iterator i = quarters.begin(); i != quarters.end(); ++i )
        {
          // Split again.
          const Extents &quarter ( *i );
          Minerva::Core::Algorithms::split ( quarter, answer, numTimesToSplit - 1 );
        }
      }
      break;
  }
}

  
} // namespace Algorithms
} // namespace Core
} // namespace Minerva


#endif // _MINERVA_CORE_ALGORITHMS_SPLIT_EXTENTS_H_
