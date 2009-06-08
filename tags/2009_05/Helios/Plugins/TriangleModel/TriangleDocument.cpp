
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for triangle models.
//
///////////////////////////////////////////////////////////////////////////////

#include "TriangleDocument.h"
#include "TriangleComponent.h"
#include "TriangleReaderSTL.h"
#include "TriangleWriterSTL.h"
#include "TriangleReaderTDF.h"
#include "TriangleWriterTDF.h"
#include "TriangleReaderR3D.h"
#include "TriangleReaderGrassRaster.h"
#include "TriangleReaderArcAsciiGrid.h"
#include "ParadisReader.h"
#include "TriangleReaderOFF.h"
#include "TriangleReaderFieldViewAscii.h"

#include "OsgTools/Triangles/FindAllConnected.h"
#include "OsgTools/Triangles/LoopSplitter.h"
#include "OsgTools/Triangles/FindLoops.h"

#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Interfaces/GUI/ICancelButton.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Interfaces/ISmoothTriangles.h"
#include "Usul/Interfaces/IDecimateTriangles.h"
#include "Usul/Interfaces/ISubdivideTriangles.h"

#include "Usul/Adaptors/Random.h"
#include "Usul/Components/Manager.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Errors/Assert.h"
#include "Usul/File/Path.h"
#include "Usul/Policies/Update.h"
#include "Usul/Resources/ProgressBar.h"
#include "Usul/Resources/StatusBar.h"
#include "Usul/Resources/CancelButton.h"
#include "Usul/Strings/Case.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Types/Types.h"

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"

#include "osgUtil/IntersectVisitor"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( TriangleDocument, TriangleDocument::BaseClass );
USUL_IMPLEMENT_TYPE_ID ( TriangleDocument );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

