
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
#include <vector>


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
  typedef typename ConfigType::SizeType                           SizeType;
  typedef typename ConfigType::IndependentType                    IndependentType;
  typedef typename ConfigType::DependentType                      DependentType;
  typedef typename ConfigType::SizeContainer                      SizeContainer;
  typedef typename ConfigType::IndependentContainer               IndependentContainer;
  typedef typename IndependentContainer::value_type               IndependentSequence;
  typedef typename ConfigType::DependentContainer                 DependentContainer;
  typedef typename DependentContainer::value_type                 DependentSequence;
  typedef typename ConfigType::ErrorCheckerType                   ErrorCheckerType;
  typedef typename ConfigType::Vec2                               Vec2;
  typedef typename ConfigType::Vec3                               Vec3;
  typedef typename ConfigType::Vec4                               Vec4;
  typedef typename ConfigType::Matrix44                           Matrix44;
  typedef typename ConfigType::IndependentTester                  IndependentTester;
  typedef typename ConfigType::DependentTester                    DependentTester;
  typedef typename ConfigType::Translation                        Translation;
  typedef typename ConfigType::Scale                              Scale;
  typedef typename ConfigType::Multiply                           Multiply;
  typedef typename ConfigType::SquareRoot                         SquareRoot;
  typedef typename ConfigType::Power                              Power;
  typedef typename ConfigType::Vector                             Vector;
  typedef typename GN::Traits::Argument<DependentType>::Type      DependentArgument;
  typedef typename GN::Traits::Argument<IndependentType>::Type    IndependentArgument;
  typedef GN::Limits::Nurbs<SizeType>                             Limits;
  typedef GN::Traits::Spline                                      TypeTag;
  typedef ThisType                                                SplineClass;
  typedef GN::Work::Container<Vector,ErrorCheckerType>            WorkSpace;


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Default constructor.
  ///
  /////////////////////////////////////////////////////////////////////////////

  Spline() : BaseClass(),
    _order       (),
    _numCtrPts   (),
    _rational     ( false ),
    _knots       (),
    _ctrPts      (),
    _work        ()
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

#ifdef GN_USE_VIRTUAL_PROTECTED_DESTRUCTORS
protected:
  virtual ~Spline()
#else
  ~Spline()
#endif
  {
  }
