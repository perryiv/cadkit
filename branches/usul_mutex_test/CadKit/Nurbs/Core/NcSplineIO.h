
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NcSplineIO: Input/output functions for NcSpline.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_NURBS_CORE_LIBRARY_SPLINE_IO_FUNCTIONS_H_
#define _CADKIT_NURBS_CORE_LIBRARY_SPLINE_IO_FUNCTIONS_H_

#include "NcSpline.h"

#include "Standard/SlTypedefs.h"

#include <iostream>


namespace CadKit {


///////////////////////////////////////////////////////////////////////////////
//
//  Write the spline to the output stream.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline void write ( const CadKit::NcSpline<NCSDCA> &spline, std::ostream &out )
{
  const char *rational = ( spline.isRational() ) ? "yes" : "no";

  out << "Size of IndexType ..................... " << spline.getNumBytesIndexType() << std::endl;
  out << "Size of ParamType ..................... " << spline.getNumBytesParamType() << std::endl;
  out << "Size of CtrPtType ..................... " << spline.getNumBytesCtrPtType() << std::endl;
  out << "Number of independent variables ....... " << spline.getNumIndepVars()      << std::endl;
  out << "Number of dependent variables ......... " << spline.getNumDepVars()        << std::endl;
  out << "Dimension ............................. " << spline.getDimension()         << std::endl;
  out << "Rational .............................. " << rational                      << std::endl;
  out << "Total number of knots ................. " << spline.getTotalNumKnots()     << std::endl;
  out << "Total number of control points ........ " << spline.getTotalNumCtrPts()    << std::endl;

  IndexType i ( 0 ), j ( 0 );

  for ( i = 0; i < spline.getNumIndepVars(); ++i )
  {
    out << "For independent variable .............. " << i                         << std::endl;
    out << "    Number of knots ................... " << spline.getNumKnots ( i )  << std::endl;
    out << "    Number of control points .......... " << spline.getNumCtrPts ( i ) << std::endl;
    out << "    Order ............................. " << spline.getOrder ( i )     << std::endl;
    out << "    Degree ............................ " << spline.getDegree ( i )    << std::endl;
    out << "    Knot vector:"                                                      << std::endl;

    for ( j = 0; j < spline.getNumKnots ( i ); ++j )
    {
      out << "    " << j << ": " << spline.getKnot ( i, j ) << std::endl;
    }
  }

  out << "Homogeneous control points ............ " << rational << std::endl;

  for ( i = 0; i < spline.getNumDepVars(); ++i )
  {
    out << "Dependent variable " << i << std::endl;

    for ( j = 0; j < spline.getTotalNumCtrPts(); ++j ) 
    {
      out << "    " << j << ": " << spline.getCtrPt ( i, j ) << std::endl;
    }
  }
}


}; // namespace CadKit


///////////////////////////////////////////////////////////////////////////////
//
//  Output operator. Keep in global namespace.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline std::ostream &operator << ( std::ostream &out, const CadKit::NcSpline<NCSDCA> &spline )
{
  CadKit::write ( spline, out );
  return out;
}


#endif // _CADKIT_NURBS_CORE_LIBRARY_SPLINE_IO_FUNCTIONS_H_
