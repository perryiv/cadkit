#include "OctTreeNode.h"
#include "PointLoader.h"

#include "Usul/Math/MinMax.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Errors/Assert.h"
#include "Usul/File/Stats.h"
#include "Usul/File/Temp.h"
#include "Usul/File/Path.h"
#include "Usul/File/Make.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Strings/Format.h"
#include "Usul/Convert/Convert.h"
#include "Usul/System/LastError.h"

#include "OsgTools/Lod.h"
#include "OsgTools/State/StateSet.h"

#include "osgUtil/CullVisitor"

#include "osg/Vec3d"
#include "osg/Plane"
#include "osg/Group"
#include "osg/Geometry"
#include "osg/Geode"
#include "osg/LOD"

#include <limits>
#include <fstream>
#include <stdexcept>

#include <Math.h>

USUL_IMPLEMENT_TYPE_ID ( OctTreeNode );

//USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( OctTreeNode, OctTreeNode::BaseClass );

 long                OctTreeNode::_streamCount  ( 0 );
 unsigned long       OctTreeNode::_numerator    ( 0 );
 unsigned long       OctTreeNode::_denominator  ( 0 );
 Usul::Types::Uint64 OctTreeNode::_numLeafNodes ( 0 );
 unsigned int        OctTreeNode::_treeDepth    ( 0 );
 unsigned int        OctTreeNode::_depthCount   ( 0 );

 ///////////////////////////////////////////////////////////////////////////////
//
//  Cull Callback for rotating the compass based on the current view matrix
//
///////////////////////////////////////////////////////////////////////////////

class CustomLODCallback : public osg::NodeCallback
{
public:
  CustomLODCallback( Usul::Jobs::Manager &jm, const std::string& p, osg::Geode* geode, unsigned int level, osg::BoundingBox bb ) :
          _mutex(),
          _path( p ),
          _geode( geode ),
          _level( level ),
          _fileSize( 0 ),
          _numPoints( 0 ),
          _myJob( 0x0 ),
          _jobLoading( false ),
          _bb ( bb ),
          _jobManager ( jm )
		  {
         // Get the size of the lod file
         _fileSize = Usul::File::size( _path );
         
         if( _fileSize != 0 )
         {
           std::cout << "";
         }
         // number of points in the file
         float np ( static_cast< float > ( _fileSize ) );
         np /= sizeof( OctTreeNode::Point );
         
         // Get the number of points in the lod file
         //Usul::Types::Uint64 numPoints ( static_cast< Usul::Types::Uint64 > ( np )  );
         _numPoints = static_cast< Usul::Types::Uint64 > ( np );
  			
		  }
      ~CustomLODCallback()         
		  {
        // Delete my job
        _myJob->cancel();
        _myJob = 0x0;
		  }

      typedef Usul::Threads::RecursiveMutex Mutex;
      typedef Usul::Threads::Guard<Mutex> Guard;

