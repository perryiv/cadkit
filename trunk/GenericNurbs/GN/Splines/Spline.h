
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all NURBS entities.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_SPLINE_CLASS_H_
#define _GENERIC_NURBS_LIBRARY_SPLINE_CLASS_H_

#include "GN/Limits/Nurbs.h"
#include "GN/Traits/Arguments.h"
#include "GN/Traits/TypeTags.h"
#include "GN/Macros/ErrorCheck.h"
#include "GN/Algorithms/Equal.h"
#include "GN/Work/Container.h"

#include <algorithm>
#include <numeric>
#include <functional>


namespace GN {
namespace Splines {


template < class ConfigType_ > class Spline : public ConfigType_::BaseClassType
{
public:

  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Useful typedefs.
  ///
  /////////////////////////////////////////////////////////////////////////////

  typedef ConfigType_                                             ConfigType;
  typedef Spline < ConfigType >                                   ThisType;
  typedef typename ConfigType::BaseClassType                      BaseClass;
  typedef typename ConfigType::UIntType                           UIntType;
  typedef typename ConfigType::KnotType                           KnotType;
  typedef typename ConfigType::ControlPointType                   ControlPointType;
  typedef typename ConfigType::UIntContainer                      UIntContainer;
  typedef typename ConfigType::KnotContainer                      KnotContainer;
  typedef typename KnotContainer::value_type                      KnotVector;
  typedef typename ConfigType::ControlPointContainer              ControlPointContainer;
  typedef typename ConfigType::ErrorCheckerType                   ErrorCheckerType;
  typedef typename UIntContainer::size_type                       UIntContainerSizeType;
  typedef typename KnotContainer::size_type                       KnotVectorSizeType;
  typedef typename ControlPointContainer::size_type               ControlPointSizeType;
  typedef typename KnotContainer::value_type                      KnotVector;
  typedef typename ControlPointContainer::value_type              DepVarContainer;
  typedef typename ConfigType::Vec2                               Vec2;
  typedef typename ConfigType::Vec3                               Vec3;
  typedef typename ConfigType::Vec4                               Vec4;
  typedef typename ConfigType::Matrix44                           Matrix44;
  typedef typename ConfigType::KnotTester                         KnotTester;
  typedef typename ConfigType::ControlPointTester                 ControlPointTester;
  typedef typename ConfigType::Translation                        Translation;
  typedef typename ConfigType::Scale                              Scale;
  typedef typename ConfigType::Multiply                           Multiply;
  typedef typename ConfigType::SquareRoot                         SquareRoot;
  typedef typename ConfigType::Power                              Power;
  typedef typename ConfigType::Vector                             Vector;
  typedef typename GN::Traits::Argument<ControlPointType>::Type   ControlPointArgument;
  typedef typename GN::Traits::Argument<KnotType>::Type           KnotArgument;
  typedef GN::Limits::Nurbs<UIntType>                             Limits;
  typedef GN::Traits::Spline                                      TypeTag;
  typedef ThisType                                                SplineClass;
  typedef GN::Work::Container<Vector,ErrorCheckerType>            WorkSpace;


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Default constructor.
  ///
  /////////////////////////////////////////////////////////////////////////////

  Spline() : BaseClass(),
    _order        (),
    _numCtrPts    (),
    _rational     ( false ),
    _knots        (),
    _ctrPts       (),
    _work         ()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Copy constructor.
  ///
  /////////////////////////////////////////////////////////////////////////////

  Spline ( const Spline &s ) : BaseClass ( s ),
    _order        ( s._order ),
    _numCtrPts    ( s._numCtrPts ),
    _rational     ( s._rational ),
    _knots        ( s._knots ), 
    _ctrPts       ( s._ctrPts ),
    _work         ( s._work )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Destructor.
  ///
  /////////////////////////////////////////////////////////////////////////////

  GN_DESTRUCTOR_TYPE ~Spline()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Ensure public scope because GN_DESTRUCTOR_TYPE is user-defined.
  ///
  /////////////////////////////////////////////////////////////////////////////

public:


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// The number of independent variables. A curve has 1, a surface 2, etc.
  ///
  /////////////////////////////////////////////////////////////////////////////

  UIntType numIndepVars() const
  {
    return _order.size();
  }

  /////////////////////////////////////////////////////////////////////////////
  ///
  /// The number of dependent variables. 
  ///
  /// A non-rational spline (curve, surface, whatever) with x-y-z control 
  /// points has 3 dependent variables. 
  ///
  /// If the spline is rational, and thus has wx-wy-wz-w for control points, 
  /// then there are 4 dependent variables.
  ///
  /////////////////////////////////////////////////////////////////////////////

  UIntType numDepVars() const
  {
    return _ctrPts.size();
  }

  /////////////////////////////////////////////////////////////////////////////
  ///
  /// See if the spline is rational. 
  ///
  /// If there are 3 dependent variables, and the rational flag is true, 
  /// the variables are interpreted as wx-wy-w.
  ///
  /// Alternatively, if the rational flag is false, then the variables are 
  /// interpreted as x-y-z.
  ///
  /////////////////////////////////////////////////////////////////////////////

  bool rational() const
  {
    return _rational;
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the dimension. 
  ///
  /// If the spline is non-rational with x-y-z control points, then the 
  /// dimension is just equal to the number of dependent, in this case 3.
  ///
  /// However, if the spline is rational with wx-wy-wz-w control points, 
  /// then the number of dependent variables is 4, and the dimension is 3.
  ///
  /////////////////////////////////////////////////////////////////////////////

  UIntType dimension() const
  {
    // We check the number of dependent variables also because the spline 
    // may be newly constructed and thus have invalid data.
    UIntType numDepVars ( this->numDepVars() );
    return ( this->rational() && numDepVars > 0 ) ? numDepVars - 1 : numDepVars;
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the order for the specified independent variable.
  ///
  ///   order == degree + 1.
  ///
  /////////////////////////////////////////////////////////////////////////////

  UIntType order ( UIntContainerSizeType whichIndepVar ) const
  {
    GN_ERROR_CHECK ( whichIndepVar < _order.size() );
    return _order[whichIndepVar];
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the degree for the specified independent variable.
  ///
  ///   degree == order - 1.
  ///
  /////////////////////////////////////////////////////////////////////////////

  UIntType degree ( UIntContainerSizeType whichIndepVar ) const
  {
    return ( this->order ( whichIndepVar ) - 1 );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the number of control points for the specified independent variable.
  ///
  /// If the spline is a surface with a 10 x 12 grid of control points, then 
  /// the following is true:
  ///
  ///   10 == numControlPoints ( 0 );
  ///   12 == numControlPoints ( 1 );
  ///
  /////////////////////////////////////////////////////////////////////////////

  UIntType numControlPoints ( UIntContainerSizeType whichIndepVar ) const
  {
    GN_ERROR_CHECK ( whichIndepVar < _numCtrPts.size() );
    return _numCtrPts[whichIndepVar];
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the number of knots for the specified independent variable, which 
  /// is equal to the number of control points plus the order.
  ///
  /// If the spline is a bi-cubic surface with a 10 x 12 grid of control 
  /// points, then the following is true:
  ///
  ///   10 + 4 == numKnots ( 0 );
  ///   12 + 4 == numKnots ( 1 );
  ///
  /////////////////////////////////////////////////////////////////////////////

  UIntType numKnots ( UIntContainerSizeType whichIndepVar ) const
  {
    GN_ERROR_CHECK ( whichIndepVar < _knots.size() );
    return _knots[whichIndepVar].size();
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the total number of control points. 
  ///
  /// This is in contrast to the number of control points for an individual 
  /// independent variables.
  ///
  /// For example, if the spline is a surface with a 10 x 12 grid of control 
  /// points, the total number of control points is 120.
  ///
  /////////////////////////////////////////////////////////////////////////////

  UIntType totalNumControlPoints() const
  {
    // If we have control points then return the size of the first dependent 
    // variable's container.
    return ( !_ctrPts.empty() ) ? _ctrPts[0].size() : 0;

    // Note: call check() to make sure that the size of the remaining 
    // dependent variable containers are all the same.
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the total number of knots. 
  ///
  /// This is in contrast to the number of knots for an individual 
  /// independent variables.
  ///
  /// For example, if the spline is a bi-cubic surface with a 10 x 12 grid of 
  /// control points, then the total number of knots is:
  ///
  ///   ( 10 + 4 ) + ( 12 + 4 ) = 30
  ///
  /////////////////////////////////////////////////////////////////////////////

  UIntType totalNumKnots() const
  {
    // This will sum all the sizes of the individual knot vectors.
    typedef typename KnotContainer::const_iterator Iterator;
    UIntType sum ( 0 );
    for ( Iterator i = _knots.begin(); i != _knots.end(); ++i )
      sum += i->size();
    return sum;
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Read-only access to the knot vector.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const KnotVector &knotVector ( KnotVectorSizeType whichIndepVar ) const
  {
    GN_ERROR_CHECK ( whichIndepVar < _knots.size() );
    return _knots[whichIndepVar];
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the knot.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const KnotType &knot ( KnotVectorSizeType whichIndepVar, KnotVectorSizeType whichKnot ) const
  {
    GN_ERROR_CHECK ( whichIndepVar < _knots.size() );
    GN_ERROR_CHECK ( whichKnot < _knots[whichIndepVar].size() );
    return _knots[whichIndepVar][whichKnot];
  }

  KnotType &knot ( KnotVectorSizeType whichIndepVar, KnotVectorSizeType whichKnot )
  {
    GN_ERROR_CHECK ( whichIndepVar < _knots.size() );
    GN_ERROR_CHECK ( whichKnot < _knots[whichIndepVar].size() );
    return _knots[whichIndepVar][whichKnot];
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the first knot in the specified knot vector.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const KnotType &firstKnot ( KnotVectorSizeType whichIndepVar ) const
  {
    return this->knot ( whichIndepVar, 0 );
  }

  KnotType &firstKnot ( KnotVectorSizeType whichIndepVar )
  {
    return this->knot ( whichIndepVar, 0 );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the last knot in the specified knot vector.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const KnotType &lastKnot ( KnotVectorSizeType whichIndepVar ) const
  {
    UIntType num ( this->numKnots ( whichIndepVar ) );
    return this->knot ( whichIndepVar, num - 1 );
  }

  KnotType &lastKnot ( KnotVectorSizeType whichIndepVar )
  {
    UIntType num ( this->numKnots ( whichIndepVar ) );
    return this->knot ( whichIndepVar, num - 1 );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the end-knot on the left.
  ///
  /// For example, if the spline is a cubic curve with 7 control points, then 
  /// endKnotLeft(2) will return:
  ///
  ///   0, 0, 0, 0, 0.25, 0.5, 0.75, 1, 1, 1, 1
  ///         |
  ///         |
  ///     this knot
  ///
  /////////////////////////////////////////////////////////////////////////////

  const KnotType &endKnotLeft ( KnotVectorSizeType whichIndepVar, KnotVectorSizeType whichEndKnot ) const
  {
    GN_ERROR_CHECK ( whichEndKnot < this->order ( whichIndepVar ) );
    return this->knot ( whichIndepVar, whichEndKnot );
  }

  KnotType &endKnotLeft ( KnotVectorSizeType whichIndepVar, KnotVectorSizeType whichEndKnot )
  {
    GN_ERROR_CHECK ( whichEndKnot < this->order ( whichIndepVar ) );
    return this->knot ( whichIndepVar, whichEndKnot );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the end-knot on the right.
  ///
  /// For example, if the spline is a cubic curve with 7 control points, then 
  /// endKnotRight(2) will return:
  ///
  ///   0, 0, 0, 0, 0.25, 0.5, 0.75, 1, 1, 1, 1
  ///                                      |
  ///                                      |
  ///                                  this knot
  ///
  /////////////////////////////////////////////////////////////////////////////

  const KnotType &endKnotRight ( KnotVectorSizeType whichIndepVar, KnotVectorSizeType whichEndKnot ) const
  {
    GN_ERROR_CHECK ( whichEndKnot < this->order ( whichIndepVar ) );
    UIntType offset ( this->numControlPoints ( whichIndepVar ) );
    return this->knot ( whichIndepVar, offset + whichEndKnot );
  }

  KnotType &endKnotRight ( KnotVectorSizeType whichIndepVar, KnotVectorSizeType whichEndKnot )
  {
    GN_ERROR_CHECK ( whichEndKnot < this->order ( whichIndepVar ) );
    UIntType offset ( this->numControlPoints ( whichIndepVar ) );
    return this->knot ( whichIndepVar, offset + whichEndKnot );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //	Get the knot multiplicity. Returns zero if the knot is not found.
  //
  /////////////////////////////////////////////////////////////////////////////

  UIntType knotMultiplicity ( KnotVectorSizeType whichIndepVar, KnotArgument u ) const
  {
    typedef typename KnotVector::const_iterator Iterator;
    typedef typename std::iterator_traits<Iterator>::difference_type DifferenceType;

    // Get the proper knot vector.
    const KnotVector &kv = this->knotVector ( whichIndepVar );

    // Find the first knot with the given value.
    Iterator first = std::find ( kv.begin(), kv.end(), u );
    if ( kv.end() == first )
      return 0;

    // Now find the first knot that is not equal to the given value. 
    // Start at the location we found above.
    Iterator last = std::find_if ( first + 1, kv.end(), std::bind2nd ( std::not_equal_to<KnotType>(), u ) );

    // Get the difference (distance) between the first and last iterator.
    DifferenceType diff = std::distance ( first, last );

    // Should always be true.
    GN_ERROR_CHECK ( diff >= 0 );

    // Safely cast to unsigned integer.
    return ( UIntType ( ( diff < 0 ) ? -diff : diff ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the control point.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const ControlPointType &controlPoint ( ControlPointSizeType whichDepVar, ControlPointSizeType whichControlPoint ) const
  {
    GN_ERROR_CHECK ( whichDepVar < _ctrPts.size() );
    GN_ERROR_CHECK ( whichControlPoint < _ctrPts[whichDepVar].size() );
    return _ctrPts[whichDepVar][whichControlPoint];
  }

  ControlPointType &controlPoint ( ControlPointSizeType whichDepVar, ControlPointSizeType whichControlPoint )
  {
    GN_ERROR_CHECK ( whichDepVar < _ctrPts.size() );
    GN_ERROR_CHECK ( whichControlPoint < _ctrPts[whichDepVar].size() );
    return _ctrPts[whichDepVar][whichControlPoint];
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the weight.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const ControlPointType &weight ( ControlPointSizeType whichWeight ) const
  {
    GN_ERROR_CHECK ( this->rational() );
    return this->controlPoint ( this->dimension(), whichWeight );
  }

  ControlPointType &weight ( ControlPointSizeType whichWeight )
  {
    GN_ERROR_CHECK ( this->rational() );
    return this->controlPoint ( this->dimension(), whichWeight );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Work-space structure.
  ///
  /////////////////////////////////////////////////////////////////////////////

  struct Work
  {
    Work() : basis(), left(), right(){}
    Work ( const Work &w ) : basis ( w.basis ), left ( w.left ), right ( w.right ){}
    WorkSpace basis;
    WorkSpace left;
    WorkSpace right;
  };


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the work-space. It is mutable.
  ///
  /////////////////////////////////////////////////////////////////////////////

  Work &work() const
  {
    return _work;
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Resize the spline.
  ///
  /// Note: Whether or not the original values of the knots and/or control 
  /// points are preserved is entirely up the respective container types. 
  /// The same applies for the appending of default vs. uninitialized values.
  ///
  /////////////////////////////////////////////////////////////////////////////

  void resize ( UIntType dimension, 
                const UIntContainer &order, 
                const UIntContainer &numCtrPts, 
                bool rational )
  {
    // Initialize.
    UIntType numIndepVars ( order.size() );
    UIntType numDepVars ( ( rational ) ? dimension + 1 : dimension );
    UIntType totalCtrPts ( this->_calculateTotalNumCtrPts ( numCtrPts ) );

    // Make sure...
    GN_ERROR_CHECK ( numIndepVars == numCtrPts.size() );
    GN_ERROR_CHECK ( numIndepVars >= Limits::MIN_NUM_INDEP_VARS );
    GN_ERROR_CHECK ( numDepVars   >= Limits::minNumDepVars ( rational ) );
    GN_ERROR_CHECK ( totalCtrPts  >= Limits::MIN_NUM_CTR_PTS * numIndepVars );

    // Set the data members.
    _order     = order;
    _numCtrPts = numCtrPts;
    _rational  = rational;

    // Resize the containers.
    _knots.resize  ( numIndepVars );
    _ctrPts.resize ( numDepVars );

    // For each independent variable...
    for ( UIntType i = 0; i < numIndepVars; ++i )
    {
      // Make sure the order and number of control points are valid.
      GN_ERROR_CHECK ( _order[i]     <= Limits::maxOrder ( _numCtrPts[i] ) );
      GN_ERROR_CHECK ( _numCtrPts[i] >= Limits::minNumCtrPts ( _order[i] ) );

      // Resize the knot vector.
      UIntType numKnots ( _numCtrPts[i] + _order[i] );
      _knots[i].resize ( numKnots );
    }

    // For each dependent variable...
    for ( UIntType j = 0; j < numDepVars; ++j )
    {
      // Resize the control points.
      _ctrPts[j].resize ( totalCtrPts );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Set the values of this spline to be the same as the given one.
  ///
  /////////////////////////////////////////////////////////////////////////////

  void set ( const ThisType &s )
  {
    // Handle self.
    if ( &s == this )
      return;
  
    // Set the data members.
    _order     = s._order;
    _numCtrPts = s._numCtrPts;
    _rational  = s._rational;
    _knots     = s._knots;
    _ctrPts    = s._ctrPts;

    // Call base class' function.
    BaseClass::set ( s );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Assignment operator.
  ///
  /////////////////////////////////////////////////////////////////////////////

  Spline &operator = ( const ThisType &s )
  {
    this->set ( s );
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// See if the current spline is valid. It uses the configured error-checker.
  ///
  /// This is a comprehensive error-checking routine, with the goal of finding 
  /// any and all errors. You may want to restrict calling this function to 
  /// portions of your code where speed is not critical.
  ///
  /// If the configured error-checker throws an exception when it finds an 
  /// error, then you can use this function as follows:
  ///
  ///   try { myspline.check() }
  ///   catch ( const std::exception &e ) { ... handle error here ... }
  ///
  /////////////////////////////////////////////////////////////////////////////

  void check() const
  {
    GN_ERROR_CHECK ( 0x0 != this );

    // Get integer values.
    UIntType numIndepVars ( this->numIndepVars() );
    UIntType numDepVars   ( this->numDepVars() );
    UIntType totalCtrPts  ( this->_calculateTotalNumCtrPts ( _numCtrPts ) );
    UIntType totalKnots   ( this->totalNumKnots() );

    // Make sure we are above the minimum.
    GN_ERROR_CHECK ( numIndepVars >= Limits::MIN_NUM_INDEP_VARS );
    GN_ERROR_CHECK ( numDepVars   >= Limits::minNumDepVars ( this->rational() ) );
    GN_ERROR_CHECK ( totalCtrPts  >= Limits::MIN_NUM_CTR_PTS * numIndepVars );
    GN_ERROR_CHECK ( totalKnots   >= Limits::MIN_NUM_KNOTS   * numIndepVars );

    // Check all the sizes.
    GN_ERROR_CHECK ( numIndepVars == _order.size() );
    GN_ERROR_CHECK ( numIndepVars == _numCtrPts.size() );
    GN_ERROR_CHECK ( numIndepVars == _knots.size() );
    GN_ERROR_CHECK ( numDepVars   == _ctrPts.size() );

    // For each independent variable...
    {
      for ( UIntType i = 0; i < numIndepVars; ++i )
      {
        // Make sure we are above the minimum.
        GN_ERROR_CHECK ( _order[i]        >= Limits::MIN_ORDER );
        GN_ERROR_CHECK ( _numCtrPts[i]    >= Limits::MIN_NUM_CTR_PTS );
        GN_ERROR_CHECK ( _knots[i].size() >= Limits::MIN_NUM_KNOTS );
        GN_ERROR_CHECK ( _order[i]        <= Limits::maxOrder ( _numCtrPts[i] ) );
        GN_ERROR_CHECK ( _numCtrPts[i]    >= Limits::minNumCtrPts ( _order[i] ) );

        // Check sizes.
        GN_ERROR_CHECK ( _knots[i].size() == _order[i] + _numCtrPts[i] );
        GN_ERROR_CHECK ( _knots[i].size() == this->numKnots ( i ) );

        // Make sure the first knot is a good (finite) floating point numbers.
        GN_ERROR_CHECK ( KnotTester::finite ( this->firstKnot ( i ) ) );

        // Loop through all the knots starting with the second one.
        UIntType numKnots ( this->numKnots ( i ) );
        {
          for ( UIntType j = 1; j < numKnots; ++j )
          {
            // Make sure the knots are not decreasing.
            const KnotType &left  = this->knot ( i, j - 1 );
            const KnotType &right = this->knot ( i, j );
            GN_ERROR_CHECK ( left <= right );

            // Make sure the knots are good (finite) floating point numbers.
            // We can just test the "right" knot because we tested the very 
            // first knot outside of the loop.
            GN_ERROR_CHECK ( KnotTester::finite ( right ) );
          }
        }

        // Get the first and last knot.
        KnotType first ( this->firstKnot ( i ) );
        KnotType last  ( this->lastKnot  ( i ) );

        // Make sure we have repeated end-knots. Start the loop at zero because 
        // "last" is the very last knot.
        {
          for ( UIntType j = 0; j < _order[i]; ++j )
          {
            const KnotType &left  = this->endKnotLeft  ( i, j );
            const KnotType &right = this->endKnotRight ( i, j );
            GN_ERROR_CHECK ( left  == first );
            GN_ERROR_CHECK ( right == last  );
          }
        }
      }
    }

    // For each dependent variables...
    {
      for ( UIntType i = 0; i < numDepVars; ++i )
      {
        // All the individual control point sequences should have the same size.
        GN_ERROR_CHECK ( _ctrPts[i].size() == totalCtrPts );

        // Make sure there are enough control points.
        GN_ERROR_CHECK ( _ctrPts[i].size() >= Limits::MIN_NUM_CTR_PTS );

        // Loop through all of the values of this dependent variable.
        {
          for ( UIntType j = 0; j < totalCtrPts; ++j )
          {
            // Make sure the value is a good (finite) floating point number.
            const ControlPointType &cp = this->controlPoint ( i, j );
            GN_ERROR_CHECK ( ControlPointTester::finite ( cp ) );
          }
        }
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// See if the splines are exactly equal.
  ///
  /////////////////////////////////////////////////////////////////////////////

  bool equal ( const ThisType &s ) const
  {
    typedef std::equal_to<KnotType>          KnotsPred;
    typedef std::equal_to<ControlPointType>  ControlPointPred;

    return ( 
      this->_equalIntegerData ( s ) &&
      GN::Algorithms::Equal::nestedContainers ( _knots,  s._knots,  KnotsPred() ) &&
      GN::Algorithms::Equal::nestedContainers ( _ctrPts, s._ctrPts, ControlPointPred() ) );
 }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// See if the splines are equal. The predicate arguments are functors
  /// that allows for special comparisons, such as being within a tolerance.
  ///
  /// Note: Predicate classes are passed by value because this works better 
  /// with a variety of compilers.
  ///
  /////////////////////////////////////////////////////////////////////////////

  template < class KnotsPred, class ControlPointPred >
  bool equal ( const ThisType &s, KnotsPred kp, ControlPointPred cpp ) const
  {
    return ( 
      this->_equalIntegerData ( s ) &&
      GN::Algorithms::Equal::nestedContainers ( _knots,  s._knots,  kp  ) &&
      GN::Algorithms::Equal::nestedContainers ( _ctrPts, s._ctrPts, cpp ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Set the number of dependent variables by truncating or appending.
  ///
  /// Note: Do not use this function to add dependent variables to an 
  /// unitialized spline, use resize() instead.
  ///
  /// nndv: New number of dependent variables.
  ///
  /////////////////////////////////////////////////////////////////////////////

  void numDepVars ( UIntType nndv )
  {
    typedef typename ControlPointContainer::value_type OneDepVarContainer;

    // Get the original number of dependent variables.
    UIntType ondv ( this->numDepVars() );

    // Both the input and the current state should be valid.
    GN_ERROR_CHECK ( nndv > Limits::minNumDepVars ( this->rational() ) );
    GN_ERROR_CHECK ( ondv > Limits::minNumDepVars ( this->rational() ) );

    // Handle trivial case.
    if ( ondv == nndv )
      return;

    // The offset at which we insert or erase depends on whether or not 
    // the spline is rational.
    UIntType adjust ( ( this->rational() ) ? 1 : 0 );

    // If we are adding dependent variables...
    if ( nndv > ondv )
    {
      // For each new dependent variable...
      for ( UIntType i = ondv; i < nndv; ++i )
      {
        // Insert the new container.
        UIntType offset ( i - adjust );
        _ctrPts.insert ( _ctrPts.begin() + offset, OneDepVarContainer() );

        // Size the new container.
        GN_ERROR_CHECK ( _ctrPts[offset].empty() );
        _ctrPts[offset].resize ( this->totalNumControlPoints() );
      }
    }

    // Otherwise, we are truncating...
    else
    {
      // Erase the range of containers.
      UIntType first ( nndv - adjust );
      UIntType last  ( ondv - adjust );
      _ctrPts.erase ( _ctrPts.begin() + first, _ctrPts.begin() + last );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Set the dimension by truncating or appending.
  ///
  /////////////////////////////////////////////////////////////////////////////

  void dimension ( UIntType d )
  {
    UIntType num ( ( this->rational() ) ? d + 1 : d );
    this->numDepVars ( num );
  }


protected:


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// See if the integer data is equal
  ///
  /////////////////////////////////////////////////////////////////////////////

  bool _equalIntegerData ( const ThisType &s ) const
  {
    return ( 
      s._order         == _order        &&
      s._numCtrPts     == _numCtrPts    &&
      s._rational      == _rational );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Calculates the total number of control points.
  ///
  /////////////////////////////////////////////////////////////////////////////

  static UIntType _calculateTotalNumCtrPts ( const UIntContainer &n )
  {
    return ( n.empty() ) ? 0 : std::accumulate ( n.begin(), n.end(), 1, std::multiplies<UIntType>() );
  }


private:


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Data members.
  ///
  /////////////////////////////////////////////////////////////////////////////

  UIntContainer         _order;
  UIntContainer         _numCtrPts;
  bool                  _rational;
  KnotContainer         _knots;
  ControlPointContainer _ctrPts;
  mutable Work          _work;
};


}; // namespace Splines
}; // namespace GN


///////////////////////////////////////////////////////////////////////////////
//
//  Equality operator. Cannot be a member.
//
///////////////////////////////////////////////////////////////////////////////

template < class ConfigType > inline bool operator == 
  ( const GN::Splines::Spline<ConfigType> &s1, const GN::Splines::Spline<ConfigType> &s2 )
{
  return s1.equal ( s2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Inequality operator. Cannot be a member.
//
///////////////////////////////////////////////////////////////////////////////

template < class ConfigType > inline bool operator != 
  ( const GN::Splines::Spline<ConfigType> &s1, const GN::Splines::Spline<ConfigType> &s2 )
{
  return !( s1.equal ( s2 ) );
}


#endif // _GENERIC_NURBS_LIBRARY_SPLINE_CLASS_H_
