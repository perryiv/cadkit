#include "OctTreeNode.h"

#include "Usul/Math/MinMax.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Errors/Assert.h"
#include "Usul/File/Stats.h"
#include "Usul/File/Temp.h"
#include "Usul/File/Path.h"
#include "Usul/Strings/Format.h"
#include "Usul/Convert/Convert.h"
#include "Usul/System/LastError.h"

#include "OsgTools/Lod.h"
#include "OsgTools/State/StateSet.h"

#include "osg/Vec3d"
#include "osg/Plane"
#include "osg/Group"
#include "osg/Geometry"
#include "osg/Geode"
#include "osg/LOD"

#include <limits>
#include <fstream>

#include <Math.h>

//USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( OctTreeNode, OctTreeNode::BaseClass );

 long OctTreeNode::_streamCount( 0 );
 unsigned long OctTreeNode::_numerator( 0 );
 unsigned long OctTreeNode::_denominator( 0 );
 unsigned int OctTreeNode::_numLeafNodes( 0 );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

OctTreeNode::OctTreeNode ( StreamBufferPtr buffer, const std::string &tempPath ):
  _bb(),
  _children(),
  _points( 0x0 ),
  _type( POINT_HOLDER ),
  _capacity( 1000 ),
  _useLOD( true ),
  _infile( 0x0 ),
  _outfile( 0x0 ),
  _tempFilename(),
  _numPoints( 0 ),
  _streamBuffer ( buffer ),
  _tempPath( tempPath )
{
  
  _tempFilename = Usul::Strings::format ( tempPath, '/', Usul::Convert::Type< unsigned int, std::string >::convert ( reinterpret_cast < unsigned int > ( this ) ), ".tmp" );
#if 1
   _lodDefinitions.push_back( 1 );
   _lodDefinitions.push_back( 10 );
   _lodDefinitions.push_back( 25 );
   _lodDefinitions.push_back( 50 );
   _lodDefinitions.push_back( 75 );
   _lodDefinitions.push_back( 100 );
   _lodDefinitions.push_back( 150 );
   _lodDefinitions.push_back( 300 );
   _lodDefinitions.push_back( 600 );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

OctTreeNode::~OctTreeNode()
{
  // Safely close and delete streams.
  this->_closeInputStream();
  this->_closeOutputStream();

  // Remove the file if it exists.
  Usul::File::remove ( _tempFilename, false, 0x0 );
}


/////////////////
//  PUBLIC
/////////////////


///////////////////////////////////////////////////////////////////////////////
//
//  Set value to use level of detail or not
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::useLOD( bool value )
{
  Guard guard ( this->mutex() );
  _useLOD = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounds of the bounding box
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::boundingBox( const osg::BoundingBox &bb )
{
  Guard guard ( this->mutex() );
  this->_bb = bb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the type of the cell
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::type( unsigned int type )
{
  Guard guard ( this->mutex() );
  this->_type = type;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a cell to this tree node
//
///////////////////////////////////////////////////////////////////////////////

bool OctTreeNode::add ( OctTreeNode::Point p )
{
  Guard guard ( this->mutex() );

  //check my bb to determine if the point fits inside me
  if( true == this->_contains( p ) )
  {
    //insert node into _cells 
#if 0
    if( _type == POINT_HOLDER && _list->size() < _capacity )
    {
      _list->push_back( p );
    }
    else
    {
      //find the bounds of the child that contains the cell
      this->_insertOrCreateChildren( p );
    }
#else
    
    // Get the output stream.
    std::ofstream &outfile ( this->_getOutputStream() );

    // Write the point.
    outfile.write ( reinterpret_cast<char *> ( &p ), sizeof ( OctTreeNode::Point ) ); 

    // Increment the count.
    ++_numPoints;

    return true;
    
#endif    

  }
  else
  {

    std::cout << "Failed to add point: ( " << p.x() << ", " << p.y() << ", " << p.z() << " ) in OctTreeNode with bounds"
              << " min( " << _bb.xMin() << ", " << _bb.yMin() << ", " << _bb.zMin() 
              << " ) max( " << _bb.xMax() << ", " << _bb.yMax() << ", " << _bb.zMax() << " )" << std::endl;
    
    return false;
  }

  
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::capacity( unsigned int level )
{
  Guard guard ( this->mutex() );
  _capacity = level;
  if( _children.size() != 0 )
  {
    for( unsigned int i = 0; i < _children.size(); ++i )
    {
      _children.at( i )->capacity( level );
    }
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Get the Bounding Box for this node
//
///////////////////////////////////////////////////////////////////////////////

osg::BoundingBox OctTreeNode::boundingBox()
{ 
  Guard guard ( this->mutex() );
  return _bb; 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the children of this node
//
///////////////////////////////////////////////////////////////////////////////

OctTreeNode::Children OctTreeNode::children()
{ 
  Guard guard ( this->mutex() );
  return _children; 
}

  
///////////////////////////////////////////////////////////////////////////////
//
//  Get the type of this node
//
///////////////////////////////////////////////////////////////////////////////

unsigned int OctTreeNode::type()
{ 
  Guard guard ( this->mutex() );
  return _type; 
}

  
///////////////////////////////////////////////////////////////////////////////
//
//  Get the capacity level of this node
//
///////////////////////////////////////////////////////////////////////////////

unsigned int OctTreeNode::capacity()
{ 
  Guard guard ( this->mutex() );
  return _capacity; 
}


osg::Node* OctTreeNode::buildScene( Unknown *caller, Unknown *progress )
{
  Guard guard ( this->mutex() );
  typedef osg::ref_ptr< osg::Geometry > GeometryPtr;
  typedef osg::ref_ptr< osg::Geode > GeodePtr;
  osg::ref_ptr< osg::LOD > lod ( new osg::LOD );
  osg::ref_ptr< osg::Group > group ( new osg::Group );

  unsigned int numLODs ( _lodDefinitions.size() );

  if( POINT_HOLDER == this->type() )
  {
    // Make sure the points are valid
    if( true == _points.valid() )
    {
      // LOds
      if( _points->size() > 0 )
      {
        for( unsigned int lodLevel = 0; lodLevel < numLODs; ++lodLevel )
        {
          //Points vertices ( new osg::Vec3Array );
          ElementsPtr indices ( new osg::DrawElementsUShort ( osg::PrimitiveSet::POINTS ) );

#if 0
          double lm ( static_cast< double >( _capacity - 1 ) / static_cast< double > ( _numLODs ) );
          unsigned short lodMultiplier = static_cast< unsigned short > ( lm * lodLevel ) + 1;
#else
          unsigned int lodMultiplier ( _lodDefinitions.at( lodLevel ) );
#endif
          indices->reserve( static_cast< unsigned int > ( ( _points->size() / lodMultiplier ) ) );
          for( unsigned int i = 0; i < _points->size(); i += lodMultiplier )
          {
            indices->push_back( i );
          }
          GeometryPtr geometry ( new osg::Geometry ); 
          geometry->setVertexArray( _points.get() );
          geometry->addPrimitiveSet( indices.get() );
        
          GeodePtr geode ( new osg::Geode );
          geode->addDrawable( geometry.get() );
          
          // High level LODs
          float minLevel ( static_cast< float > ( lodLevel ) / static_cast< float > ( numLODs ) );
          float maxLevel ( ( static_cast< float > ( lodLevel ) + 1.0f ) / static_cast< float > ( numLODs ) );
          float minDistance = Usul::Math::maximum( ( minLevel * _distance ), this->getBoundingRadius() * minLevel );
          float maxDistance = Usul::Math::maximum( ( maxLevel * _distance ), this->getBoundingRadius() * maxLevel );
          if( lodLevel == numLODs - 1 )
          {
            maxDistance = Usul::Math::maximum( ( maxLevel * _distance ), std::numeric_limits< float >::max() );
          }

          lod->addChild( geode.get(), minDistance, maxDistance );
        }

        // add the lods to the main group
        group->addChild( lod.get() );
      }
    }
  }
  else if( NODE_HOLDER == this->type() )
  { 
    for( unsigned int i = 0; i < _children.size(); ++i )
    {
      group->addChild( _children.at( i )->buildScene() );
    } 
  }

  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check to see if the point is contained within this bounding box
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::distance( float d )
{
  Guard guard ( this->mutex() );
  _distance = d;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the diagnol distance of the bounding box of this node
//
///////////////////////////////////////////////////////////////////////////////

float OctTreeNode::getBoundingRadius()
{
  Guard guard ( this->mutex() );
  Usul::Math::Vec3f min ( _bb.xMin(), _bb.yMin(), _bb.zMin() );
  Usul::Math::Vec3f max ( _bb.xMax(), _bb.yMax(), _bb.zMax() );
  float d = max.distanceSquared( min );
  d = sqrt( d );
  return d;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the vector of points from the linked list
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::buildVectors()
{
  Guard guard ( this->mutex() );
#if 0
  if( true == _children.empty() )
  {
    _points->reserve( _list->size() );
    for( LinkedList::iterator iter = _list->begin(); iter != _list->end(); ++iter )
    {
      _points->push_back( (*iter) );
    }
    _list->clear();
    delete _list;
    _list = 0x0;
  }
  else
  {
    for( unsigned int i = 0; i < _children.size(); ++i )
    {
      _children.at( i )->buildVectors();
    }
  }
#else
  if( POINT_HOLDER == this->type() && _numPoints > 0 )
  {
    
    //this->_openTempFileForRead();
    std::ifstream &infile ( this->_getInputStream() );

    if( false == _points.valid() )
    {
      _points = new osg::Vec3Array;
      _points->resize ( _numPoints, OctTreeNode::Point ( 0.0, 0.0, 0.0 ) );
    }

    unsigned int size = _points->size() * sizeof( OctTreeNode::Point );

    // Sanity check.
    USUL_ASSERT ( size == Usul::File::size ( _tempFilename ) );

    infile.read( reinterpret_cast<char *> ( &((*_points)[0]) ), size );

    // Close the streams
    this->_closeInputStream();
    this->_closeOutputStream();

    // Increment the leaf node count
    ++_numLeafNodes;

  }
  else
  {
    for( unsigned int i = 0; i < _children.size(); ++i )
    {
      _children.at( i )->buildVectors();
    }
  }
#endif
}



///////////////////////////////////////////////////////////////////////////////
//
//  Close the stream
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::closeInputStream()
{
  this->_closeInputStream();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Close the stream
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::closeOutputStream()
{
  this->_closeOutputStream();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Populate the octree.
//
///////////////////////////////////////////////////////////////////////////////

bool OctTreeNode::split( Usul::Documents::Document* document, Unknown *caller, Unknown *progress )
{
  Guard guard ( this->mutex() );

  // Debug info
  //std::cout << "Number of points before split is: " << _numPoints << std::endl;

  if( _numPoints > _capacity )
  {
   
    // Update progress
    ++_numerator;
    document->setProgressBar( true, _numerator, _denominator, progress );
    //std::cout << "Splitting... numerator is: " << _numerator << " denominator is: " << _denominator << std::endl;

    // Create children and populate them with my points
    this->_split();

    // Close all the open streams for each child
    this->_closeChildrenStreams();

    // Safely close and delete streams.
    this->_closeInputStream();
    this->_closeOutputStream();

    // Remove the file if it exists.
    Usul::File::remove ( _tempFilename, false, 0x0 );

    // Tell my children to split if needed
    for( unsigned int i = 0; i < _children.size(); ++i )
    {
      _children.at( i )->split( document, caller, progress );
    }
    return true;
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the progress for the split function
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::initSplitProgress( unsigned long n, unsigned long d )
{
  Guard guard ( this->mutex() );
  _numerator = n;
  _denominator = d;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the number of points in this node
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint64 OctTreeNode::getNumPoints()
{
  Guard guard ( this->mutex() );  
  return _numPoints;
}


/////////////////
//  PROTECTED
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Check to see if the point is contained within this bounding box
//
///////////////////////////////////////////////////////////////////////////////

bool OctTreeNode::_contains( OctTreeNode::Point p )
{
  Guard guard ( this->mutex() );
  if( _bb.contains( p ) )
    return true;
  else
    return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert a point into a child node
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::_insertOrCreateChildren( OctTreeNode::Point p )
{
  Guard guard ( this->mutex() );
  if( _children.size() < 8 )
  {
    this->_createChildren();
    this->_reorder();
#if 0
    _list->clear();
    delete _list;
    _list = 0x0;
#else
    // Reset the number of points
    _numPoints = 0;

    // Safely close and delete streams.
    this->_closeInputStream();
    this->_closeOutputStream();

    // Remove the file if it exists.
    Usul::File::remove ( _tempFilename, false, 0x0 );

#endif
  }

  this->_addCellToChild( p );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reorder the points so that they are distributed to all children
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::_reorder()
{
  Guard guard ( this->mutex() );

  // Make sure the streams are both closed.
  this->_closeInputStream();
  this->_closeOutputStream();

  // Sanity check.
  USUL_ASSERT ( ( _numPoints * sizeof ( OctTreeNode::Point ) ) == Usul::File::size ( _tempFilename ) );

  // Get input stream.
  std::ifstream &infile ( this->_getInputStream() );

  // Point used in the loop.
  OctTreeNode::Point p ( 0.0, 0.0, 0.0 );

  // Loop through the points.
  for( unsigned int i = 0; i < _numPoints; ++i )
  {
    infile.read ( reinterpret_cast<char *> ( &p ), sizeof ( OctTreeNode::Point ) );
    this->_addCellToChild( p );
  }

  // Sanity check.
  USUL_ASSERT ( EOF == infile.peek() );

  // close the input stream
  this->_closeInputStream();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Create child cells and set the bounds of each one
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::_createChildren()
{
  Guard guard ( this->mutex() );
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Split my bounding box into 8 to distribute to my children.  
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::_split()
{
  Guard guard ( this->mutex() );


  BoundingBoxVec b ( 0 );
  osg::Vec3d min = _bb.corner( 0 );
  osg::Vec3d max = _bb.corner( 7 );

  // box 0
  osg::BoundingBox b0( min, osg::Vec3d( min.x() + ( ( max.x() - min.x() ) / 2 ), 
                                        min.y() + ( ( max.y() - min.y() ) / 2 ),
                                        min.z() + ( ( max.z() - min.z() ) / 2 ) ) );
  // box 7
  osg::BoundingBox b7( b0.corner( 7 ), max );

  // box 4
  osg::BoundingBox b4( b0.corner( 4 ), b7.corner( 4 ) );

  // box 3
  osg::BoundingBox b3( b0.corner( 3 ), b7.corner( 3 ) ); 

  b.push_back( b0 );
  b.push_back( osg::BoundingBox( b0.corner( 1 ),  b7.corner( 1 ) ) );
  b.push_back( osg::BoundingBox( b0.corner( 2 ),  b7.corner( 2 ) ) );
  b.push_back( b3 );
  b.push_back( b4 );
  b.push_back( osg::BoundingBox( b4.corner( 1 ),  b7.corner( 5 ) ) );
  b.push_back( osg::BoundingBox( b4.corner( 2 ),  b7.corner( 6 ) ) );
  b.push_back( b7 );

  // Create the children and set the appropriate bounds for each
  this->type( NODE_HOLDER );
  _children.resize( 8 );

  for( unsigned int i = 0; i < 8; i ++ )
  {
    _children.at( i ) = new OctTreeNode ( _streamBuffer, _tempPath );
    _children.at( i )->distance( _distance );
    _children.at( i )->boundingBox( b.at( i ) );
    _children.at( i )->type( POINT_HOLDER );
    _children.at( i )->capacity( _capacity );

  }

  // send the points to the children
  this->_reorder();

  // Reset the number of points
  _numPoints = 0;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a point to a child node
//
///////////////////////////////////////////////////////////////////////////////

bool OctTreeNode::_addCellToChild( OctTreeNode::Point p )
{
  Guard guard ( this->mutex() );
  bool found = false;
  const unsigned int size = _children.size();
  for( unsigned int c = 0; c < size; ++c )
  {
    if( true == _children.at( c )->boundingBox().contains( p ) )
    {
      _children.at( c )->add( p );
      found = true;
      c = size;
    }
  }
  return found;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return input stream. Make sure other stream is closed and deleted.
//
///////////////////////////////////////////////////////////////////////////////

std::ifstream& OctTreeNode::_getInputStream()
{
  Guard guard ( this );

  // Delete the other stream if needed.
  this->_closeOutputStream();

  // Make the input stream if needed.
  if ( 0x0 == _infile )
  {
    Usul::System::LastError::init();
    
    _infile = new std::ifstream ( _tempFilename.c_str(), std::ifstream::in | std::ifstream::binary );

    if ( false == _infile->is_open() )
    {
      // Output the open stream count
      std::cout << "Number of open streams: " << _streamCount << std::endl;
      std::cout << "System Error: " << Usul::System::LastError::message() << std::endl;
      throw std::runtime_error ( "Error 2917103380: Failed to open file for reading: " + _tempFilename );
    }

    ++_streamCount;
    USUL_ASSERT ( _streamCount >= 0 );

    if ( 0x0 != _streamBuffer.get() )
    {
      StreamBuffer &buffer ( *_streamBuffer );
      _infile->rdbuf()->pubsetbuf ( &buffer[0], buffer.size() - 1 );
    }
  }

  // Return the stream.
  return *_infile;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return output stream. Make sure other stream is closed and deleted.
//
///////////////////////////////////////////////////////////////////////////////

std::ofstream& OctTreeNode::_getOutputStream()
{
  Guard guard ( this );

  // Delete the other stream if needed.
  this->_closeInputStream();

  // Make the input stream if needed.
  if ( 0x0 == _outfile )
  {
    Usul::System::LastError::init();
    _outfile = new std::ofstream ( _tempFilename.c_str(), std::ofstream::out | std::ofstream::binary );

    if ( false == _outfile->is_open() )
    {
      // Output the open stream count
      std::cout << "Number of open streams: " << _streamCount << std::endl;
      std::cout << "System Error: " << Usul::System::LastError::message() << std::endl;
      throw std::runtime_error ( "Error 4217716870: Failed to open file for writing: " + _tempFilename );
    }

    ++_streamCount;
    USUL_ASSERT ( _streamCount >= 0 );
  }

  // Return the stream.
  return *_outfile;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Close the stream
//
///////////////////////////////////////////////////////////////////////////////

void  OctTreeNode::_closeInputStream()
{
  Guard guard ( this );

   // Delete the stream if needed.
  if ( 0x0 != _infile )
  {
    delete _infile;
    _infile = 0x0;

    --_streamCount;
    //USUL_ASSERT ( _streamCount >= 0 );
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Close the stream
//
///////////////////////////////////////////////////////////////////////////////

void  OctTreeNode::_closeOutputStream()
{
  Guard guard ( this );

  // Delete the stream if needed.
  if ( 0x0 != _outfile )
  {
    delete _outfile;
    _outfile = 0x0;

    --_streamCount;
    //USUL_ASSERT ( _streamCount >= 0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close the streams of all children
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::_closeChildrenStreams()
{
  for( unsigned int i = 0; i < _children.size(); ++i )
  {
    _children.at( i )->closeInputStream();
    _children.at( i )->closeOutputStream();
  }
}