      virtual void operator()( osg::Node *node, osg::NodeVisitor *nv )
      {
        //Guard guard ( this );

        osgUtil::CullVisitor *cv = dynamic_cast<osgUtil::CullVisitor *>(nv);
        if ( 0x0 == cv )
        {
          this->traverse ( node, nv );
          return;
        }
        // Check to see if we are culled
        if( true == cv->isCulled( _bb ) )
        {
          this->traverse ( node, nv );
          return;
        }
        // Check to see if our points are already loaded
        // If they are don't reload them again and DON'T
        // unload the other children again

        // Get the geometry
        osg::ref_ptr< osg::Geometry > geodeGeometry ( dynamic_cast< osg::Geometry* > ( _geode->getDrawable( 0 ) ) );
        if( 0x0 == geodeGeometry )
        {
          this->traverse( node, nv );
          return;
        }
            
        // Get the vertices
        osg::ref_ptr< osg::Vec3Array > vertices ( dynamic_cast< osg::Vec3Array* > ( geodeGeometry->getVertexArray() ) );
        if( 0x0 == vertices )
        {
          traverse( node, nv );
          return;
        }
        if( vertices->size() == 0 )
        {
          std::cout << "";
        }
        if( _fileSize > 0 )
        {
          std::cout << "";
        }
        if( vertices->size() == 0 && _fileSize > 0 )
        {
          // If my job isn't valid, create it
          if( 0x0 == _myJob )
          {
            // kill the other jobs that my parent has
            osg::ref_ptr< osg::LOD > parent ( dynamic_cast< osg::LOD* > ( _geode->getParent( 0 ) ) );
            if( 0x0 != parent )
            {
              for( unsigned int i = 0; i < parent->getNumChildren(); ++i )
              {
                osg::ref_ptr< osg::Geode > child ( dynamic_cast< osg::Geode* > ( parent->getChild( i ) ) );
                if( 0x0 != child )
                {
                  osg::ref_ptr< CustomLODCallback > callback ( dynamic_cast< CustomLODCallback* > ( child->getCullCallback() ) );
                  if( 0x0 != callback )
                  {
                    callback->cancelJob();
                  }                 
                }
              }
            }

            // Guarding this code
            {
              Guard guard ( this );

              // Create my job
              _myJob = new PointLoader( _path, _numPoints );
            
              // Set the priority level
              _myJob->priority( static_cast< int > ( _level ) );

              // Start my job
              _jobManager.addJob ( _myJob.get() );
            }
           
          }

          if( true == _myJob.valid() && true == _myJob->foundNewData() )
          {
            // Remove the vertices of the other children
            osg::ref_ptr< osg::LOD > parent ( dynamic_cast< osg::LOD* > ( _geode->getParent( 0 ) ) );
            if( 0x0 != parent )
            {
              for( unsigned int i = 0; i < parent->getNumChildren(); ++i )
              {
                osg::ref_ptr< osg::Geode > child ( dynamic_cast< osg::Geode* > ( parent->getChild( i ) ) );
                if( 0x0 != child )
                {
                  osg::ref_ptr< osg::Geometry > childGeometry ( dynamic_cast< osg::Geometry* > ( child->getDrawable( 0 ) ) );
                  if( 0x0 != childGeometry )
                  {
                    Guard guard ( this );

                    // Remove the vertices
                    childGeometry->setVertexArray( new osg::Vec3Array );
                    
                    // dirty the display lists to be sure the scene is rebuilt
                    childGeometry->dirtyDisplayList();
                  }
                }
              }
            }
          
            // Initialize the points holder
            osg::ref_ptr< osg::Vec3Array > points ( _myJob->getData() );
                     
            // Create the Geometry         
            osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry ); 

            // Set the geometry
            geometry->setVertexArray( points.get() );
            geometry->addPrimitiveSet( new osg::DrawArrays ( osg::PrimitiveSet::POINTS, 0, points->size() ) );
          
            // Guarding this part
            {
              Guard guard ( this );
              // Add the points to the geode
              _geode->removeDrawables( 0, _geode->getNumDrawables() );
              _geode->addDrawable( geometry.get() );
            
              // Reset the job
              _myJob = 0x0;
            }
          }  
        }
        traverse( node, nv );
      }
      void cancelJob()
      {
        Guard guard ( this );
        if( true == _myJob.valid() )
        {
          Usul::Jobs::Job::RefPtr job ( dynamic_cast< Usul::Jobs::Job* > ( _myJob.get() ) );
          if( 0x0 != job )
            _jobManager.removeQueuedJob( job );

          _myJob->cancel();
          _myJob = 0x0;
        }

      }
      // Return the mutex. Use with caution.
      Mutex & mutex() const
      {
        return _mutex;
      }

	 private:

    CustomLODCallback ( const CustomLODCallback & );
    CustomLODCallback &operator = ( const CustomLODCallback & );
  
    mutable Mutex                _mutex;
    std::string                  _path;
    osg::ref_ptr< osg::Geode >   _geode;
    unsigned int                 _level;
    Usul::Types::Uint64          _fileSize;
    float                        _numPoints;
    PointLoader::RefPtr          _myJob;
    bool                         _jobLoading;
    osg::BoundingBox             _bb;
    Usul::Jobs::Manager &        _jobManager;
}; 


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

