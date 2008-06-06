
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Like std::copy but with a predicate. Adapted from:
//  http://www.comms.scitech.susx.ac.uk/fft/books/ThinkingInCpp_2ndEdition_vol2_Example_Codes/C05/copy_if.h
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ALGORITHMS_COPY_IF_H_
#define _USUL_ALGORITHMS_COPY_IF_H_

#include <algorithm>


namespace Usul {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  This one will write to "output".
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ForwardIterator, 
  class OutputIterator,
  class UnaryPredicate
>
OutputIterator copyIf ( ForwardIterator start, ForwardIterator stop, OutputIterator output, UnaryPredicate pred )
{
  while ( start != stop )
  {
    if ( pred ( *start ) )
    {
      *output = *start;
      ++output;
    }
    ++start;
  }
  return output;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns a copy.
//
///////////////////////////////////////////////////////////////////////////////

template < class OutputContainer > struct GetCopy
{
  typedef typename OutputContainer::iterator OutputIterator;
  template < class ForwardIterator, class UnaryPredicate > 
  static OutputContainer copyIf ( ForwardIterator start, ForwardIterator stop, UnaryPredicate pred )
  {
    OutputContainer out;
    Usul::Algorithms::copyIf ( start, stop, std::back_inserter < OutputContainer > ( out ), pred );
    return out;
  }
};


}; // namespace Algorithms
}; // namespace Usul


#endif // _USUL_ALGORITHMS_COPY_IF_H_
