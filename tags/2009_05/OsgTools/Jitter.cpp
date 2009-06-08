
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For jitter-based antialiasing.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Jitter.h"

#include "Usul/Exceptions/Thrower.h"
#include "Usul/Math/Constants.h"

#include "osg/Matrixd"
#include "osg/Viewport"

#include <stdexcept>

using namespace OsgTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data members.
//
///////////////////////////////////////////////////////////////////////////////

Jitter *Jitter::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Jitter::Jitter() : _methods ( 65 ), _scatterScale ( 1 )
{
  // Populate the points. Lots of inefficient memory shuffeling here, 
  // but it happens only once.

  // 2 jitter points.
  {
    Points points;
    points.push_back ( Point ( -0.250000,  0.250000 ) );
    points.push_back ( Point (  0.250000, -0.250000 ) );
    _methods.at ( points.size() ) = points;
  }

  // 3 jitter points
  {
    Points points;
    points.push_back ( Point ( -0.003392,  0.331797 ) );
    points.push_back ( Point (  0.280602, -0.249562 ) );
    points.push_back ( Point ( -0.273817, -0.086845 ) );
    _methods.at ( points.size() ) = points;
  };


  // 4 jitter points
  {
    Points points;
    points.push_back ( Point ( -0.125000, -0.250000 ) );
    points.push_back ( Point ( -0.375000,  0.250000 ) );
    points.push_back ( Point (  0.375000, -0.250000 ) );
    points.push_back ( Point (  0.125000,  0.250000 ) );
    _methods.at ( points.size() ) = points;
  };


  // 5 jitter points
  {
    Points points;
    points.push_back ( Point (  0.000000,  0.000000 ) );
    points.push_back ( Point ( -0.200000, -0.400000 ) );
    points.push_back ( Point (  0.200000,  0.400000 ) );
    points.push_back ( Point (  0.400000, -0.200000 ) );
    points.push_back ( Point ( -0.400000,  0.200000 ) );
    _methods.at ( points.size() ) = points;
  };


  // 6 jitter points
  {
    Points points;
    points.push_back ( Point ( -0.035354, -0.035354 ) );
    points.push_back ( Point ( -0.368687,  0.297979 ) );
    points.push_back ( Point (  0.035354,  0.368687 ) );
    points.push_back ( Point (  0.368687,  0.035354 ) );
    points.push_back ( Point (  0.297979, -0.368687 ) );
    points.push_back ( Point ( -0.297979, -0.297979 ) );
    _methods.at ( points.size() ) = points;
  };


  // 8 jitter points
  {
    Points points;
    points.push_back ( Point (  0.062500, -0.062500 ) );
    points.push_back ( Point ( -0.437500,  0.437500 ) );
    points.push_back ( Point ( -0.187500,  0.187500 ) );
    points.push_back ( Point (  0.187500,  0.312500 ) );
    points.push_back ( Point (  0.312500, -0.312500 ) );
    points.push_back ( Point (  0.437500,  0.062500 ) );
    points.push_back ( Point ( -0.062500, -0.437500 ) );
    points.push_back ( Point ( -0.312500, -0.187500 ) );
    _methods.at ( points.size() ) = points;
  };


  // 9 jitter points
  {
    Points points;
    points.push_back ( Point (  0.000000,  0.000000 ) );
    points.push_back ( Point ( -0.333333,  0.444444 ) );
    points.push_back ( Point (  0.000000, -0.333333 ) );
    points.push_back ( Point (  0.000000,  0.333333 ) );
    points.push_back ( Point ( -0.333333, -0.222222 ) );
    points.push_back ( Point (  0.333333, -0.111111 ) );
    points.push_back ( Point ( -0.333333,  0.111111 ) );
    points.push_back ( Point (  0.333333,  0.222222 ) );
    points.push_back ( Point (  0.333333, -0.444444 ) );
    _methods.at ( points.size() ) = points;
  };


  // 12 jitter points
  {
    Points points;
    points.push_back ( Point (  0.083333,  0.125000 ) );
    points.push_back ( Point (  0.416666,  0.375000 ) );
    points.push_back ( Point ( -0.250000,  0.125000 ) );
    points.push_back ( Point (  0.083333, -0.375000 ) );
    points.push_back ( Point (  0.250000, -0.375000 ) );
    points.push_back ( Point ( -0.416666, -0.375000 ) );
    points.push_back ( Point (  0.250000,  0.125000 ) );
    points.push_back ( Point ( -0.250000, -0.375000 ) );
    points.push_back ( Point (  0.083333, -0.125000 ) );
    points.push_back ( Point (  0.416666, -0.125000 ) );
    points.push_back ( Point ( -0.416666,  0.125000 ) );
    points.push_back ( Point (  0.083333,  0.375000 ) );
    _methods.at ( points.size() ) = points;
  };


  // 15 jitter points
  {
    Points points;
    points.push_back ( Point (  0.285561,  0.188437 ) );
    points.push_back ( Point (  0.360176, -0.065688 ) );
    points.push_back ( Point ( -0.111751,  0.275019 ) );
    points.push_back ( Point ( -0.055918, -0.215197 ) );
    points.push_back ( Point ( -0.080231, -0.470965 ) );
    points.push_back ( Point (  0.138721,  0.409168 ) );
    points.push_back ( Point (  0.384120,  0.458500 ) );
    points.push_back ( Point ( -0.454968,  0.134088 ) );
    points.push_back ( Point (  0.179271, -0.331196 ) );
    points.push_back ( Point ( -0.307049, -0.364927 ) );
    points.push_back ( Point (  0.105354, -0.010099 ) );
    points.push_back ( Point ( -0.154180,  0.021794 ) );
    points.push_back ( Point ( -0.370135, -0.116425 ) );
    points.push_back ( Point (  0.451636, -0.300013 ) );
    points.push_back ( Point ( -0.370610,  0.387504 ) );
    _methods.at ( points.size() ) = points;
  };


  // 16 jitter points
  {
    Points points;
    points.push_back ( Point ( -0.125000, -0.062500 ) );
    points.push_back ( Point (  0.125000, -0.437500 ) );
    points.push_back ( Point (  0.375000, -0.312500 ) );
    points.push_back ( Point ( -0.375000, -0.437500 ) );
    points.push_back ( Point ( -0.125000,  0.187500 ) );
    points.push_back ( Point (  0.375000, -0.062500 ) );
    points.push_back ( Point (  0.125000,  0.062500 ) );
    points.push_back ( Point ( -0.125000,  0.437500 ) );
    points.push_back ( Point (  0.125000, -0.187500 ) );
    points.push_back ( Point ( -0.375000,  0.062500 ) );
    points.push_back ( Point ( -0.375000,  0.312500 ) );
    points.push_back ( Point ( -0.125000, -0.312500 ) );
    points.push_back ( Point (  0.375000,  0.437500 ) );
    points.push_back ( Point (  0.375000, -0.187500 ) );
    points.push_back ( Point ( -0.375000, -0.187500 ) );
    points.push_back ( Point (  0.125000,  0.312500 ) );
    _methods.at ( points.size() ) = points;
  };


  // 23 jitter points
  {
    Points points;
    points.push_back ( Point (  0.030245,  0.136384 ) );
    points.push_back ( Point (  0.018865, -0.348867 ) );
    points.push_back ( Point ( -0.350114, -0.472309 ) );
    points.push_back ( Point (  0.222181,  0.149524 ) );
    points.push_back ( Point ( -0.393670, -0.266873 ) );
    points.push_back ( Point (  0.404568,  0.230436 ) );
    points.push_back ( Point (  0.098381,  0.465337 ) );
    points.push_back ( Point (  0.462671,  0.442116 ) );
    points.push_back ( Point (  0.400373, -0.212720 ) );
    points.push_back ( Point ( -0.409988,  0.263345 ) );
    points.push_back ( Point ( -0.115878, -0.001981 ) );
    points.push_back ( Point (  0.348425, -0.009237 ) );
    points.push_back ( Point ( -0.464016,  0.066467 ) );
    points.push_back ( Point ( -0.138674, -0.468006 ) );
    points.push_back ( Point (  0.144932, -0.022780 ) );
    points.push_back ( Point ( -0.250195,  0.150161 ) );
    points.push_back ( Point ( -0.181400, -0.264219 ) );
    points.push_back ( Point (  0.196097, -0.234139 ) );
    points.push_back ( Point ( -0.311082, -0.078815 ) );
    points.push_back ( Point (  0.268379,  0.366778 ) );
    points.push_back ( Point ( -0.040601,  0.327109 ) );
    points.push_back ( Point ( -0.234392,  0.354659 ) );
    points.push_back ( Point ( -0.003102, -0.154402 ) );
    _methods.at ( points.size() ) = points;
  };


  // 24 jitter points
  {
    Points points;
    points.push_back ( Point (  0.030245,  0.136384 ) );
    points.push_back ( Point (  0.018865, -0.348867 ) );
    points.push_back ( Point ( -0.350114, -0.472309 ) );
    points.push_back ( Point (  0.222181,  0.149524 ) );
    points.push_back ( Point ( -0.393670, -0.266873 ) );
    points.push_back ( Point (  0.404568,  0.230436 ) );
    points.push_back ( Point (  0.098381,  0.465337 ) );
    points.push_back ( Point (  0.462671,  0.442116 ) );
    points.push_back ( Point (  0.400373, -0.212720 ) );
    points.push_back ( Point ( -0.409988,  0.263345 ) );
    points.push_back ( Point ( -0.115878, -0.001981 ) );
    points.push_back ( Point (  0.348425, -0.009237 ) );
    points.push_back ( Point ( -0.464016,  0.066467 ) );
    points.push_back ( Point ( -0.138674, -0.468006 ) );
    points.push_back ( Point (  0.144932, -0.022780 ) );
    points.push_back ( Point ( -0.250195,  0.150161 ) );
    points.push_back ( Point ( -0.181400, -0.264219 ) );
    points.push_back ( Point (  0.196097, -0.234139 ) );
    points.push_back ( Point ( -0.311082, -0.078815 ) );
    points.push_back ( Point (  0.268379,  0.366778 ) );
    points.push_back ( Point ( -0.040601,  0.327109 ) );
    points.push_back ( Point ( -0.234392,  0.354659 ) );
    points.push_back ( Point ( -0.003102, -0.154402 ) );
    points.push_back ( Point (  0.297997, -0.417965 ) );
    _methods.at ( points.size() ) = points;
  };


  // 64 jitter points
  {
    Points points;
    points.push_back ( Point (  0.266377, -0.218171 ) );
    points.push_back ( Point ( -0.170919, -0.429368 ) );
    points.push_back ( Point (  0.047356, -0.387135 ) );
    points.push_back ( Point ( -0.430063,  0.363413 ) );
    points.push_back ( Point ( -0.221638, -0.313768 ) );
    points.push_back ( Point (  0.124758, -0.197109 ) );
    points.push_back ( Point ( -0.400021,  0.482195 ) );
    points.push_back ( Point (  0.247882,  0.152010 ) );
    points.push_back ( Point ( -0.286709, -0.470214 ) );
    points.push_back ( Point ( -0.426790,  0.004977 ) );
    points.push_back ( Point ( -0.361249, -0.104549 ) );
    points.push_back ( Point ( -0.040643,  0.123453 ) );
    points.push_back ( Point ( -0.189296,  0.438963 ) );
    points.push_back ( Point ( -0.453521, -0.299889 ) );
    points.push_back ( Point (  0.408216, -0.457699 ) );
    points.push_back ( Point (  0.328973, -0.101914 ) );
    points.push_back ( Point ( -0.055540, -0.477952 ) );
    points.push_back ( Point (  0.194421,  0.453510 ) );
    points.push_back ( Point (  0.404051,  0.224974 ) );
    points.push_back ( Point (  0.310136,  0.419700 ) );
    points.push_back ( Point ( -0.021743,  0.403898 ) );
    points.push_back ( Point ( -0.466210,  0.248839 ) );
    points.push_back ( Point (  0.341369,  0.081490 ) );
    points.push_back ( Point (  0.124156, -0.016859 ) );
    points.push_back ( Point ( -0.461321, -0.176661 ) );
    points.push_back ( Point (  0.013210,  0.234401 ) );
    points.push_back ( Point (  0.174258, -0.311854 ) );
    points.push_back ( Point (  0.294061,  0.263364 ) );
    points.push_back ( Point ( -0.114836,  0.328189 ) );
    points.push_back ( Point (  0.041206, -0.106205 ) );
    points.push_back ( Point (  0.079227,  0.345021 ) );
    points.push_back ( Point ( -0.109319, -0.242380 ) );
    points.push_back ( Point (  0.425005, -0.332397 ) );
    points.push_back ( Point (  0.009146,  0.015098 ) );
    points.push_back ( Point ( -0.339084, -0.355707 ) );
    points.push_back ( Point ( -0.224596, -0.189548 ) );
    points.push_back ( Point (  0.083475,  0.117028 ) );
    points.push_back ( Point (  0.295962, -0.334699 ) );
    points.push_back ( Point (  0.452998,  0.025397 ) );
    points.push_back ( Point (  0.206511, -0.104668 ) );
    points.push_back ( Point (  0.447544, -0.096004 ) );
    points.push_back ( Point ( -0.108006, -0.002471 ) );
    points.push_back ( Point ( -0.380810,  0.130036 ) );
    points.push_back ( Point ( -0.242440,  0.186934 ) );
    points.push_back ( Point ( -0.200363,  0.070863 ) );
    points.push_back ( Point ( -0.344844, -0.230814 ) );
    points.push_back ( Point (  0.408660,  0.345826 ) );
    points.push_back ( Point ( -0.233016,  0.305203 ) );
    points.push_back ( Point (  0.158475, -0.430762 ) );
    points.push_back ( Point (  0.486972,  0.139163 ) );
    points.push_back ( Point ( -0.301610,  0.009319 ) );
    points.push_back ( Point (  0.282245, -0.458671 ) );
    points.push_back ( Point (  0.482046,  0.443890 ) );
    points.push_back ( Point ( -0.121527,  0.210223 ) );
    points.push_back ( Point ( -0.477606, -0.424878 ) );
    points.push_back ( Point ( -0.083941, -0.121440 ) );
    points.push_back ( Point ( -0.345773,  0.253779 ) );
    points.push_back ( Point (  0.234646,  0.034549 ) );
    points.push_back ( Point (  0.394102, -0.210901 ) );
    points.push_back ( Point ( -0.312571,  0.397656 ) );
    points.push_back ( Point (  0.200906,  0.333293 ) );
    points.push_back ( Point (  0.018703, -0.261792 ) );
    points.push_back ( Point ( -0.209349, -0.065383 ) );
    points.push_back ( Point (  0.076248,  0.478538 ) );
    _methods.at ( points.size() ) = points;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Jitter::~Jitter()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton instantiation and access.
//
///////////////////////////////////////////////////////////////////////////////

Jitter &Jitter::instance()
{
  if ( 0x0 == _instance )
    _instance = new Jitter;
  return *_instance;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Generate appropriate frustum for jitter-based antialiasing.
//  See: http://www.opengl.org/resources/code/basics/redbook/accpersp.c
//  Not sure how this works, so not many comments...
//
/////////////////////////////////////////////////////////////////////////////

namespace Details
{
  void _frustum ( double left,  double right, 
                  double bottom, double top, 
                  double zNear, double zFar, 
                  double pixdx, double pixdy, 
                  double width, double height, 
                  osg::Matrixd &mat )
  {
    double xwsize ( right - left );
    double ywsize ( top - bottom );
    double dx ( - ( pixdx * xwsize / width  ) );
    double dy ( - ( pixdy * ywsize / height ) );
    mat.makeFrustum ( left + dx, right + dx, bottom + dy, top + dy, zNear, zFar );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Generate appropriate perspective matrix for jitter-based antialiasing.
//  See: http://www.opengl.org/resources/code/basics/redbook/accpersp.c
//  Not sure how this works, so not many comments...
//
//  Also might want to look at:
//  http://www.opengl.org/resources/tutorials/advanced/advanced97/programs/accumaa.c
//
/////////////////////////////////////////////////////////////////////////////

namespace Details
{
  void _perspective ( double fov, double aspect, 
                      double zNear, double zFar, 
                      double pixdx, double pixdy, 
                      double width, double height, 
                      osg::Matrixd &mat )
  {
    double fov2 ( fov * Usul::Math::DEG_TO_RAD / 2.0 );
    double top ( zNear / ( Usul::Math::cos ( fov2 ) / Usul::Math::sin ( fov2 ) ) );
    double bottom ( -top );
    double right ( top * aspect );
    double left ( -right );
    Details::_frustum ( left, right, bottom, top, zNear, zFar, pixdx, pixdy, width, height, mat );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get jittered perspective matrix for given number of passes.
//
///////////////////////////////////////////////////////////////////////////////

void Jitter::perspective ( unsigned int numPasses, unsigned int currentPass, const osg::Viewport &vp, const osg::Matrixd &base, osg::Matrixd &mat )
{
  // Make sure the input makes sense. It is easy to reverse them.
  if ( currentPass >= numPasses )
    Usul::Exceptions::Thrower<std::range_error> ( "Error 1610665489: currentPass = ", currentPass, ", but numPasses = ", numPasses );

  // See if method is available.
  if ( numPasses >= _methods.size() || _methods.at ( numPasses ).empty() )
    Usul::Exceptions::Thrower<std::range_error> ( "Error 2492596738: Jitter points for ", numPasses, " rendering passes is not available" );

  // Get the points.
  const Points &points = _methods.at ( numPasses );

  // See if requested point exists.
  if ( currentPass >= points.size() )
    Usul::Exceptions::Thrower<std::range_error> ( "Error 3164801215: Jitter point ", currentPass, " is out of range for ", points.size(), "-pass rendering" );

  // These "points" are actually fractions of a pixel size 
  // so we multiply by the scaller-scale.
  const Point point = _scatterScale * points.at ( currentPass );

  // Get the perspective values from the base matrix.
  double fovy ( 0 ), aspect ( 0 ), zNear ( 0 ), zFar ( 0 );
  if ( false == base.getPerspective ( fovy, aspect, zNear, zFar ) )
    Usul::Exceptions::Thrower<std::range_error> ( "Error 1717302052: Given projection matrix is not a symmetric perspective matrix" );

  // Apply jitter to get new matrix.
  Details::_perspective ( fovy, aspect, zNear, zFar, point[0], point[1], vp.width(), vp.height(), mat );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get list of available methods. Most of "_methods" is empty.
//
///////////////////////////////////////////////////////////////////////////////

Jitter::Available Jitter::available() const
{
  Available a;

  for ( Methods::const_iterator i = _methods.begin(); i != _methods.end(); ++i )
  {
    const Points &points = *i;
    if ( false == points.empty() )
    {
      std::ostringstream s;
      s << points.size() << "-Pass";
      a.push_back ( MethodPair ( points.size(), s.str() ) );
    }
  }

  return a;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given methos is available.
//
///////////////////////////////////////////////////////////////////////////////

bool Jitter::available ( unsigned int num ) const
{
  for ( Methods::const_iterator i = _methods.begin(); i != _methods.end(); ++i )
  {
    const Points &points = *i;
    if ( points.size() == num )
      return true;
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scatter scale.
//
///////////////////////////////////////////////////////////////////////////////

void Jitter::scatterScale ( double scale )
{
  _scatterScale = scale;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scatter scale.
//
///////////////////////////////////////////////////////////////////////////////

double Jitter::scatterScale() const
{
  return _scatterScale;
}
