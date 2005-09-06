
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The knot-insertion algorithm.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_KNOT_INSERTION_ALGORITHM_H_
#define _GENERIC_NURBS_LIBRARY_KNOT_INSERTION_ALGORITHM_H_

#include "GN/Macros/ErrorCheck.h"


namespace GN {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Start of namespace Detail.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct for finding the knot span.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > struct KnotInserter
{
  typedef typename SplineType::ErrorCheckerType ErrorCheckerType;
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::IndependentType IndependentType;
  typedef typename SplineType::IndependentArgument IndependentArgument;
  typedef typename SplineType::Limits Limits;


  /////////////////////////////////////////////////////////////////////////////
  //
  //	See if you can insert the given knot the specified number of times.
  //  This function assumes that the given spline is valid. If the answer 
  //  is "no" then the reason is written to the given string "reason".
  //
  /////////////////////////////////////////////////////////////////////////////

  static bool canInsert ( 
    const SplineType &s, 
    SizeType whichIndepVar, 
    IndependentArgument newKnot, 
    SizeType numTimes, 
    std::string &reason )
  {
    // The current curve has to be valid. This is just a partial check. 
    // The client can call Spline::check() for a comprehensive test.
    if ( s.numIndepVars() < Limits::MIN_NUM_INDEP_VARS ) 
    {
      std::ostringstream out;
      out << "Inserting the new knot '" 
          << newKnot 
          << "' is not possible because the number of independent variables is " 
          << s.numIndepVars()
          << '.';
      reason = out.str();
      return false;
    }

    // New knot has to be in range.
    IndependentType first ( s.firstKnot ( whichIndepVar ) );
    if ( newKnot <= first )
    {
      std::ostringstream out;
      out << "New knot '" 
          << newKnot 
          << "' would be "
          << ( ( newKnot <= first ) ? "less than" : "equal to" )
          << " the current first knot.";
      reason = out.str();
      return false;
    }

    // New knot has to be in range.
    IndependentType last ( s.lastKnot ( whichIndepVar ) );
    if ( newKnot >= last )
    {
      std::ostringstream out;
      out << "New knot '" 
          << newKnot 
          << "' would be "
          << ( ( newKnot <= first ) ? "less than" : "equal to" )
          << " the current last knot.";
      reason = out.str();
      return false;
    }

    // Get the current multiplicity.
    SizeType currentMultiplicity ( s.knotMultiplicity ( whichIndepVar, newKnot ) );

    // After inserting, we can have a multiplicity of degree, but no more.
    if ( numTimes + currentMultiplicity > s.degree ( whichIndepVar ) )
    {
      std::ostringstream out;
      out << "Inserting knot '" 
          << newKnot 
          << "' " 
          << numTimes 
          << ( ( 1 == numTimes ) ? " time " : " times " )
          << "will exceed maximum allowable knot multiplicity of " 
          << s.degree ( whichIndepVar ) 
          << " (the degree).";
      reason = out.str();
      return false;
    }

    return true;
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  End of namespace Detail.
//
///////////////////////////////////////////////////////////////////////////////

};


///////////////////////////////////////////////////////////////////////////////
//
//  See if you can insert the given knot the specified number of times.
//  This function assumes that the given spline is valid. If the answer 
//  is "no" then the reason is written to the given string "reason".
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > inline
bool canInsertKnot ( const SplineType &spline, 
                     typename SplineType::SizeType whichIndepVar, 
                     typename SplineType::IndependentArgument newKnot, 
                     typename SplineType::SizeType numTimes,
                     std::string &reason )
{
  typedef typename SplineType::SplineClass SplineClass;
  return Detail::KnotInserter<SplineClass>::canInsert ( spline, whichIndepVar, newKnot, numTimes, reason );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if you can insert the given knot the specified number of times.
//  This function assumes that the given spline is valid.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > inline
bool canInsertKnot ( const SplineType &spline, 
                     typename SplineType::SizeType whichIndepVar, 
                     typename SplineType::IndependentArgument newKnot, 
                     typename SplineType::SizeType numTimes )
{
  std::string reason;
  return canInsertKnot ( spline, whichIndepVar, newKnot, numTimes, reason );
}


}; // namespace Algorithms
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_KNOT_INSERTION_ALGORITHM_H_
