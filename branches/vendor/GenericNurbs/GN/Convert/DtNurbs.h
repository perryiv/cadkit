
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  DT_NURBS conversions.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_CONVERT_DT_NURBS_H_
#define _GENERIC_NURBS_LIBRARY_CONVERT_DT_NURBS_H_


namespace GN {
namespace Convert {
namespace DtNurbs {


///////////////////////////////////////////////////////////////////////////////
///
///  Read the DT_NURBS C-array and create the spline.
///
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class SplineType > struct Setter
  {
    typedef typename SplineType::SplineClass SplineClass;
    typedef typename SplineType::ErrorCheckerType ErrorCheckerType;
    typedef typename SplineType::UIntType UIntType;
    typedef typename SplineType::UIntContainer UIntContainer;
    typedef typename SplineType::KnotType KnotType;
    typedef typename SplineType::ControlPointType ControlPointType;
    typedef typename SplineType::Limits Limits;

    static void set ( const double *Carray, SplineClass &s )
    {
      GN_ERROR_CHECK ( NULL != Carray );

      // Set the integer data.
      const UIntType numIndepVars = UIntType ( Carray[0] );
      const bool rational = ( Carray[1] < 0 ) ? true : false;
      const UIntType numDepVars = UIntType ( ( rational ) ? -Carray[1] : Carray[1] );

      // Make sure...
      GN_ERROR_CHECK ( numIndepVars >= Limits::MIN_NUM_INDEP_VARS );
      GN_ERROR_CHECK ( numDepVars   >= Limits::minNumDepVars ( rational ) );

      // Set the dimension.
      const UIntType dimension ( ( rational ) ? numDepVars - 1 : numDepVars );
      GN_ERROR_CHECK ( dimension >= Limits::MIN_NUM_DIMENSIONS );

      // Declare the containers.
      UIntContainer order, numCtrPts;
      order.resize     ( numIndepVars );
      numCtrPts.resize ( numIndepVars );

      // Shortcuts.
      const double *so = &Carray[2];
      const double *sn = so + numIndepVars;

      // Loop through the independent variables.
      {
        for ( UIntType i = 0; i < numIndepVars; ++i )
        {
          order[i]     = UIntType ( so[i] );
          numCtrPts[i] = UIntType ( sn[i] );
        }
      }

      // Resize the spline.
      s.resize ( dimension, order, numCtrPts, rational );

      // Shortcut to the knots.
      const double *knots = sn + numIndepVars + numIndepVars;

      // Assign the knots.
      {
        // Loop through the independent variables.
        for ( UIntType i = 0; i < numIndepVars; ++i )
        {
          // Get the number of knots.
          UIntType numKnots ( s.numKnots ( i ) );

          // Loop through all the knots.
          for ( UIntType j = 0; j < numKnots; ++j )
          {
            // Assign the knots.
            s.knot(i,j) = KnotType ( knots[j] );
          }

          // Move the shortcut.
          knots = &knots[numKnots];
        }
      }

      // Shortcut to the control points.
      const double *cp = knots;

      // Needed below.
      UIntType totalCtrPts ( s.totalNumControlPoints() );

      // Assign the control points.
      {
        // Loop through all the dependent variables.
        for ( UIntType i = 0; i < numDepVars; ++i )
        {
          // Loop through all the control points.
          for ( UIntType j = 0; j < totalCtrPts; ++j )
          {
            // Assign the control point value.
            s.controlPoint(i,j) = ControlPointType ( cp[j] );
          }

          // Move the shortcut.
          cp = &cp[totalCtrPts];
        }
      }
    }
  };
}; // namespace Detail


///////////////////////////////////////////////////////////////////////////////
///
///  Read the DT_NURBS C-array and create the spline.
///
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > inline void set ( const double *Carray, SplineType &s )
{
  Detail::Setter<SplineType>::set ( Carray, s );
}


}; // namespace DtNurbs
}; // namespace Convert
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_CONVERT_DT_NURBS_H_
