
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Animate/Movie.h"

#include "GN/Interpolate/Global.h"
#include "GN/Algorithms/KnotVector.h"
#include "GN/Algorithms/Fill.h"

#include "Usul/Math/Transpose.h"
#include "Usul/Errors/Assert.h"
#include "Usul/System/Clock.h"
#include "Usul/Components/Manager.h"

#include "Usul/Interfaces/IActiveView.h"
#include "Usul/Interfaces/ITrackball.h"
#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Interfaces/IFlushEvents.h"
#include "Usul/Interfaces/IWriteMovieFile.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Errors/ThrowingPolicy.h"
#include "Usul/File/Temp.h"
#include "Usul/Strings/Case.h"
#include "Usul/File/Path.h"

#include "XmlDom/Policy.h"
#include "XmlDom/Reader.h"
#include "XmlDom/BuildTree.h"
#include "XmlDom/Node.h"
#include "XmlDom/Visitor.h"
#include "XmlDom/Convert.h"
#include "XmlDom/File.h"

#include <sstream>
#include <fstream>
#include <ctime>

using namespace Animate;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Movie::Movie () :
BaseClass ( "Camera Document" ),
_frames(),
_currentFrame( 0 ),
_segTime   ( 2000 ),
_lastU     ( 0.0  ),
_mode      ( IDLE ),
_loop      ( false )
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Movie::~Movie()
{
  _frames.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Movie::ParserHelper::ParserHelper ( Frames &frames ) : 
  _frame    ( 0x0 ), 
  _frames   ( frames ),
  _center   ( 0, 0, 0 ),
  _rotation ( 0, 0, 0, 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current frame's value to given frame
//
///////////////////////////////////////////////////////////////////////////////

void Movie::currentFrame( const Frame& frame )
{
  _frames.at( _currentFrame ) = frame;

  this->modified( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the current frame
//
///////////////////////////////////////////////////////////////////////////////

const Frame& Movie::currentFrame() const
{
  return this->frame( _currentFrame );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Return the current frame
//
///////////////////////////////////////////////////////////////////////////////

Frame& Movie::currentFrame()
{
  return this->frame( _currentFrame );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current frame to the first frame
//
///////////////////////////////////////////////////////////////////////////////

void Movie::firstFrame()
{
  this->setCurrentFrame ( 0 );
}
 

///////////////////////////////////////////////////////////////////////////////
//
//  Go to the previous frame
//
///////////////////////////////////////////////////////////////////////////////

void Movie::previousFrame()
{
  // If we aren't at the beginning...
  if( _currentFrame != 0 )
    this->setCurrentFrame ( --_currentFrame );
}
  

///////////////////////////////////////////////////////////////////////////////
//
//  Go to the next frame
//
///////////////////////////////////////////////////////////////////////////////

void Movie::nextFrame()
{
  // If we aren't at the end...
  if( _currentFrame != _frames.size() - 1 )
    this->setCurrentFrame ( ++_currentFrame );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go to the last frame
//
///////////////////////////////////////////////////////////////////////////////

void Movie::lastFrame()
{
  this->setCurrentFrame ( _frames.size() - 1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return frame at the given index
//
///////////////////////////////////////////////////////////////////////////////

const Frame& Movie::frame ( unsigned int index ) const
{
  if( _frames.empty() )
    throw std::runtime_error ("Error 3941818088: The movie is empty." );

  Frames::const_iterator i = _frames.begin();
  i += index;

  return *i;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Return frame at the given index
//
///////////////////////////////////////////////////////////////////////////////

Frame& Movie::frame ( unsigned int index )
{
  if( _frames.empty() )
    throw std::runtime_error ("Error 3941818088: The movie is empty." );

  Frames::iterator i = _frames.begin();
  i += index;

  return *i;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Add a frame to the beginning
//
///////////////////////////////////////////////////////////////////////////////

void Movie::prependFrame ( const Frame &frame )
{
  _frames.insert( _frames.begin(), frame );

  this->modified( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a frame the the end
//
///////////////////////////////////////////////////////////////////////////////

void Movie::appendFrame ( const Frame &frame )
{
  _frames.push_back ( frame );

  this->modified( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Erease given frame
//
///////////////////////////////////////////////////////////////////////////////

void Movie::removeFrame ( unsigned int index )
{
  if( _frames.empty() )
    throw std::runtime_error ("Error 2175264157: The movie is empty." );

  if( index > _frames.size() - 1 )
    throw std::runtime_error ( "Error 1560670104: Index out of range." );

  Frames::iterator iter ( _frames.begin() );
  iter += index;

  _frames.erase( iter );

  if( _currentFrame == index )
    this->previousFrame();

  this->modified( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play the movie forwards
//
///////////////////////////////////////////////////////////////////////////////

void Movie::playForward( Usul::Interfaces::IUnknown* caller )
{
  do
  {
    this->_forward( caller );

    // If we have stopped, breack out of the loop.
    if ( this->stopped() )
    {
      break;
    }

    // reset lastU and current frame if we are looping.
    if ( _loop )
    {
      _lastU = 0.0;
      _currentFrame = 0;
    }

  } while ( _loop );

  if( !this->stopped() && !this->paused() )
    _currentFrame = _frames.size() - 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play the movie in reverse
//
///////////////////////////////////////////////////////////////////////////////

void Movie::playReverse( Usul::Interfaces::IUnknown* caller )
{
  do
  {
    this->_reverse( caller );

    // If we have stopped, breack out of the loop.
    if ( this->stopped() )
    {
      break;
    }

    // reset lastU and current frame if we are looping.
    if ( _loop )
    {
      _lastU = 1.0;
      _currentFrame = _frames.size() - 1;
    }

  } while ( _loop );

  if( !this->stopped() && !this->paused() )
    _currentFrame = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start a new Frame
//
///////////////////////////////////////////////////////////////////////////////

void Movie::ParserHelper::startFrame ( const std::string &s )
{
  if ( !_frame )
    _frame = new Frame();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the frame's distance
//
///////////////////////////////////////////////////////////////////////////////

void Movie::ParserHelper::setDistance ( const std::string &s )
{
  _frame->setDistance( XML::Convert<float>() ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the center
//
///////////////////////////////////////////////////////////////////////////////

void Movie::ParserHelper::startCenter ( const std::string &s )
{
  _center[0] = _center[1] = _center[2] = 0.0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the center's X value
//
///////////////////////////////////////////////////////////////////////////////

void Movie::ParserHelper::getCenterX ( const std::string &s )
{
  _center[0] = XML::Convert< float > () ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the center's Y value
//
///////////////////////////////////////////////////////////////////////////////

void Movie::ParserHelper::getCenterY ( const std::string &s )
{
  _center[1] = XML::Convert< float > () ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the center's Z value
//
///////////////////////////////////////////////////////////////////////////////

void Movie::ParserHelper::getCenterZ ( const std::string &s )
{
  _center[2] = XML::Convert< float > () ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the frames center
//
///////////////////////////////////////////////////////////////////////////////

void Movie::ParserHelper::finishCenter ( const std::string &s )
{
  _frame->setCenter( _center );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the frames rotation
//
///////////////////////////////////////////////////////////////////////////////

void Movie::ParserHelper::startRotation ( const std::string &s )
{
  _rotation[0] = _rotation[1] = _rotation[2] = _rotation[3] = 0.0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the rotation's X value
//
///////////////////////////////////////////////////////////////////////////////

void Movie::ParserHelper::getRotationX ( const std::string &s )
{
  _rotation[0] = XML::Convert< float > () ( s );
}
    

///////////////////////////////////////////////////////////////////////////////
//
//  Get the rotation's Y value
//
///////////////////////////////////////////////////////////////////////////////

void Movie::ParserHelper::getRotationY ( const std::string &s )
{
  _rotation[1] = XML::Convert< float > () ( s );
}
      

///////////////////////////////////////////////////////////////////////////////
//
//  Get the rotation's Z value
//
///////////////////////////////////////////////////////////////////////////////

void Movie::ParserHelper::getRotationZ ( const std::string &s )
{
  _rotation[2] = XML::Convert< float > () ( s );
}
      

///////////////////////////////////////////////////////////////////////////////
//
//  Get the rotation's W value
//
///////////////////////////////////////////////////////////////////////////////

void Movie::ParserHelper::getRotationW ( const std::string &s )
{
  _rotation[3] = XML::Convert< float > () ( s );
}
      

///////////////////////////////////////////////////////////////////////////////
//
//  Set the frame's rotation
//
///////////////////////////////////////////////////////////////////////////////

void Movie::ParserHelper::finishRotation ( const std::string &s )
{
  _frame->setRotation( _rotation );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the frame's name
//
///////////////////////////////////////////////////////////////////////////////

void Movie::ParserHelper::getName ( const std::string &s )
{
  _frame->setName ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a comment
//
///////////////////////////////////////////////////////////////////////////////

void Movie::ParserHelper::getComment ( const std::string &s )
{
  _frame->addComment( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the frame to the frame list
//
///////////////////////////////////////////////////////////////////////////////

void Movie::ParserHelper::finishFrame ( const std::string &s )
{
  _frames.push_back( *_frame );
  delete _frame;
  _frame = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool Movie::canExport ( const std::string &file ) const
{
  return this->canSave ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool Movie::canInsert ( const std::string &file ) const
{
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool Movie::canOpen   ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "ofa" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool Movie::canSave   ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "ofa" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void Movie::clear ( Unknown *caller )
{
  _frames.clear();
  _currentFrame = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the default GUI.
//
///////////////////////////////////////////////////////////////////////////////

void Movie::createDefaultGUI ( Unknown *caller )
{
  if( this->delegate() )
    this->delegate()->createDefaultGUI( this, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

Movie::Filters Movie::filtersExport() const
{
  return this->filtersSave();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

Movie::Filters Movie::filtersInsert() const
{
  return this->filtersOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

Movie::Filters Movie::filtersOpen() const
{
  Filters filters;
  filters.push_back ( Filter ( "Animation File (*.ofa)", "*.ofa" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

Movie::Filters Movie::filtersSave() const
{
  Filters filters;
  filters.push_back ( Filter ( "Animation File (*.ofa)", "*.ofa" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing document's data.
//
///////////////////////////////////////////////////////////////////////////////

void Movie::read ( const std::string &filename, Unknown *caller )
{
  // Typedefs.
  typedef XML::Config::Policy<>                 Policy;
  typedef void (ParserHelper::*Function) ( const std::string &s );
  typedef Usul::Adaptors::MemberFunction < ParserHelper*, Function > MemFun;
  typedef XML::Node < Policy >                  Node;
  typedef XML::Callback::BuildTree < Node >     BuildTree;
  typedef XML::Reader < Policy, BuildTree >     Reader;
  typedef XML::Visitor < Policy, Node, MemFun > Visitor;

  //Clear the old values.
  _frames.clear();

  XML::File < Policy > file ( filename );

  ParserHelper parserHelper ( _frames );

  Reader reader;
  reader.read ( file.begin(), file.end() );

  Visitor visitor;
  visitor.add ( Visitor::NODE_START, "Movie/frame",            MemFun ( &parserHelper, &ParserHelper::startFrame     ) );
  visitor.add ( Visitor::NODE_START, "Movie/frame/center",     MemFun ( &parserHelper, &ParserHelper::startCenter    ) );
  visitor.add ( Visitor::NODE_START, "Movie/frame/center/x",   MemFun ( &parserHelper, &ParserHelper::getCenterX     ) );
  visitor.add ( Visitor::NODE_START, "Movie/frame/center/y",   MemFun ( &parserHelper, &ParserHelper::getCenterY     ) );
  visitor.add ( Visitor::NODE_START, "Movie/frame/center/z",   MemFun ( &parserHelper, &ParserHelper::getCenterZ     ) );
  visitor.add ( Visitor::NODE_END,   "Movie/frame/center",     MemFun ( &parserHelper, &ParserHelper::finishCenter   ) );
  visitor.add ( Visitor::NODE_START, "Movie/frame/distance",   MemFun ( &parserHelper, &ParserHelper::setDistance    ) );
  visitor.add ( Visitor::NODE_START, "Movie/frame/rotation",   MemFun ( &parserHelper, &ParserHelper::startRotation  ) );
  visitor.add ( Visitor::NODE_START, "Movie/frame/rotation/x", MemFun ( &parserHelper, &ParserHelper::getRotationX   ) );
  visitor.add ( Visitor::NODE_START, "Movie/frame/rotation/y", MemFun ( &parserHelper, &ParserHelper::getRotationY   ) );
  visitor.add ( Visitor::NODE_START, "Movie/frame/rotation/z", MemFun ( &parserHelper, &ParserHelper::getRotationZ   ) );
  visitor.add ( Visitor::NODE_START, "Movie/frame/rotation/w", MemFun ( &parserHelper, &ParserHelper::getRotationW   ) );
  visitor.add ( Visitor::NODE_END,   "Movie/frame/rotation",   MemFun ( &parserHelper, &ParserHelper::finishRotation ) );
  visitor.add ( Visitor::NODE_START, "Movie/frame/name",       MemFun ( &parserHelper, &ParserHelper::getName        ) );
  visitor.add ( Visitor::NODE_START, "Movie/frame/comment",    MemFun ( &parserHelper, &ParserHelper::getComment     ) );
  visitor.add ( Visitor::NODE_END,   "Movie/frame",            MemFun ( &parserHelper, &ParserHelper::finishFrame    ) );

  reader.callback().root()->accept ( visitor );

  _currentFrame = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name. Does not rename this document.
//
///////////////////////////////////////////////////////////////////////////////

void Movie::write ( const std::string &filename, Unknown *caller ) const
{
  Usul::File::Temp temp;

  std::ostream &out = temp.stream();

  out << "<Movie>" << std::endl;
  for ( Frames::const_iterator i = _frames.begin(); i != _frames.end(); ++i )
  {
    out << "<frame>\n"
      << "\t<center>\n" 
      << "\t\t<x>" << i->getCenter()[0] << "</x>\n"
      << "\t\t<y>" << i->getCenter()[1] << "</y>\n"
      << "\t\t<z>" << i->getCenter()[2] << "</z>\n"
      << "\t</center>\n"
      << "\t<distance>" << i->getDistance() << "</distance>\n"
      << "\t<rotation>\n"
      << "\t\t<x>" << i->getRotation()[0] << "</x>\n"
      << "\t\t<y>" << i->getRotation()[1] << "</y>\n"
      << "\t\t<z>" << i->getRotation()[2] << "</z>\n"
      << "\t\t<w>" << i->getRotation()[3] << "</w>\n"
      << "\t</rotation>\n"
      << "\t<name>" << i->getName() << "</name>\n";
      for( Frame::const_iterator iter = i->begin(); iter != i->end(); ++iter )
        out << "\t<comment>" << *iter << "</comment>\n";
      out << "</frame>\n";
  }
  out << "</Movie>";

  temp.rename( filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the percentage of the movie that had played.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Movie::getPercentageDone () const
{
  return _lastU * 100;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play forward.
//
///////////////////////////////////////////////////////////////////////////////

void Movie::_forward ( IUnknown* caller )
{
  this->_changeMode ( PLAYING_FORWARDS );
  this->_play ( _frames.begin(), _frames.end(), caller );
  
  if( !this->stopped() )
    this->_changeMode ( IDLE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play in reverse.
//
///////////////////////////////////////////////////////////////////////////////

void Movie::_reverse ( IUnknown* caller )
{
  this->_changeMode ( PLAYING_REVERSE );

  // Change lastU for playing reverse works.
  _lastU = 1.0 - _lastU;

  this->_play ( _frames.rbegin(), _frames.rend(), caller );

  // Set lastU so playing forwards works later.
  _lastU = 1.0 - _lastU;
  
  if( !this->stopped() )
    this->_changeMode ( IDLE );
}
 

///////////////////////////////////////////////////////////////////////////////
//
//  Stop playing.
//
///////////////////////////////////////////////////////////////////////////////

void Movie::stop()
{
  this->_changeMode( STOPPED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pause playing.
//
///////////////////////////////////////////////////////////////////////////////

void Movie::pause()
{
  this->_changeMode ( PAUSED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Un-pause //TODO
//
///////////////////////////////////////////////////////////////////////////////

void Movie::unpause()
{
  if( this->paused() )
  {
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to play in either direction.
//
///////////////////////////////////////////////////////////////////////////////

template < class Iter > void Movie::_play ( Iter begin, Iter end, IUnknown* caller )
{
  // Return if nothing to animate.
  if( begin == end )
    return;

  IndependentSequence params;

  DependentContainer rot;

  DoubleCurve curve;

  this->_interpolate( begin, end, curve, rot, params  );

  // Return if we don't have any parameters.
  if( params.empty() )
    return;

  //Get the starting u value
  Parameter startVar ( _lastU );

  // Animate through the path.
  this->_animatePath ( curve, rot, params, startVar, _segTime, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate from one position and orientation to another.
//
///////////////////////////////////////////////////////////////////////////////

void Movie::_animatePath ( const DoubleCurve &positions,
                           const DependentContainer &rotations,
                           const IndependentSequence &params,
                           const Parameter& startU,
                           const Parameter& segTime,
                           IUnknown* caller )
{
  typedef DoubleCurve::Vector Point;
  typedef DoubleCurve::SizeType SizeType;
  typedef IndependentSequence::const_iterator ConstIterator;
  typedef std::greater<Parameter> IsGreater;
  typedef Usul::Interfaces::IActiveView        IActiveView;
  typedef Usul::Interfaces::ITrackball         ITrackball;
  typedef Usul::Interfaces::IRedraw            IRedraw;
  typedef Usul::Interfaces::IFlushEvents       IFlushEvents;

  // Get the needed interfaces.
  IActiveView::ValidQueryPtr   activeView ( caller );
  ITrackball::ValidQueryPtr    trackball ( activeView->getActiveView() );
  IRedraw::ValidQueryPtr       redraw ( activeView->getActiveView() );
  IFlushEvents::ValidQueryPtr  flush ( caller );

  // Should be true.
  USUL_ASSERT ( rotations.size() == params.size() );

  // Handle bad input.
  if ( params.empty() || 0 == positions.numControlPoints() )
    throw std::runtime_error ( "Error 3536980075: Bad parameters or positions." );

  // Determine times.
  Parameter duration ( segTime * params.size() );

  duration *= ( positions.lastKnot() - startU ) / positions.lastKnot();
  Parameter start ( Usul::System::Clock::milliseconds() );
  Parameter stop ( start + duration );
  
  // Initialize.
  ConstIterator iu = params.begin();

  // Make a point for the position and rotation.
  Point pos ( positions.dimension() );
  DependentSequence r0 ( rotations.size(), 0 );
  DependentSequence r1 ( rotations.size(), 0 );

  //The independent variable.
  Parameter u ( 0.0 );

  // Loop until out of range.
  while ( Usul::System::Clock::milliseconds() < stop )
  {
    //If we need to stop, return
    if( this->stopped() || this->paused() )
      return;

    // Current clock value
    const Parameter current ( Usul::System::Clock::milliseconds() );

    // Ensure we do not step past end of curve.
    u = std::min ( ( ( current - start ) / duration ), positions.lastKnot() );
    u = startU + ( u * ( ( positions.lastKnot() - startU ) / positions.lastKnot() ) );

    _lastU = u;

    // Evaluate the point.
    GN::Evaluate::point ( positions, u, pos );

    // Find the span in the list of parameters.
    iu = std::find_if ( iu, params.end(), std::bind2nd ( IsGreater(), u ) );

    // Convert this to an index into the list of rotations.
    const SizeType ir1 ( std::distance ( params.begin(), iu ) );
    const SizeType ir0 ( ir1 - 1 );

    // Set the frame for this u
    _currentFrame = ir0;

    // Get the parameters.
    const Parameter u0 ( params.at ( ir0 ) );
    const Parameter u1 ( params.at ( ir1 ) );

    // Get the quaternions.
    r0 = rotations.at ( ir0 );
    r1 = rotations.at ( ir1 );

    // What fraction are we between the adjacent parameters?
    const Parameter fraction ( ( u - u0 ) / ( u1 - u0 ) );

    // Spherical linear interpolation.
    const osg::Quat q0 ( r0.at(0), r0.at(1), r0.at(2), r0.at(3) );
    const osg::Quat q1 ( r1.at(0), r1.at(1), r1.at(2), r1.at(3) );
    osg::Quat quat;
    quat.slerp ( fraction, q0, q1 );

    // Set the trackball.
    const osg::Vec3 center ( pos.at(0), pos.at(1), pos.at( 2 ) );
    trackball->setTrackball ( center, pos.at(3), quat, true, true );

    // Add a chore to re-paint.    
    redraw->addChoreRedraw();

    // Process events.
    flush->flushEventQueue();
  }

  // Ensure that we end up where we are supposed to be. If we don't do this, 
  // then depending on how long things take in the above loop, the model may 
  // not be left in the final position.
  GN::Evaluate::point ( positions, 1, pos );
  r0 = rotations.back();
  const osg::Quat quat ( r0.at(0), r0.at(1), r0.at(2), r0.at(3) );
  const osg::Vec3 center ( pos.at(0), pos.at(1), pos.at( 2 ) );
  trackball->setTrackball ( center, pos.at(3), quat, true, true );

  // Redraw now.
  redraw->redraw();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Change the mode
//
///////////////////////////////////////////////////////////////////////////////

void Movie::_changeMode ( Mode mode)
{
  _lastMode = _mode;
  _mode = mode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate needed sequences for playing.
//
///////////////////////////////////////////////////////////////////////////////

template < class Iter > 
void  Movie::_interpolate ( Iter begin, Iter end, DoubleCurve &curve, DependentContainer &rot, IndependentSequence &params )
{
  // For readability.
  typedef DoubleCurve::IndependentType IndependentType;
  typedef DoubleCurve::SizeType SizeType;
  typedef DoubleCurve::ErrorCheckerType ErrorCheckerType;
  typedef DoubleCurve::Power PowerFunctor;
  typedef GN::Algorithms::KnotVector < IndependentSequence, ErrorCheckerType > KnotVectorBuilder;

  // Container for the data.
  DependentContainer points;

  // Loop through the frames.
  for ( Iter i = begin; i != end; ++i )
  {
    // Get the frame data.
    const Frame &frame = *i;

    const osg::Vec3 &c = frame.getCenter();
    double d ( frame.getDistance() );
    const osg::Quat &r = frame.getRotation();

    //if ( c != osg::Vec3 ( 0.0, 0.0, 0.0 ) && d != 0.0 && r != osg::Quat ( 0.0, 0.0, 0.0, 0.0 ) )
    if ( d != 0.0 && r != osg::Quat ( 0.0, 0.0, 0.0, 0.0 ) ) // TODO, is this needed?
    {
      // We push_back because we skip frames that are not trackballs.
      DependentSequence point;
      point.push_back ( c[0] );
      point.push_back ( c[1] );
      point.push_back ( c[2] );
      point.push_back ( d    );
      point.push_back ( r[0] );
      point.push_back ( r[1] );
      point.push_back ( r[2] );
      point.push_back ( r[3] );
      points.push_back ( point );
    }
  }

  // Make sure there are no adjacent points that are identical.
  typedef DependentContainer::IsEqual IsEqualPoint;
  points.erase ( std::unique ( points.begin(), points.end(), IsEqualPoint() ), points.end() );

  // Now that we have unique frames, make sure there are enough.
  if ( points.size() < Movie::DoubleCurve::Limits::MIN_NUM_CTR_PTS )
    return;

  // Make the parameters evenly spaced.
  GN::Algorithms::fill ( params, points.size(), 0, 1 );

  // Should be true.
  USUL_ASSERT ( params.size() == points.size() );

  // Transpose so that the first index of "points" is the dimension.
  Usul::Math::transpose ( points );

  // Make the knot vector. Size it for interpolation.
  IndependentSequence knots;
  const SizeType cubic ( 4 );
  const SizeType order ( std::min<SizeType> ( cubic, params.size() ) ); // TODO, the order should be a preference.
  knots.resize ( params.size() + order );
  KnotVectorBuilder::build ( params, order, knots );

  // Make a copy of the points.
  DependentContainer &pos = points;
  rot = points;

  // Trim the two containers of data. Note: the 4 is related to the 
  // container dimension (see above), not the order.
  pos.erase ( pos.begin() + 4, pos.end() );
  rot.erase ( rot.begin(), rot.begin() + 4 );

  // Note: rot.at(0) should point to the 4 components of the quaternian.
  Usul::Math::transpose ( rot );

  // Interpolate the positions.
  GN::Interpolate::global ( order, params, knots, pos, curve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the u value that the movie will start playing from.
//
///////////////////////////////////////////////////////////////////////////////

Frame Movie::lastU ( Parameter u ) 
{ 
  _lastU = u; 

  IndependentSequence params;

  DependentContainer rot;

  DoubleCurve curve;

  this->_interpolate( _frames.begin(), _frames.end(), curve, rot, params  );

  // Find the span in the list of parameters.
  typedef IndependentSequence::const_iterator ConstIterator;
  typedef std::greater<Parameter> IsGreater;
  ConstIterator iu = std::find_if ( params.begin(), params.end(), std::bind2nd ( IsGreater(), u ) );

  // Convert this to an index into the list of rotations.
  typedef DoubleCurve::SizeType SizeType;
  const SizeType ir1 ( std::distance< ConstIterator > ( params.begin(), iu ) );
  const SizeType ir0 ( ir1 - 1 );

  _currentFrame = ir0;

  Frame frame;

  if ( u < 1.0 )
  {
    typedef DoubleCurve::Vector Point;
    typedef DoubleCurve::IndependentType IndependentType;
    typedef DoubleCurve::SizeType SizeType;

    Point pos ( curve.dimension() );
    DependentSequence r0 ( rot.size(), 0 );
    DependentSequence r1 ( rot.size(), 0 );
    
    // Evaluate the point.
    GN::Evaluate::point ( curve, u, pos );

    // Get the parameters.
    const Parameter u0 ( params.at ( ir0 ) );
    const Parameter u1 ( params.at ( ir1 ) );

      // Get the quaternions.
    r0 = rot.at ( ir0 );
    r1 = rot.at ( ir1 );

    // What fraction are we between the adjacent parameters?
    const Parameter fraction ( ( u - u0 ) / ( u1 - u0 ) );

    // Spherical linear interpolation.
    const osg::Quat q0 ( r0.at(0), r0.at(1), r0.at(2), r0.at(3) );
    const osg::Quat q1 ( r1.at(0), r1.at(1), r1.at(2), r1.at(3) );
    osg::Quat quat;
    quat.slerp ( fraction, q0, q1 );

    // Get the center
    const osg::Vec3 center ( pos.at(0), pos.at(1), pos.at( 2 ) );

    
    frame.setCenter( center );
    frame.setRotation( quat );
    frame.setDistance ( pos.at(3) );
  }
  else
  {
    frame = _frames.back();
  }

  return frame;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current frame.  Set the u value.
//
///////////////////////////////////////////////////////////////////////////////

void Movie::setCurrentFrame ( unsigned int i ) 
{ 
  _currentFrame = i;

  IndependentSequence params;

  DependentContainer rot;

  DoubleCurve curve;

  this->_interpolate( _frames.begin(), _frames.end(), curve, rot, params  );

  // Return if we don't have any parameters.
  if( params.empty() )
    return;

  // set the starting u value
  _lastU = params.at( i );
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that we can write.
//
///////////////////////////////////////////////////////////////////////////////

Movie::Filters Movie::filtersWrite() const
{
  Filters filters;

  typedef Usul::Components::Manager Manager;
  typedef Manager::UnknownSet UnknownSet;

  UnknownSet unknowns ( Usul::Components::Manager::instance().getInterfaces ( Usul::Interfaces::IWriteMovieFile::IID ) );

  for ( UnknownSet::const_iterator iter = unknowns.begin(); iter != unknowns.end(); ++iter )
  {
    Usul::Interfaces::IWriteMovieFile::ValidQueryPtr write ( iter->get() );
    Filters f ( write->filtersWrite() );

    filters.insert ( filters.end(), f.begin(), f.end() );
  }

  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the movie.
//
///////////////////////////////////////////////////////////////////////////////

void Movie::writeMovie ( const Filename& filename, const Filenames& filenames )
{
  typedef Usul::Components::Manager Manager;
  typedef Manager::UnknownSet UnknownSet;

  UnknownSet unknowns ( Usul::Components::Manager::instance().getInterfaces ( Usul::Interfaces::IWriteMovieFile::IID ) );

  for ( UnknownSet::const_iterator iter = unknowns.begin(); iter != unknowns.end(); ++iter )
  {
    Usul::Interfaces::IWriteMovieFile::ValidQueryPtr write ( iter->get() );

    if ( write->canWrite ( filename ) )
    {
      write->writeMovie ( filename, filenames );
    }
  }
}