#ifdef GN_USE_VIRTUAL_PROTECTED_DESTRUCTORS
public:
#endif


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// The number of independent variables. A curve has 1, a surface 2, etc.
  ///
  /////////////////////////////////////////////////////////////////////////////

  SizeType numIndepVars() const
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

  SizeType numDepVars() const
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

  SizeType dimension() const
  {
    // We check the number of dependent variables also because the spline 
    // may be newly constructed and thus have invalid data.
    SizeType numDepVars ( this->numDepVars() );
    return ( this->rational() && numDepVars > 0 ) ? numDepVars - 1 : numDepVars;
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the order for the specified independent variable.
  ///
  ///   order == degree + 1.
  ///
  /////////////////////////////////////////////////////////////////////////////

  SizeType order ( SizeType whichIndepVar ) const
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

  SizeType degree ( SizeType whichIndepVar ) const
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

  SizeType numControlPoints ( SizeType whichIndepVar ) const
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

  SizeType numKnots ( SizeType whichIndepVar ) const
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

  SizeType totalNumControlPoints() const
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

  SizeType totalNumKnots() const
  {
    // This will sum all the sizes of the individual knot vectors.
    typedef typename IndependentContainer::const_iterator Iterator;
    SizeType sum ( 0 );
    for ( Iterator i = _knots.begin(); i != _knots.end(); ++i )
      sum += i->size();
    return sum;
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Access to the knot vector. Be carful with this!
  ///
  /////////////////////////////////////////////////////////////////////////////

  const IndependentSequence &knotVector ( SizeType whichIndepVar ) const
  {
    GN_ERROR_CHECK ( whichIndepVar < _knots.size() );
    return _knots[whichIndepVar];
  }

  IndependentSequence &knotVector ( SizeType whichIndepVar )
  {
    GN_ERROR_CHECK ( whichIndepVar < _knots.size() );
    return _knots[whichIndepVar];
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the knot.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const IndependentType &knot ( SizeType whichIndepVar, SizeType whichKnot ) const
  {
    GN_ERROR_CHECK ( whichIndepVar < _knots.size() );
    GN_ERROR_CHECK ( whichKnot < _knots[whichIndepVar].size() );
    return _knots[whichIndepVar][whichKnot];
  }

  IndependentType &knot ( SizeType whichIndepVar, SizeType whichKnot )
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

  const IndependentType &firstKnot ( SizeType whichIndepVar ) const
  {
    return this->knot ( whichIndepVar, 0 );
  }

  IndependentType &firstKnot ( SizeType whichIndepVar )
  {
    return this->knot ( whichIndepVar, 0 );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the last knot in the specified knot vector.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const IndependentType &lastKnot ( SizeType whichIndepVar ) const
  {
    SizeType num ( this->numKnots ( whichIndepVar ) );
    return this->knot ( whichIndepVar, num - 1 );
  }

  IndependentType &lastKnot ( SizeType whichIndepVar )
  {
    SizeType num ( this->numKnots ( whichIndepVar ) );
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

  const IndependentType &endKnotLeft ( SizeType whichIndepVar, SizeType whichEndKnot ) const
  {
    GN_ERROR_CHECK ( whichEndKnot < this->order ( whichIndepVar ) );
    return this->knot ( whichIndepVar, whichEndKnot );
  }

  IndependentType &endKnotLeft ( SizeType whichIndepVar, SizeType whichEndKnot )
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

  const IndependentType &endKnotRight ( SizeType whichIndepVar, SizeType whichEndKnot ) const
  {
    GN_ERROR_CHECK ( whichEndKnot < this->order ( whichIndepVar ) );
    SizeType offset ( this->numControlPoints ( whichIndepVar ) );
    return this->knot ( whichIndepVar, offset + whichEndKnot );
  }

  IndependentType &endKnotRight ( SizeType whichIndepVar, SizeType whichEndKnot )
  {
    GN_ERROR_CHECK ( whichEndKnot < this->order ( whichIndepVar ) );
    SizeType offset ( this->numControlPoints ( whichIndepVar ) );
    return this->knot ( whichIndepVar, offset + whichEndKnot );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //	Get the knot multiplicity. Returns zero if the knot is not found.
  //
  /////////////////////////////////////////////////////////////////////////////

  SizeType knotMultiplicity ( SizeType whichIndepVar, IndependentArgument u ) const
  {
    typedef typename IndependentSequence::const_iterator Iterator;
    typedef typename std::iterator_traits<Iterator>::difference_type DifferenceType;

    // Get the proper knot vector.
    const IndependentSequence &kv = this->knotVector ( whichIndepVar );

    // Find the first knot with the given value.
    Iterator first = std::find ( kv.begin(), kv.end(), u );
    if ( kv.end() == first )
      return 0;

    // Now find the first knot that is not equal to the given value. 
    // Start at the location we found above.
    Iterator last = std::find_if ( first + 1, kv.end(), std::bind2nd ( std::not_equal_to<IndependentType>(), u ) );

    // Get the difference (distance) between the first and last iterator.
    DifferenceType diff = std::distance ( first, last );

    // Should always be true.
    GN_ERROR_CHECK ( diff >= 0 );

    // Safely cast to unsigned integer.
    return ( SizeType ( ( diff < 0 ) ? -diff : diff ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the control point.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const DependentType &controlPoint ( SizeType whichDepVar, SizeType whichControlPoint ) const
  {
    GN_ERROR_CHECK ( whichDepVar < _ctrPts.size() );
    GN_ERROR_CHECK ( whichControlPoint < _ctrPts[whichDepVar].size() );
    return _ctrPts[whichDepVar][whichControlPoint];
  }

  DependentType &controlPoint ( SizeType whichDepVar, SizeType whichControlPoint )
  {
    GN_ERROR_CHECK ( whichDepVar < _ctrPts.size() );
    GN_ERROR_CHECK ( whichControlPoint < _ctrPts[whichDepVar].size() );
    return _ctrPts[whichDepVar][whichControlPoint];
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Access to the control point container. Be careful!
  ///
  /////////////////////////////////////////////////////////////////////////////

  const DependentSequence &controlPoints ( SizeType whichDepVar ) const
  {
    GN_ERROR_CHECK ( whichDepVar < _ctrPts.size() );
    return _ctrPts[whichDepVar];
  }

  DependentSequence &controlPoints ( SizeType whichDepVar )
  {
    GN_ERROR_CHECK ( whichDepVar < _ctrPts.size() );
    return _ctrPts[whichDepVar];
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the weight.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const DependentType &weight ( SizeType whichWeight ) const
  {
    GN_ERROR_CHECK ( this->rational() );
    return this->controlPoint ( this->dimension(), whichWeight );
  }

  DependentType &weight ( SizeType whichWeight )
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
    Work() : basis(), left(), right(), pw(), temp(){}
    Work ( const Work &w ) : basis ( w.basis ), left ( w.left ), right ( w.right ), pw ( w.pw ), temp ( w.temp ){}
    WorkSpace basis;
    WorkSpace left;
    WorkSpace right;
    WorkSpace pw;
    WorkSpace temp;
  };


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the work-space. It is mutable.
  ///
  /////////////////////////////////////////////////////////////////////////////

  Work &work ( SizeType whichIndepVar ) const
  {
    GN_ERROR_CHECK ( whichIndepVar < _work.size() );
    return _work[whichIndepVar];
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

  void resize ( SizeType dimension, 
                const SizeContainer &order, 
                const SizeContainer &numCtrPts, 
                bool rational )
  {
    // Initialize.
    SizeType numIndepVars ( order.size() );
    SizeType numDepVars ( ( rational ) ? dimension + 1 : dimension );
    SizeType totalCtrPts ( this->_calculateTotalNumCtrPts ( numCtrPts ) );

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
    for ( SizeType i = 0; i < numIndepVars; ++i )
    {
      // Make sure the order and number of control points are valid.
      GN_ERROR_CHECK ( _order[i]     <= Limits::maxOrder ( _numCtrPts[i] ) );
      GN_ERROR_CHECK ( _numCtrPts[i] >= Limits::minNumCtrPts ( _order[i] ) );

      // Resize the knot vector.
      SizeType numKnots ( _numCtrPts[i] + _order[i] );
      _knots[i].resize ( numKnots );
    }

    // For each dependent variable...
    for ( SizeType j = 0; j < numDepVars; ++j )
    {
      // Resize the control points.
      _ctrPts[j].resize ( totalCtrPts );
    }

    // Resize the work.
    _work.resize ( numIndepVars );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Resize as a curve.
  ///
  /////////////////////////////////////////////////////////////////////////////

  void resize ( SizeType dimension, 
                SizeType order, 
                SizeType numCtrPts, 
                bool rational )
  {
    SizeContainer o;
    o.resize ( 1 );
    o[0] = order;

    SizeContainer n;
    n.resize ( 1 );
    n[0] = numCtrPts;

    this->resize ( dimension, o, n, rational );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Resize as a surface.
  ///
  /////////////////////////////////////////////////////////////////////////////

  void resize ( SizeType dimension, 
                SizeType orderU, 
                SizeType orderV, 
                SizeType numCtrPtsU, 
                SizeType numCtrPtsV, 
                bool rational )
  {
    SizeContainer o;
    o.resize ( 2 );
    o[0] = orderU;
    o[1] = orderV;

    SizeContainer n;
    n.resize ( 2 );
    n[0] = numCtrPtsU;
    n[1] = numCtrPtsV;

    this->resize ( dimension, o, n, rational );
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
    SizeType numIndepVars ( this->numIndepVars() );
    SizeType numDepVars   ( this->numDepVars() );
    SizeType totalCtrPts  ( this->_calculateTotalNumCtrPts ( _numCtrPts ) );
    SizeType totalKnots   ( this->totalNumKnots() );

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
      for ( SizeType i = 0; i < numIndepVars; ++i )
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
        GN_ERROR_CHECK ( IndependentTester::finite ( this->firstKnot ( i ) ) );

        // Loop through all the knots starting with the second one.
        SizeType numKnots ( this->numKnots ( i ) );
        {
          for ( SizeType j = 1; j < numKnots; ++j )
          {
            // Make sure the knots are not decreasing.
            const IndependentType &left  = this->knot ( i, j - 1 );
            const IndependentType &right = this->knot ( i, j );
            GN_ERROR_CHECK ( left <= right );

            // Make sure the knots are good (finite) floating point numbers.
            // We can just test the "right" knot because we tested the very 
            // first knot outside of the loop.
            GN_ERROR_CHECK ( IndependentTester::finite ( right ) );
          }
        }

        // Get the first and last knot.
        IndependentType first ( this->firstKnot ( i ) );
        IndependentType last  ( this->lastKnot  ( i ) );

        // Make sure we have repeated end-knots. Start the loop at zero because 
        // "last" is the very last knot.
        {
          for ( SizeType j = 0; j < _order[i]; ++j )
          {
            const IndependentType &left  = this->endKnotLeft  ( i, j );
            const IndependentType &right = this->endKnotRight ( i, j );
            GN_ERROR_CHECK ( left  == first );
            GN_ERROR_CHECK ( right == last  );
          }
        }
      }
    }

    // For each dependent variables...
    {
      for ( SizeType i = 0; i < numDepVars; ++i )
      {
        // All the individual control point sequences should have the same size.
        GN_ERROR_CHECK ( _ctrPts[i].size() == totalCtrPts );

        // Make sure there are enough control points.
        GN_ERROR_CHECK ( _ctrPts[i].size() >= Limits::MIN_NUM_CTR_PTS );

        // Loop through all of the values of this dependent variable.
        {
          for ( SizeType j = 0; j < totalCtrPts; ++j )
          {
            // Make sure the value is a good (finite) floating point number.
            const DependentType &cp = this->controlPoint ( i, j );
            GN_ERROR_CHECK ( DependentTester::finite ( cp ) );
          }
        }
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// See if the spline is valid.
  ///
  /////////////////////////////////////////////////////////////////////////////

  bool valid() const
  {
    try
    {
      this->check();
    }
    catch ( ... )
    {
      return false;
    }

    return true;
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// See if the splines are exactly equal.
  ///
  /////////////////////////////////////////////////////////////////////////////

  bool equal ( const ThisType &s ) const
  {
    typedef std::equal_to<IndependentType> KnotsPred;
    typedef std::equal_to<DependentType> ControlPointPred;

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

  void numDepVars ( SizeType nndv )
  {
    typedef typename DependentContainer::value_type OneDepVarContainer;

    // Get the original number of dependent variables.
    SizeType ondv ( this->numDepVars() );

    // Both the input and the current state should be valid.
    GN_ERROR_CHECK ( nndv > Limits::minNumDepVars ( this->rational() ) );
    GN_ERROR_CHECK ( ondv > Limits::minNumDepVars ( this->rational() ) );

    // Handle trivial case.
    if ( ondv == nndv )
      return;

    // The offset at which we insert or erase depends on whether or not 
    // the spline is rational.
    SizeType adjust ( ( this->rational() ) ? 1 : 0 );

    // If we are adding dependent variables...
    if ( nndv > ondv )
    {
      // For each new dependent variable...
      for ( SizeType i = ondv; i < nndv; ++i )
      {
        // Insert the new container.
        SizeType offset ( i - adjust );
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
      SizeType first ( nndv - adjust );
      SizeType last  ( ondv - adjust );
      _ctrPts.erase ( _ctrPts.begin() + first, _ctrPts.begin() + last );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Set the dimension by truncating or appending.
  ///
  /////////////////////////////////////////////////////////////////////////////

  void dimension ( SizeType d )
  {
    SizeType num ( ( this->rational() ) ? d + 1 : d );
    this->numDepVars ( num );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Return a reference to this spline.
  ///
  /////////////////////////////////////////////////////////////////////////////

  SplineClass &spline()
  {
    return *this;
  }
  const SplineClass &spline() const
  {
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Clear this spline.
  ///
  /////////////////////////////////////////////////////////////////////////////

  void clear()
  {
    _order.clear();
    _numCtrPts.clear();
    _knots.clear();
    _ctrPts.clear();
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

  static SizeType _calculateTotalNumCtrPts ( const SizeContainer &n )
  {
    return ( n.empty() ) ? 0 : std::accumulate ( n.begin(), n.end(), 1, std::multiplies<SizeType>() );
  }


private:


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Typedefs.
  ///
  /////////////////////////////////////////////////////////////////////////////

  typedef std::vector < Work > WorkSequence;


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Data members.
  ///
  /////////////////////////////////////////////////////////////////////////////

  SizeContainer         _order;
  SizeContainer         _numCtrPts;
  bool                  _rational;
  IndependentContainer  _knots;
  DependentContainer    _ctrPts;
  mutable WorkSequence  _work;
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