OctTreeNode::OctTreeNode ( Usul::Jobs::Manager *jm, StreamBufferPtr buffer, const std::string &tempPath ):
  _bb(),
  _children(),
  _points( 0x0 ),
  _type( POINT_HOLDER ),
  _capacity( 1000 ),
  _name(),
  _currentLodLevel( 0 ),
  _useLOD( true ),
  _lodDefinitions(),
  _infile( 0x0 ),
  _outfile( 0x0 ),
  _tempFilename(),
  _numPoints( 0 ),
  _streamBuffer ( buffer ),
  _tempPath( tempPath ),
  _distance( 0 ),
  _material( 1.0f, 0.0f, 0.0f, 1.0f ),
  _mutex(),
  _workingDir(),
  _baseName(),
  _jobManager ( jm ),
  _nodeDepth( 0 )
{
  
  _tempFilename = Usul::Strings::format ( tempPath, '/', Usul::Convert::Type< unsigned int, std::string >::convert ( reinterpret_cast < unsigned int > ( this ) ), ".tmp" );

  if ( 0x0 == _jobManager )
    throw std::invalid_argument ( Usul::Strings::format ( "Error 2032454300: null job manager given" ) );

#if 1
   _lodDefinitions.push_back( 1 );
   //_lodDefinitions.push_back( 3 );
   _lodDefinitions.push_back( 10 );
   //_lodDefinitions.push_back( 25 );
   _lodDefinitions.push_back( 50 );
   //_lodDefinitions.push_back( 75 );
   _lodDefinitions.push_back( 100 );
   //_lodDefinitions.push_back( 500 );
   _lodDefinitions.push_back( 1000 );

#endif

#if 0
   _lodDefinitions.push_back( 1 );
   _lodDefinitions.push_back( 3 );
   _lodDefinitions.push_back( 5 );
   _lodDefinitions.push_back( 7 );
   _lodDefinitions.push_back( 9 );
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

  // Clear out any queued jobs
  _jobManager->clearQueuedJobs();

#if 0
  // Remove Lod Files
  for( unsigned int i = 0; i < _lodDefinitions.size(); ++i )
  {
    std::string lodName ( Usul::Strings::format( _workingDir, _name, "LOD", i ) );
    Usul::File::remove ( lodName, false, 0x0 );
  }
#endif

  // Do not delete _jobManager
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
  Guard guard ( this );
  _useLOD = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounds of the bounding box
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::boundingBox( const osg::BoundingBox &bb )
{
  Guard guard ( this );
  this->setInitialBound( osg::BoundingSphere( bb ) );
  this->_bb = bb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the type of the cell
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::type( unsigned int type )
{
  Guard guard ( this );
  this->_type = type;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a cell to this tree node
//
///////////////////////////////////////////////////////////////////////////////

bool OctTreeNode::add ( OctTreeNode::Point p )
{
  Guard guard ( this );

  //check my bb to determine if the point fits inside me
  if( true == this->_contains( p ) )
  {
    // Get the output stream.
    std::ofstream &outfile ( this->_getOutputStream() );

    // Write the point.
    outfile.write ( reinterpret_cast<char *> ( &p ), sizeof ( OctTreeNode::Point ) ); 

    // Increment the count.
    ++_numPoints;

    return true;
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
  Guard guard ( this );
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

osg::BoundingBox OctTreeNode::boundingBox() const
{ 
  Guard guard ( this );
  return _bb; 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the children of this node
//
///////////////////////////////////////////////////////////////////////////////

OctTreeNode::Children OctTreeNode::children() const
{ 
  Guard guard ( this );
  return _children; 
}

  
///////////////////////////////////////////////////////////////////////////////
//
//  Get the type of this node
//
///////////////////////////////////////////////////////////////////////////////

unsigned int OctTreeNode::type() const
{ 
  Guard guard ( this );
  return _type; 
}

  
///////////////////////////////////////////////////////////////////////////////
//
//  Get the capacity level of this node
//
///////////////////////////////////////////////////////////////////////////////

unsigned int OctTreeNode::capacity() const
{ 
  Guard guard ( this );
  return _capacity; 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the scene
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* OctTreeNode::buildScene( Unknown *caller, Unknown *progress )
{
  Guard guard ( this );

  //typedef osg::ref_ptr< osg::Geometry > GeometryPtr;
  typedef osg::ref_ptr< osg::Geode > GeodePtr;
  osg::ref_ptr< osg::LOD > lod ( new osg::LOD );
  unsigned int numLODs ( this->getTreeDepth() );
  unsigned int lodLevel ( ( numLODs - 1 ) - this->getNodeDepth() );

  
  // Add the geode with the points to the LOD
  {
    // Add this node's points to the LOD
    GeodePtr geode ( new osg::Geode );

    osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );
    geometry->setVertexArray( new osg::Vec3Array );
    geode->addDrawable( geometry.get() );

    // LOD name and level for this geode
    std::string lodName ( Usul::Strings::format( _workingDir, _name, "_points" ) );

    // Add the cull callback so vertices can be added and deleted at run time
    geode->setCullCallback( new CustomLODCallback( *_jobManager, lodName, geode.get(), lodLevel, _bb ) );

    // Dynamically create lod level distance definitions
    float minLevel ( static_cast< float > ( lodLevel ) / static_cast< float > ( numLODs ) );
    float maxLevel ( ( static_cast< float > ( lodLevel ) + 1.0f ) / static_cast< float > ( numLODs ) );
    float minDistance = Usul::Math::maximum( ( minLevel * _distance ), this->getBoundingRadius() * minLevel );
    float maxDistance = Usul::Math::maximum( ( maxLevel * _distance ), this->getBoundingRadius() * maxLevel );
    if( lodLevel == numLODs - 1 )
    {
      maxDistance = Usul::Math::maximum( float( maxLevel * _distance ), std::numeric_limits< float >::max() );
    }

    lod->addChild( geode.get(), minDistance, maxDistance );
  }

  // Add the children to the LOD
  {
  // Create a group to hold the children
    GroupPtr group ( new osg::Group );

    // Loop through the children and add them to the group
    for( unsigned int i = 0; i < _children.size(); ++i )
    {
      if( true == _children.at( i ).valid() )
      {
        // Add a child to the group
        group->addChild( _children.at( i )->buildScene() );

        //this->addChild( _children.at( i )->buildScene() );
      }
    } 
    
    // Dynamically create lod level distance definitions
    float minLevel ( 0.0f );
    float maxLevel ( static_cast< float > ( lodLevel ) / static_cast< float > ( numLODs ) );
   // float maxLevel ( static_cast< float > ( lodLevel ) / static_cast< float > ( numLods ) );
    float minDistance = Usul::Math::maximum( ( minLevel * _distance ), this->getBoundingRadius() * minLevel );
    float maxDistance = Usul::Math::maximum( ( maxLevel * _distance ), this->getBoundingRadius() * maxLevel );

    // Add the group to the LOD
    lod->addChild( group.get(), minDistance, maxDistance );
  }
  this->addChild( lod.get() );

  return this;//group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::preBuildScene( Usul::Documents::Document* document, Unknown *caller, Unknown *progress )
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Check to see if the point is contained within this bounding box
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::distance( double d )
{
  Guard guard ( this );
  _distance = d;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the diagnol distance of the bounding box of this node
//
///////////////////////////////////////////////////////////////////////////////

double OctTreeNode::getBoundingRadius() const
{
  Guard guard ( this );
  Usul::Math::Vec3f min ( _bb.xMin(), _bb.yMin(), _bb.zMin() );
  Usul::Math::Vec3f max ( _bb.xMax(), _bb.yMax(), _bb.zMax() );
  double d = max.distanceSquared( min );
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
  Guard guard ( this );

  // Make the lod file for this node
  std::string lodName ( Usul::Strings::format( _workingDir, _name, "LOD", _currentLodLevel ) );

  // If the file doesn't exist we are done
  if( false == Usul::Predicates::FileExists::test( lodName ) )
  {
    return;
  }

  // Get the size of the lod file
  Usul::Types::Uint64 size ( Usul::File::size( lodName )  );

  float np ( static_cast< float > ( size ) / static_cast< float > ( sizeof( OctTreeNode::Point ) ) );
  // Get the number of points in the lod file
  Usul::Types::Uint64 numPoints ( static_cast< Usul::Types::Uint64 > ( np )  );

  // Make sure there are some points in the file or we are done
  if( POINT_HOLDER == this->type() && numPoints > 0 )
  {
    // Read the file
    std::ifstream infile ( lodName.c_str(),  std::ofstream::in | std::ofstream::binary );

    // If we have an empty point vector
    if( false == _points.valid() )
    {
      _points = new osg::Vec3Array;
    }
    
    // Clear and Initialize the points holder
    _points->clear();
    _points->resize ( numPoints, OctTreeNode::Point ( 0.0, 0.0, 0.0 ) );
 
    // Sanity check.    
    USUL_ASSERT ( size == Usul::File::size ( lodName ) );

    // Read the points
    infile.read( reinterpret_cast<char *> ( &((*_points)[0]) ), size );

    // Close the streams
    this->_closeInputStream();
    this->_closeOutputStream();

  }
  else
  {
    for( unsigned int i = 0; i < _children.size(); ++i )
    {
      _children.at( i )->buildVectors();
    }
  }
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
  Guard guard ( this );
  if( _numPoints > _capacity )
  {
   
    // Update progress
    ++_numerator;
    document->setProgressBar( true, _numerator, _denominator, progress );

    // About to create my children.  I need to increment the total tree depth
    // this->setTreeDepth( this->getTreeDepth() + 1 );
    ++_depthCount;
    if( _depthCount > this->getTreeDepth() )
      this->setTreeDepth( _depthCount );

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

    // Back at the parent so decrement the depth count
    --_depthCount;
    
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
  Guard guard ( this );
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
  Guard guard ( this );  
  return _numPoints;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the number of leaf nodes in this octree
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint64 OctTreeNode::numLeafNodes() const
{
  Guard guard ( this );  
  return _numLeafNodes;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the LOD level definitions in this octree
//
///////////////////////////////////////////////////////////////////////////////

OctTreeNode::LodDefinitions OctTreeNode::getLodDefinitions() const
{
  Guard guard ( this );  
  return _lodDefinitions;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write this node and/or children to a binary restart file
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::write( std::ofstream* ofs, unsigned int numerator, unsigned int denominator, Usul::Documents::Document* document, Unknown *caller, Unknown *progress ) const
{
  Guard guard ( this );

  Usul::Types::Uint64 size ( this->_headerSize() );
  this->_writeRecord( ofs, PointSetRecords::Record::OCTREE_NODE, size );

  // write the node header information
  this->_writeNodeInfo( ofs );

  if( true == _children.empty() )
  {
    // Size of the node's name
    Usul::Types::Uint64 nameSize( _name.size() );

    // write the record information
    Usul::Types::Uint64 recordSize ( ( sizeof( Usul::Types::Uint64 ) * 2 ) + nameSize + sizeof( Usul::Types::Uint32 ) );
    this->_writeRecord( ofs, PointSetRecords::Record::OOC_VERTICES, recordSize );

    // Write the node name
    this->_writeOOCNodeInfo( ofs );

    // Update progress
    ++numerator;
    document->setProgressBar( true, numerator, denominator, progress );
  }
  else 
  {
    // Size of the node's name
    Usul::Types::Uint64 nameSize( _name.size() );

    // write the record information    
    Usul::Types::Uint64 recordSize ( ( sizeof( Usul::Types::Uint64 ) * 2 ) + nameSize + sizeof( Usul::Types::Uint32 ) );
    this->_writeRecord( ofs, PointSetRecords::Record::CHILDREN, recordSize );

    // Write the node information
    this->_writeOOCNodeInfo( ofs );

    // tell children to write their information
    for( unsigned int i = 0; i < _children.size(); ++i )
    {
      _children.at( i )->write( ofs, numerator, denominator, document, caller, progress );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read from the binary restart file
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::read( std::ifstream* ifs, Usul::Documents::Document* document, Unknown *caller, Unknown *progress )
{
  Guard guard ( this );

  // Read the records until we find an appropriate match
  const Usul::Types::Uint64 recordSize ( this->_readToRecord( ifs, PointSetRecords::Record::OCTREE_NODE ) );

  // Get the current position.
  const Usul::Types::Uint64 currentPosition ( ifs->tellg() );

  // write the node header information
  this->_readNodeInfo( ifs );

  while ( ( EOF != ifs->peek() ) && ( ifs->tellg() < ( currentPosition + recordSize ) ) )
  {
    unsigned int pos ( ifs->tellg() );

    // read the record type
    Usul::Types::Uint32 recordType ( 0 );
    ifs->read( reinterpret_cast< char* > ( &recordType ), sizeof( Usul::Types::Uint32 ) );

    // read the record size
    Usul::Types::Uint64 rs ( 0 );
    ifs->read( reinterpret_cast< char* > ( &rs ), sizeof( Usul::Types::Uint64 ) );

    switch ( recordType )
    {
      case PointSetRecords::Record::POINTS:
      {
        // write the node points
        this->_readPoints( ifs );

        // Update progress
        ++_numerator;
        document->setProgressBar( true, _numerator, _denominator, progress );
        break;
      }
      case PointSetRecords::Record::OOC_VERTICES:
      {
        // Read the name from the restart file
        this->name( _readOOCNodeInfo( ifs ) );

        ++_numerator;
        document->setProgressBar( true, _numerator, _denominator, progress );
        break;
      }
      case PointSetRecords::Record::COLORS:
      {
        // Skip it.
        // Print feedback.
        break;
      }
      case PointSetRecords::Record::CHILDREN:
      {
        // Read the name from the restart file
        this->name( _readOOCNodeInfo( ifs ) );

        // tell children to read
        _children.resize( 8 );
        for( unsigned int i = 0; i < _children.size(); ++i )
        {
          _children.at( i ) = new OctTreeNode( _jobManager, _streamBuffer, _tempPath );
          _children.at( i )->workingDir( this->workingDir() );
          _children.at( i )->distance( _distance );
          _children.at( i )->setTreeDepth( this->getTreeDepth() );
          //_children.at( i )->setNodeDepth( this->getNodeDepth() + 1 );
          _children.at( i )->read( ifs, document, caller, progress );

        }
        break;
      }
      default:
      {
        // Skip.
        break;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of lead nodes
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::numLeafNodes( Usul::Types::Uint64 num )
{
  Guard guard ( this );
  _numLeafNodes = num;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lod definitions
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::lodDefinitions( LodDefinitions lods )
{
  Guard guard ( this );
  _lodDefinitions = lods;
}


/////////////////
//  PROTECTED
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Return the header size
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint32 OctTreeNode::_headerSize() const
{
  Guard guard ( this );
  Usul::Types::Uint32 size ( ( sizeof( osg::Vec3f ) * 2 ) + ( sizeof( Usul::Types::Uint32 ) * 2 ) + sizeof( Usul::Types::Uint64 ) );
  return size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the record
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::_writeRecord( std::ofstream* ofs, Usul::Types::Uint32 type, Usul::Types::Uint64 size ) const
{
  Guard guard ( this );
  
  // write the record type
  Usul::Types::Uint32 recordType ( type );
  ofs->write( reinterpret_cast< char* > ( &recordType ), sizeof( Usul::Types::Uint32 ) );

  // write the record size
  Usul::Types::Uint64 recordSize ( size );
  ofs->write( reinterpret_cast< char* > ( &recordSize ), sizeof( Usul::Types::Uint64 ) );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the record
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint64 OctTreeNode::_readToRecord( std::ifstream* ifs, Usul::Types::Uint32 type )
{
  Guard guard ( this );

  // read the record type
  Usul::Types::Uint32 recordType ( 0 );
  ifs->read( reinterpret_cast< char* > ( &recordType ), sizeof( Usul::Types::Uint32 ) );

  // read the record size
  Usul::Types::Uint64 recordSize ( 0 );
  ifs->read( reinterpret_cast< char* > ( &recordSize ), sizeof( Usul::Types::Uint64 ) );

  // check to see if we have an octree node
  if( type != recordType )
  {
    if( EOF == ifs->peek() )
      return recordSize = 0;

    ifs->seekg( recordSize, std::ifstream::cur );

    // check for end of file
    this->_readToRecord( ifs, type );
  }  

  return recordSize;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Read the header information
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::_readNodeInfo( std::ifstream* ifs )
{
  Guard guard ( this );
  
  // read the node bounding box parameters
  osg::Vec3f minBB ( 0.0, 0.0, 0.0 );
  osg::Vec3f maxBB ( 0.0, 0.0, 0.0 );
  ifs->read( reinterpret_cast< char* > ( &minBB ), sizeof( osg::Vec3f ) );
  ifs->read( reinterpret_cast< char* > ( &maxBB ), sizeof( osg::Vec3f ) );
  osg::BoundingBox bb ( minBB.x(), minBB.y(), minBB.z(), maxBB.x(), maxBB.y(), maxBB.z() );
  this->boundingBox( bb );

  // read the node capacity
  Usul::Types::Uint32 capacity( 0 );
  ifs->read( reinterpret_cast< char* > ( &capacity ), sizeof( Usul::Types::Uint32 ) );
  this->capacity( capacity );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the points
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::_readPoints( std::ifstream* ifs )
{
  Guard guard ( this );
  Usul::Types::Uint64 numPoints( 0 );

  // read the number of points
  ifs->read( reinterpret_cast< char * > ( &numPoints ), sizeof( Usul::Types::Uint64 ) );
 
  // read the points
  if( numPoints > 0 )
  {
    if( false == _points.valid() )
    {
      _points = new osg::Vec3Array( numPoints );
    }
    else
    {
      _points->reserve( numPoints );
    }

    Usul::Types::Uint64 pointsSize ( sizeof( osg::Vec3f ) * numPoints );
    ifs->read( reinterpret_cast< char * > ( &((*_points)[0]) ), pointsSize );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the node name
//
///////////////////////////////////////////////////////////////////////////////

std::string OctTreeNode::_readOOCNodeInfo( std::ifstream* ifs )
{
  Guard guard ( this );
  
  // Read the number of points
  ifs->read( reinterpret_cast< char * > ( &_numPoints ), sizeof( Usul::Types::Uint64 ) );

  // read the number of characters in the name
  Usul::Types::Uint64 size( 0 );
  ifs->read( reinterpret_cast< char * > ( &size ), sizeof( Usul::Types::Uint64 ) );

  // read the name
  std::vector<char> name ( size );
  ifs->read( &name[0], name.size() );

  // Read the node depth
  Usul::Types::Uint32 depth( 0 );
  ifs->read( reinterpret_cast< char* > ( &depth), sizeof( Usul::Types::Uint32 ) );
  
  // Set the node's depth
  this->setNodeDepth( depth );

  return std::string ( name.begin(), name.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the node name
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::_writeOOCNodeInfo( std::ofstream* ofs ) const
{
  Guard guard ( this );

  // write the number of points
  Usul::Types::Uint64 numPoints ( _numPoints );
  ofs->write( reinterpret_cast< char * > ( &numPoints ), sizeof( Usul::Types::Uint64 ) );

  // Size of the node's name
  Usul::Types::Uint64 nameSize( _name.size() );

  // write the number of characters in the name
  ofs->write( reinterpret_cast< char * > ( &nameSize ), sizeof( Usul::Types::Uint64 ) );

  // write the name
  ofs->write( _name.c_str(), nameSize );

  //write the node depth
  Usul::Types::Uint32 depth ( _nodeDepth );
  ofs->write( reinterpret_cast< char* > ( &depth ), sizeof( Usul::Types::Uint32 ) );
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write this node's header information
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::_writeNodeInfo( std::ofstream* ofs ) const
{
  Guard guard ( this );

  // write the node bounding box parameters
  osg::Vec3f minBB ( _bb.xMin(), _bb.yMin(), _bb.zMin() );
  osg::Vec3f maxBB ( _bb.xMax(), _bb.yMax(), _bb.zMax() );
  ofs->write( reinterpret_cast< char* > ( &minBB ), sizeof( osg::Vec3f ) );
  ofs->write( reinterpret_cast< char* > ( &maxBB ), sizeof( osg::Vec3f ) );

  // write the node capacity
  Usul::Types::Uint32 capacity( _capacity );
  ofs->write( reinterpret_cast< char* > ( &capacity ), sizeof( Usul::Types::Uint32 ) );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Write this node and/or children to a binary restart file
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::_writePoints( std::ofstream* ofs, Usul::Documents::Document* document, Unknown *caller, Unknown *progress ) const
{
  Guard guard ( this );

  Usul::Types::Uint64 numPoints ( 0 );

  if( true == _points.valid() )
  {
    Points points( _points );

    numPoints = _points->size();
  
    // write the number of points
    ofs->write( reinterpret_cast< char * > ( &numPoints ), sizeof( Usul::Types::Uint64 ) );

    // write the points
    Usul::Types::Uint64 pointsSize ( numPoints * sizeof( osg::Vec3f ) );
    ofs->write( reinterpret_cast< char * > ( &((*_points)[0]) ), pointsSize );
  }
  else
  {
    // write the number of points
    ofs->write( reinterpret_cast< char * > ( &numPoints ), sizeof( Usul::Types::Uint64 ) );
  }

}

///////////////////////////////////////////////////////////////////////////////
//
//  Check to see if the point is contained within this bounding box
//
///////////////////////////////////////////////////////////////////////////////

bool OctTreeNode::_contains( OctTreeNode::Point p )
{
  Guard guard ( this );
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
  Guard guard ( this );
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
  Guard guard ( this );

  // Make sure the streams are both closed.
  this->_closeInputStream();
  this->_closeOutputStream();

  // Sanity check.
  USUL_ASSERT ( ( _numPoints * sizeof ( OctTreeNode::Point ) ) == Usul::File::size ( _tempFilename ) );

  // Get input stream.
  std::ifstream &infile ( this->_getInputStream() );

  // Point used in the loop.
  OctTreeNode::Point p ( 0.0, 0.0, 0.0 );

  // Point counter to determine when to write a point to this node's LOD file
  unsigned int count ( 0 );

  // Number of points to "skip"
  unsigned int target( _numPoints / _capacity );

  // If there are less points than the capacity then we write every point
  if( _numPoints < _capacity )
    target = 1;

  // Open an output stream for the LOD file
  std::string lodName ( Usul::Strings::format( _workingDir, "/", _name, "_points" ) );
  std::ofstream ofs ( lodName.c_str(), std::ofstream::out | std::ofstream::binary );

  // Loop through the points.
  for( unsigned int i = 0; i < _numPoints; ++i )
  {
    // Read the point from the file
    infile.read ( reinterpret_cast<char *> ( &p ), sizeof ( OctTreeNode::Point ) );

    // Add the point to the children
    this->_addCellToChild( p );

    // Determine if we need to dump a point to our LOD file
    if( count == target && target != 0 )
    {
      // reset the count
      count = 1;

      // Write a point to the file
      ofs.write( reinterpret_cast<char *> ( &p ), sizeof( OctTreeNode::Point ) );
    }
    else
    {
      // Increment the count
      ++count;
    }

  }

  // Close the LOD file
  ofs.close();

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
  Guard guard ( this );
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Split my bounding box into 8 to distribute to my children.  
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::_split()
{
  Guard guard ( this );


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

  // Decrement the leaf node count
  --_numLeafNodes;

  unsigned int nodeDepth ( this->getNodeDepth() );

  for( unsigned int i = 0; i < 8; i ++ )
  {
    _children.at( i ) = new OctTreeNode ( _jobManager, _streamBuffer, _tempPath );
    _children.at( i )->distance( _distance );
    _children.at( i )->boundingBox( b.at( i ) );
    _children.at( i )->type( POINT_HOLDER );
    _children.at( i )->capacity( _capacity );
    _children.at( i )->workingDir( _workingDir );

    // make the new directory
    Usul::File::make( Usul::Strings::format( _workingDir, "/", _name, "C", i, "/" ) );

    // Name the tree
    _children.at( i )->name( Usul::Strings::format( _name, "C", i, "/" ) );

    // Set the node depth
    _children.at( i )->setNodeDepth( nodeDepth + 1 );

    // Add the newly created child to the scene so it gets traversed in the callback
    this->addChild( _children.at( i ).get() );

    // Increment the leaf node count
    ++_numLeafNodes;

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
  Guard guard (this );
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
  Guard guard (this );

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
  Guard guard ( this );

  for( unsigned int i = 0; i < _children.size(); ++i )
  {
    _children.at( i )->closeInputStream();
    _children.at( i )->closeOutputStream();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the mutex.
//
///////////////////////////////////////////////////////////////////////////////

OctTreeNode::Mutex &OctTreeNode::mutex() const
{
  return _mutex;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Build this node
//
///////////////////////////////////////////////////////////////////////////////

std::string OctTreeNode::name()
{
  Guard guard ( this );
  return _name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build this node
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::name( const std::string& n )
{
  Guard guard ( this );
  _name = n;
  //_tempFilename = Usul::Strings::format ( _tempPath, "/", _name, "vertices.tmp" );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Create the LOD level binary files
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::createLodLevels()
{
  Guard guard ( this );

  if( false == _children.empty() )
  {
    for( unsigned int i = 0; i < _children.size(); ++i )
    {
      _children.at( i )->createLodLevels();
    }
  }
  else
  { 
    if( true == Usul::Predicates::FileExists::test( _tempFilename ) )
      this->_createLodLevels();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the working directory
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::workingDir( const std::string& dir )
{
  Guard guard ( this );

  if( _children.size() > 0 )
  {
    for( unsigned int i = 0; i < _children.size(); ++i )
    {
      _children.at( i )->workingDir( dir );
    }
  }
  _workingDir = dir;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the working directory
//
///////////////////////////////////////////////////////////////////////////////

std::string OctTreeNode::workingDir()
{
  Guard guard ( this );

  return _workingDir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the base name
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::baseName( const std::string& name )
{
  Guard guard ( this );

  if( _children.size() > 0 )
  {
    for( unsigned int i = 0; i < _children.size(); ++i )
    {
      _children.at( i )->baseName( name );
    }
  }
  _baseName = name;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the base name
//
///////////////////////////////////////////////////////////////////////////////

std::string OctTreeNode::baseName()
{
  Guard guard ( this );

  return _baseName;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Create the LOD level binary files
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::_createLodLevels()
{
  Guard guard ( this );

  // Define the number of LODs
  unsigned int numLODs ( _lodDefinitions.size() );

  //for( unsigned int lodLevel = 0; lodLevel < numLODs; ++lodLevel )
  {
    // Define the amount of points to skip at this lod level
    // unsigned int lodMultiplier ( _lodDefinitions.at( lodLevel ) );
    unsigned int lodMultiplier ( 1 );

    // Open the stream to the master list of points
    std::ifstream ifs ( _tempFilename.c_str(), std::ofstream::in | std::ofstream::binary  );

    // Make the lod file for this node
    //std::string lodName ( Usul::Strings::format( _workingDir, "/", _name, "LOD", lodLevel ) );
    std::string lodName ( Usul::Strings::format( _workingDir, "/", _name, "_points" ) );
    std::ofstream ofs ( lodName.c_str(), std::ofstream::out | std::ofstream::binary );

    // Current file position
    unsigned long long currentFilePos ( 0 );

    // Read and write the points for this lod level
    for( unsigned int i = 0; i < _numPoints; i += lodMultiplier )
    {
      // Go to the current point
      currentFilePos = i * sizeof( OctTreeNode::Point );

      // Temp point to hold the input value
      OctTreeNode::Point p ( 0.0f, 0.0f, 0.0f );

      // Make sure we are within a valid range
      //if( currentFilePos <= infile.eof() )
      {
        // Seek to the next point to load
        ifs.seekg( currentFilePos ); 
        
        // Read the point
        ifs.read( reinterpret_cast<char *> ( &p ), sizeof( OctTreeNode::Point ) );

        // Write the point to the temp file
        ofs.write( reinterpret_cast<char *> ( &p ), sizeof( OctTreeNode::Point ) );
        
      }

      
    }
    // close the output stream
    ofs.close();

    // close the input stream
    ifs.close();

  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current LOD level
//
///////////////////////////////////////////////////////////////////////////////

unsigned int OctTreeNode::_getLodLevel( const osg::Vec3d& eye )
{
  Guard guard ( this );

  unsigned int lodLevel ( 0 );

  unsigned int numLods = _lodDefinitions.size();

  for( unsigned int i = 0; i < numLods; ++i )
  {
    float minLevel ( static_cast< float > ( i ) / static_cast< float > ( numLods ) );
    float maxLevel ( ( static_cast< float > ( i ) + 1.0f ) / static_cast< float > ( numLods ) );
    float minDistance = Usul::Math::maximum( ( minLevel * _distance ), this->getBoundingRadius() * minLevel );
    float maxDistance = Usul::Math::maximum( ( maxLevel * _distance ), this->getBoundingRadius() * maxLevel );

    if( lodLevel == numLods - 1 )
    {
      maxDistance = Usul::Math::maximum( ( maxLevel * _distance ), std::numeric_limits< double >::max() );
    }

    osg::Vec3 d ( eye - _bb.center() );
    float currentDistance ( abs ( d.length() ) );
    if( currentDistance >= minDistance && currentDistance <= maxDistance )
    {
      lodLevel = i;
      break;
    }
  }
  return lodLevel;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build this node
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* OctTreeNode::_buildNode( Unknown *caller, Unknown *progress )
{
  Guard guard ( this );
  typedef osg::ref_ptr< osg::Geometry > GeometryPtr;
  typedef osg::ref_ptr< osg::Geode > GeodePtr;
  osg::ref_ptr< osg::LOD > lod ( new osg::LOD );
  osg::ref_ptr< osg::Group > group ( new osg::Group );
  unsigned int numLODs ( _lodDefinitions.size() );

  // Get the current lod level
  unsigned int lodLevel = _currentLodLevel;

  // Make sure the points are valid
  if( true == _points.valid() )
  {
    // LOds
    if( _points->size() > 0 )
    {
      
      ////Points vertices ( new osg::Vec3Array );
      //ElementsPtr indices ( new osg::DrawElementsUShort ( osg::PrimitiveSet::POINTS ) );

      //// set
      //unsigned int lodMultiplier ( _lodDefinitions.at( lodLevel ) );

      //indices->reserve( static_cast< unsigned int > ( ( _numPoints / lodMultiplier ) ) );
      //for( unsigned int i = 0; i < _numPoints; i += lodMultiplier )
      //{
      //  indices->push_back( i );
      //}

      GeometryPtr geometry ( new osg::Geometry ); 

      geometry->setVertexArray( _points.get() );
      geometry->addPrimitiveSet( new osg::DrawArrays ( osg::PrimitiveSet::POINTS, 0, _points->size() ) );
    
      GeodePtr geode ( new osg::Geode );
      geode->addDrawable( geometry.get() );
      
      
      group->addChild( geode.get() );
     

    }
  }

  return group.release();
}

///////////////////////////////////////////////////////////////////////////////
//
// Get/Set Node Depth
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint32 OctTreeNode::getNodeDepth()
{
  Guard guard ( this );
  return _nodeDepth;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get/Set Node Depth
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::setNodeDepth( Usul::Types::Uint32 d )
{
  Guard guard ( this );
  _nodeDepth = d;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get/Set Tree Depth
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint32 OctTreeNode::getTreeDepth()
{
  Guard guard ( this );
  return _treeDepth;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get/Set Tree Depth
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::setTreeDepth( Usul::Types::Uint32 d )
{
  Guard guard ( this );
  _treeDepth = d;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close the streams of all children
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::traverse ( osg::NodeVisitor &nv )
{
  #if 0
  // If it's a cull visitor...
  if ( osg::NodeVisitor::CULL_VISITOR == nv.getVisitorType() )
  {
    // Get cull visitor.
    osgUtil::CullVisitor *cv ( dynamic_cast < osgUtil::CullVisitor * > ( &nv ) );
    
    // Check for a valid cull visitor object.
    if ( 0x0 == cv )
      return;

    // If we are a leaf node
    if( this->type() == POINT_HOLDER ) 
    {
      // If we aren't culled...
      if( true == cv->isCulled ( _bb ) )
      {
        Guard guard ( this );

        // Get the lod node
        osg::ref_ptr< osg::LOD > lod ( dynamic_cast< osg::LOD* > ( this->getChild( 0 ) ) );

        // Make sure we got it
        if( 0x0 == lod )
        {
          BaseClass::traverse ( nv );
          return;
        }

        for( unsigned int i = 0; i < lod->getNumChildren(); ++i )
        {
          osg::ref_ptr< osg::Geode > child ( dynamic_cast< osg::Geode* > ( lod->getChild( i ) ) );
          
          // make sure the child is a geode
          if( 0x0 == child )
          {
            BaseClass::traverse ( nv );
            return;
          }

          // get the geometry
          osg::ref_ptr< osg::Geometry > geometry ( dynamic_cast< osg::Geometry* > ( child->getDrawable( 0 ) ) );
          
          // make sure the geometry is valid
          if( 0x0 == geometry )
          {
            BaseClass::traverse ( nv );
            return;
          }
          
          // clear out the vertices
          geometry->setVertexArray( new osg::Vec3Array );

          // dirty the display lists to be sure the scene is rebuilt
          geometry->dirtyDisplayList();

        }
      }
    }
  }
  #endif
  BaseClass::traverse ( nv );
  
}