TriangleDocument::TriangleDocument() : BaseClass ( "Triangle Document" ),
  _triangles ( new OsgTools::Triangles::TriangleSet ),
  _uncapped (),
  _capped ()
{
#if 1
  _triangles->factory()->usePool ( false );
#endif

  // Default options.
  this->setOption ( "normals", "per-vertex" );
  this->setOption ( "colors", "per-vertex" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

TriangleDocument::~TriangleDocument()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *TriangleDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IFlipNormals::IID:
    return static_cast < Usul::Interfaces::IFlipNormals * > ( this );
  case Usul::Interfaces::IFlipNormal::IID:
    return static_cast < Usul::Interfaces::IFlipNormal * > ( this );
  case Usul::Interfaces::IAddTriangleWithSharedVertex::IID:
    return static_cast < Usul::Interfaces::IAddTriangleWithSharedVertex* > ( this );
  case Usul::Interfaces::IAddTrangleWithOsgVec3f::IID:
    return static_cast < Usul::Interfaces::IAddTrangleWithOsgVec3f* > ( this );
  case Usul::Interfaces::IFindLoops::IID:
    return static_cast < Usul::Interfaces::IFindLoops* > ( this );
  case Usul::Interfaces::IGetLoops::IID:
    return static_cast < Usul::Interfaces::IGetLoops* > ( this );
  case Usul::Interfaces::IGetVertex::IID:
    return static_cast < Usul::Interfaces::IGetVertex* > ( this );
  case Usul::Interfaces::IDeletePrimitive::IID:
    return static_cast < Usul::Interfaces::IDeletePrimitive* > ( this );
  case Usul::Interfaces::IKeepAllConnected::IID:
    return static_cast < Usul::Interfaces::IKeepAllConnected* > ( this );
  case Usul::Interfaces::IDeleteAllConnected::IID:
    return static_cast < Usul::Interfaces::IDeleteAllConnected* > ( this );
  case Usul::Interfaces::IGroupPrimitives::IID:
      return static_cast < Usul::Interfaces::IGroupPrimitives* > ( this );
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IGetVertexNormal::IID:
    return static_cast < Usul::Interfaces::IGetVertexNormal* > ( this );
  case Usul::Interfaces::IGetTriangleNormal::IID:
    return static_cast < Usul::Interfaces::IGetTriangleNormal* > ( this );
  case Usul::Interfaces::IAddSharedVertex::IID:
    return static_cast < Usul::Interfaces::IAddSharedVertex* > ( this );
  case Usul::Interfaces::IGetBoundingBox::IID:
    return static_cast < Usul::Interfaces::IGetBoundingBox* > ( this );
  case Usul::Interfaces::ITriangle::IID:
    return static_cast < Usul::Interfaces::ITriangle* > ( this );
  case Usul::Interfaces::ITriangleSV::IID:
    return static_cast < Usul::Interfaces::ITriangleSV* > ( this );
  case Usul::Interfaces::IVertices::IID:
    return static_cast < Usul::Interfaces::IVertices* > ( this );
  case Usul::Interfaces::IColorsPerVertex::IID:
    return static_cast < Usul::Interfaces::IColorsPerVertex* > ( this );
  case Usul::Interfaces::IMemoryPool::IID:
    return static_cast < Usul::Interfaces::IMemoryPool* > ( this );
  case Usul::Interfaces::IMaterials::IID:
    return static_cast < Usul::Interfaces::IMaterials* > ( this );
  case Usul::Interfaces::IDisplaylists::IID:
    return static_cast < Usul::Interfaces::IDisplaylists* > ( this );
    case Usul::Interfaces::ILoadColorFile::IID:
    return static_cast < Usul::Interfaces::ILoadColorFile* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool TriangleDocument::canExport ( const std::string &file ) const
{
  return this->canSave ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool TriangleDocument::canInsert ( const std::string &file ) const
{
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool TriangleDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "stl" || ext == "r3d" || ext == "tdf" || ext == "asc" || ext == "grs" || ext == "off" || ext == "fvas"/*|| ext == "prds"*/ );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool TriangleDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "stl" || ext == "tdf" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing data.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( name ) ) );

  if ( "stl" == ext )
  {
    TriangleReaderSTL reader ( name, progress, this );
    reader();
  }
  else if ( "r3d" == ext )
  {
    TriangleReaderR3D reader ( name, progress, this );
    reader();
  }
  else if ( "tdf" == ext )
  {
    TriangleReaderTDF reader ( name, progress, this );
    reader();
  } 
  else if ( "asc" == ext )
  {
    TriangleReaderArcAsciiGrid reader ( name, progress, this );
    reader();
  }
  else if ( "grs" == ext )
  {
    TriangleReaderGrassRaster reader ( name, progress, this );
    reader();
  }
#if 0
  else if ( "prds" == ext )
  {
    ParadisReader reader ( name, caller, this );
    reader();
  }
#endif
  else if ( "off" == ext )
  {
    TriangleReaderOFF reader ( name, progress, this );
    reader ();
  }
  else if ( "fvas" == ext )
  {
    TriangleReaderFieldViewAscii reader ( name, progress, this );
    reader ();
  }
  else
  {
    throw std::runtime_error("Error: 983249834290 Invalid file extension for Triangle Document:" + name);
  }

  this->purge();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to the given file name.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::write ( const std::string &name, Unknown *caller, Unknown *progress ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( name ) ) );
   
  if ( "stl" == ext )
  {
    TriangleWriterSTL writer ( name, this->binary(), caller, this );
    writer();
  }
  else if ( "tdf" == ext )
  {
    TriangleWriterTDF writer ( name, caller, this );
    writer();
  }
  else
  {
    throw std::runtime_error("Error: 983249834291 Invalid file extension for Triangle Document write:" + name);
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::clear ( Usul::Interfaces::IUnknown *caller )
{
  // Make sure loops are cleared before triangle set is cleared.
  _uncapped.clear();
  _capped.clear();

  // Clear the triangle set.
  _triangles->clear ( caller );
  _triangles->purge();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

TriangleDocument::Filters TriangleDocument::filtersInsert() const
{
  return this->filtersOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

TriangleDocument::Filters TriangleDocument::filtersOpen() const
{
  Filters filters;
  filters.push_back ( Filter ( "All Triangle Files (*.tdf *.stl *.r3d *.asc)", "*.tdf;*.stl;*.r3d;*.asc" ) );
  filters.push_back ( Filter ( "Triangle Document Format (*.tdf)", "*.tdf" ) );
  filters.push_back ( Filter ( "Stereolithography (*.stl)", "*.stl" ) );
  filters.push_back ( Filter ( "RoboMet 3D (*.r3d)", "*.r3d" ) );
  filters.push_back ( Filter ( "Arc ASCII Grid (*.asc)", "*.asc" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

TriangleDocument::Filters TriangleDocument::filtersSave() const
{
  Filters filters;
  filters.push_back ( Filter ( "Triangle Document Format (*.tdf)", "*.tdf" ) );
  filters.push_back ( Filter ( "Stereolithography ASCII (*.stl)",  "*.stl" ) );
  filters.push_back ( Filter ( "Stereolithography Binary (*.stl)", "*.stl" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

TriangleDocument::Filters TriangleDocument::filtersExport() const
{
  Filters filters;
  filters.push_back ( Filter ( "Triangle Document Format (*.tdf)", "*.tdf" ) );
  filters.push_back ( Filter ( "Stereolithography ASCII (*.stl)",  "*.stl" ) );
  filters.push_back ( Filter ( "Stereolithography Binary (*.stl)", "*.stl" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *TriangleDocument::buildScene ( const BaseClass::Options &opt, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  // Redirect to triangle set
  return _triangles->buildScene ( opt, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Flip the normals in this document.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::flipNormalVectors()
{
  _triangles->flipNormals();
  this->modified ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Flip the normal of the i'th triangle in this document.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::flipNormalVector( unsigned int i )
{
  _triangles->flipNormal( i );
  this->modified ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of triangles.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int TriangleDocument::numTriangles() const
{
  return _triangles->numTriangles();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reserve the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::reserveTriangles ( unsigned int num )
{
  _triangles->reserve ( num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the triangles.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Triangles::Triangle *TriangleDocument::addTriangle ( const osg::Vec3f &v0, const osg::Vec3f &v1, const osg::Vec3f &v2, const osg::Vec3f &n, bool update, bool look )
{
  return _triangles->addTriangle ( v0, v1, v2, n, update, look );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the triangles.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Triangles::Triangle *TriangleDocument::addTriangle ( SharedVertex *v0, SharedVertex *v1, SharedVertex *v2, const osg::Vec3f &n, bool update, bool )
{
  return _triangles->addTriangle ( v0, v1, v2, n, update );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the loops
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::findLoops ( Usul::Interfaces::IUnknown* caller )
{
  // For readability.
  typedef OsgTools::Triangles::Triangle Triangle;
  typedef OsgTools::Triangles::SharedVertex SharedVertex;
  typedef Usul::Polygons::TriangleTest AdjacencyTest;

  // Get needed interfaces
  Usul::Interfaces::IRedraw::QueryPtr redraw ( Usul::Documents::Manager::instance().activeView() );

  // Turn off stats updating on the status bar.
  Usul::Interfaces::IRedraw::ResetStatsDisplay resetStats ( redraw.get(), false, true  );

  // Show the cancel button
  Usul::Interfaces::ICancelButton::ShowHide cancel ( caller );

  // Update functor for finding loops
  Usul::Interfaces::IFindLoops::UpdateFindingLoops updateLoops ( 0.0, 0.90, caller );

  // Update for finding inner loops
  Usul::Interfaces::IProgressBar::UpdateProgressBar updateProgress ( 0.90, 1.0, caller );

  // Need to make sure all triangles and shared vertices are unvisited
  _triangles->setAllUnvisited();

  // Reset the on edge flags.
  _triangles->resetOnEdge();

  // Need to get triangles
  TriangleSet::TriangleVector &triangles ( _triangles->triangles() );

  // The adjacency test
  AdjacencyTest adjacent;

  // Clear our uncapped loops.
  _uncapped.clear();

  // Find the loops that need to be triangulated.
  OsgTools::Triangles::capPolygons ( triangles, _uncapped, adjacent, 3, updateLoops );

  // Interface to flush event queue
  Usul::Interfaces::IFlushEvents::ValidQueryPtr flush ( caller );

  // Functor for updating the status bar
  Usul::Interfaces::IStatusBar::UpdateStatusBar status ( caller );

  status( "Detecting inner loops..." );

  Usul::Interfaces::IUnknown::ValidQueryPtr me ( this );
  
  OsgTools::Triangles::LoopSplitter ls ( _triangles->getBoundingBox(), me );
  Loops copy ( _uncapped );
  _uncapped.clear();
 
  for( Loops::iterator i = copy.begin(); i != copy.end(); ++ i )
  {
    try
    {
      OsgTools::Triangles::LoopSplitter::Loops splitLoops = ls.split(*i);
      if (splitLoops.size() == 0) 
      {
        _uncapped.push_back(*i);
      }
      else 
      {
        
       // std::copy( splitLoops.begin(), splitLoops.end(), _uncapped.end() );
        for ( OsgTools::Triangles::LoopSplitter::Loops::iterator iter = splitLoops.begin(); iter != splitLoops.end(); ++iter )
          _uncapped.push_back( *iter );
        //std::cout << "Loops were Split at index :" << i-copy.begin() << std::endl;
      }
    }
    catch ( const std::exception & e )
    {
      std::cout << "Exception thrown splitting loop at index: " << i-copy.begin() << std::endl;
      std::cout << e.what() << std::endl;
      i->valid( false );
      _uncapped.push_back( *i );
    }
	  catch ( ... )
	  {
		  std::cout << "Unknown Exception thrown splitting loop at index: " << i-copy.begin() << std::endl;
      i->valid( false );
      _uncapped.push_back( *i );
	  }
  }
  

  // Find any inner loops.
  for( Loops::iterator i = _uncapped.begin(); i != _uncapped.end(); ++ i )
  {
    for ( Loops::iterator j = _uncapped.begin(); j != _uncapped.end(); ++j )
    {
      // Make the app responsive.
      flush->flushEventQueue();

      osg::Vec3 v ( j->vertex( 0, me.get() ) );

      // Test to see if first point of j is in i.
      if( i->pointInside( v, me.get() ) )
      {
        // Add j to i.
        i->addInnerLoop ( *j );
        
        // Make a copy of j to erease.
        Loops::iterator doomed ( j );

        // Move to a valid iterator.
        --j;
        
        // Erase j.
        _uncapped.erase( doomed );

      }
    }
    updateProgress ( i - _uncapped.begin(), _uncapped.size() );
  }
    
  //Reserve enough room
  _capped.reserve ( _uncapped.size() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the uncapped loops
//
///////////////////////////////////////////////////////////////////////////////

const TriangleDocument::Loops& TriangleDocument::getUncappedLoops() const
{
  return _uncapped;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the uncapped loops
//
///////////////////////////////////////////////////////////////////////////////

TriangleDocument::Loops& TriangleDocument::getUncappedLoops()
{
  return _uncapped;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the capped loops
//
///////////////////////////////////////////////////////////////////////////////

const TriangleDocument::Loops& TriangleDocument::getCappedLoops() const
{
  return _capped;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the capped loops
//
///////////////////////////////////////////////////////////////////////////////

TriangleDocument::Loops& TriangleDocument::getCappedLoops()
{
  return _capped;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vertex at the index
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f& TriangleDocument::getVertex ( unsigned int index ) const
{
  return _triangles->getVertex ( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete triangle at given index
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::deletePrimitive ( const osgUtil::LineSegmentIntersector::Intersection& hit )
{
  const osg::Geode* geode ( dynamic_cast<const osg::Geode*> ( *hit.nodePath.rbegin() ) );
  _triangles->removeTriangle ( geode, hit.drawable.get(), hit.primitiveIndex );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find all triangles connected to the seed value
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::_findAllConnected ( Usul::Interfaces::IUnknown* caller, Connected& connected, Uint32 seed, bool showProgress, bool clearVisitedFlag )
{
  _triangles->findAllConnected ( caller, connected, seed, showProgress, clearVisitedFlag );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Keep all triangles connected to the seed value
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::keepAllConnected ( Usul::Interfaces::IUnknown *caller, const osgUtil::LineSegmentIntersector::Intersection &hit )
{
  Connected connected;
  this->_findAllConnected ( caller, connected, _triangles->index ( hit ), true, true );
  _triangles->keepTriangles ( connected, caller );
  //this->sendMessage ( BaseClass::ID_BUILD_SCENE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete all triangles connected to the seed value
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::deleteAllConnected ( Usul::Interfaces::IUnknown *caller, const osgUtil::LineSegmentIntersector::Intersection &hit )
{
  Connected connected;
  this->_findAllConnected ( caller, connected,  _triangles->index ( hit ), true, true );
  _triangles->removeTriangles ( connected, caller );
  //this->sendMessage ( BaseClass::ID_BUILD_SCENE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Group the Primitive Drawables by their connectivity
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::groupPrimitives( Usul::Interfaces::IUnknown *caller )
{
  _triangles->groupTriangles ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the default gui
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::createDefaultGUI ( Unknown *caller )
{
  if ( this->delegate() )
  {
    this->setOption ( "normals", "per-vertex" );
    this->setOption ( "colors", "per-vertex" );
    this->delegate()->createDefaultGUI ( this, caller );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get normal for the i'th vertex.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f& TriangleDocument::getVertexNormal ( unsigned int index ) const
{
  return _triangles->vertexNormal ( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get normal for the i'th triangle.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f& TriangleDocument::getTriangleNormal ( unsigned int index ) const
{
  return _triangles->triangleNormal ( index );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get The BoundingBox for this document
//
///////////////////////////////////////////////////////////////////////////////

osg::BoundingBox TriangleDocument::getBoundingBox() const 
{
  return _triangles->getBoundingBox();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a shared vertex.
//
///////////////////////////////////////////////////////////////////////////////

TriangleDocument::SharedVertex* TriangleDocument::addSharedVertex ( const osg::Vec3f &v, bool look )
{
  return _triangles->addSharedVertex ( v, look );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::updateBounds ( const osg::Vec3f &v ) 
{
  _triangles->updateBounds ( v );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the triangle set. If the current one is empty, it just replaces it.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::addTriangleSet ( OsgTools::Triangles::TriangleSet *t ) 
{
  if ( _triangles->empty() )
  {
    _triangles = t;
    return;
  }

  USUL_ASSERT ( 0 ); // TODO, insert...
}


///////////////////////////////////////////////////////////////////////////////
//
//  Purge memory.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::purge()
{
  _triangles->purge();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of groups.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int TriangleDocument::groupsNumber () const
{
  return _triangles->blocksNumber();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the group as an IUnknown.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* TriangleDocument::getPrimitiveGroup ( unsigned int i )
{
  return _triangles->blocks  ( i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Smooth the model.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::smoothModel ( )
{
  Usul::Interfaces::IUnknown *unknown ( Usul::Components::Manager::instance().getInterface ( Usul::Interfaces::ISmoothTriangles::IID ) );
  Usul::Interfaces::ISmoothTriangles::QueryPtr plugin ( unknown );

  if ( plugin.valid() )
  {
    plugin->smoothTriangles( _triangles.get(), 50 );
    this->modified( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decimate the model.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::decimateModel ( )
{
  Usul::Interfaces::IUnknown *unknown ( Usul::Components::Manager::instance().getInterface ( Usul::Interfaces::IDecimateTriangles::IID ) );
  Usul::Interfaces::IDecimateTriangles::QueryPtr plugin ( unknown );

  if ( plugin.valid() )
  {
    plugin->decimateTriangles( _triangles.get(), 0.50 );
    this->modified( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subdivide the model.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::subdivideModel ( unsigned int numSubdivisions )
{
  Usul::Interfaces::IUnknown *unknown ( Usul::Components::Manager::instance().getInterface ( Usul::Interfaces::ISubdivideTriangles::IID ) );
  Usul::Interfaces::ISubdivideTriangles::QueryPtr plugin ( unknown );

  if ( plugin.valid() )
  {
    plugin->subdivideTriangles( _triangles.get(), numSubdivisions );
    this->modified( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a node showing the new primtives.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* TriangleDocument::getNewPrimitives ( )
{
  return _triangles->showNewTriangles();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create and allocate the set of colors per vertex
//  added by Jeff Conner (2007)
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4Array * TriangleDocument::getColorsV ( bool reserve )
{
  return _triangles->getColorsV ( reserve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::dirtyColorsV ( bool flag )
{
  _triangles->dirtyColorsV ( flag );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Use memory pool
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::usePool ( bool use )
{
  _triangles->factory()->usePool ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Use memory pool
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::useMaterial ( bool use )
{
  _triangles->useMaterial ( use );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Get display list state
//
///////////////////////////////////////////////////////////////////////////////

bool TriangleDocument::displayList () const
{
  return _triangles->displayList();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Use display lists
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDocument::displayList ( bool use )
{
  _triangles->displayList( use );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Load a color file into a triangle set
//
/////////////////////////////////////////////////////////////////////////////

void TriangleDocument::loadColorFile( const std::string &filename, const HeaderInfo& header )
{
  _triangles->loadColorFile( filename, header );
}
