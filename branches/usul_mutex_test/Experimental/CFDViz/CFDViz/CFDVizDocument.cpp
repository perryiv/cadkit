
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/CFDViz/CFDViz/CFDVizDocument.h"
#include "CFDAnimation.h"

#include "Usul/Interfaces/IDisplaylists.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IViewPort.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Predicates/FileExists.h"

#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/File/Slash.h"
#include "Usul/Print/Matrix.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/UMath.h"
#include "Usul/Math/MinMax.h"

#include "OsgTools/DisplayLists.h"
#include "OsgTools/Group.h"
#include "OsgTools/Convert.h"
#include "OsgTools/Builders/Arrow.h"
#include "OsgTools/State/StateSet.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/RadioButton.h"

#include "osgDB/ReadFile"
#include "osgDB/WriteFile"
#include "osg/Group"
#include "osg/Switch"
#include "osg/MatrixTransform"
#include "osgDB/WriteFile"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/StateSet"
#include "osg/Texture2D"
#include "osg/BoundingBox"
#include "osg/Plane"

#include <iterator>
#include <vector>
#include <fstream>
#include <iostream>


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( CFDVizDocument, CFDVizDocument::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

CFDVizDocument::CFDVizDocument() : 
  BaseClass ( "CFD Visualization Document" ),
  _root( new osg::Group ),
  _particleScene( new osg::Group ),
  _vertices( 0 ),
  _progress( 0.0, 0.0 ),
  _colorTable( 0 ),
  _firstPass( true ),
  _particles( 0 ),
  _update( 5 ),
  _isAnimating( false ),
  _streamLines( new osg::Vec3Array )
{
  USUL_TRACE_SCOPE;
   
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

CFDVizDocument::~CFDVizDocument()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void CFDVizDocument::clear ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *CFDVizDocument::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener* > ( this );
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd* > ( this );
  case Usul::Interfaces::ICFDCommands::IID:
    return static_cast < Usul::Interfaces::ICFDCommands* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool CFDVizDocument::canExport ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool CFDVizDocument::canInsert ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool CFDVizDocument::canOpen ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "cfd" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool CFDVizDocument::canSave ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "cfd" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void CFDVizDocument::write ( const std::string &name, Unknown *caller  ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  osgDB::writeNodeFile( *_root.get(), name+".ive" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void CFDVizDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  this->_readColorTable( name, progress );

#if 1
  this->_readVertices( name );
#else
  this->_readGridFile( name, progress );
  this->_initParticles();
  this->_partition();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

CFDVizDocument::Filters CFDVizDocument::filtersSave() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "CFD Visualization (*.cfd)", "*.cfd" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

CFDVizDocument::Filters CFDVizDocument::filtersExport() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

CFDVizDocument::Filters CFDVizDocument::filtersOpen() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "CFD Visualization (*.cfd)", "*.cfd" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

CFDVizDocument::Filters CFDVizDocument::filtersInsert() const
{
  USUL_TRACE_SCOPE;
  return this->filtersOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *CFDVizDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _root->removeChild( 0, _root->getNumChildren() );
  
#if 1
  _root->addChild( this->_buildVectorArrows( true, 2.0, caller ) );
#else
  _root->addChild( this->_drawGrid( caller ) );
  _root->addChild( this->_buildVectorArrows( false, 1.0, caller ) );
  _root->addChild( this->_drawParticles( caller ) );
#endif
  std::cout << "Buildscene complete!" << std::endl;
  return _root.get();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void CFDVizDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  if( true == _update() && true == _isAnimating )
  {
    this->_moveParticles( caller );
    this->_drawParticles( caller );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void CFDVizDocument::_openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if ( 0x0 == document )
    return;
#if 1
  if( true == document->canOpen( file ) )
    document->open ( file, caller );
  else
    std::cout << "Unable to open document for file: " << file << std::endl;
#else
    document->open ( file, caller );
#endif
  std::cout << "Done" << std::endl;
  
	
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CFDVizDocument::_readGridFile( const std::string & filename, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  std::ifstream in;
  in.open( filename.c_str() );

  in >> _gridResolution[0] >> _gridResolution[1] >> _gridResolution[2];

  unsigned int numVectors = _gridResolution[0] * _gridResolution[1] * _gridResolution[2]; 
  // Set the progress numbers.
  _progress.first = 0;
  _progress.second = numVectors;

  Usul::Policies::TimeBased update ( 1000 ); // Every second.


  _vertices.reserve( numVectors );
  for( unsigned int i = 0; i < numVectors; ++i )
  {
    double x = 0, y = 0, z = 0, u = 0, v = 0, w = 0;
    in >> x >> y >> z >> u >> v >> w;
    CFDPoint p;
    p.pos = Vec3d( x, y, z );
    p.dir = Vec3d( u, v, w );
    _vertices.push_back( p );

    // Feedback.
    this->_incrementProgress ( update(), progress );
  }

  in.close();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CFDVizDocument::_moveParticles( Unknown *caller )
{
  USUL_TRACE_SCOPE;

  //feedback
  //std::cout << "Moving Particles... " << std::endl;

  for( unsigned int i = 0; i < _particles.size(); ++i )
  {
    double magnitude = 0.1;
    Vec3d pos = _particles.at( i ).pos;
    Vec3d dir = this->_getDirectionAt2( pos, caller );
    pos = pos + ( dir * magnitude );

    {
      Guard guard ( this->mutex() );
      _particles.at( i ).pos = pos;
      _particles.at( i ).dir = dir * magnitude;
    }
    
      
  }
  
}

///////////////////////////////////////////////////////////////////////////////
//
//  Test to see if a given point lies on the positive side of the plane
//
///////////////////////////////////////////////////////////////////////////////

bool CFDVizDocument::_rayQuadIntersect( Vec3d a, Vec3d b, Vec3d c, Vec3d d, Vec3d p )
{
  //t= (Xn dot (PointOnPLANE - Raystart)) / (Xn dot Raydirection) 
    
#if 0
  // triangle abc
  {
    osg::Vec3d n = ( osg::Vec3d( c[0], c[1], c[2] ) - osg::Vec3d( b[0], b[1], b[2] ) ) ^
                   ( osg::Vec3d( a[0], a[1], a[2] ) - osg::Vec3d( b[0], b[1], b[2] ) );
    Vec3d N ( n.x(), n.y(), n.z() );
    N.normalize();
    Vec3d rayDir = a - p;
    rayDir.normalize();

    double t = ( N.dot( a - p ) );
    t /= ( N.dot( rayDir ) );
    if( t < 0 )
      return false;
  }

  //triangle acd
  {
    osg::Vec3d n = ( osg::Vec3d( d[0], d[1], d[2] ) - osg::Vec3d( c[0], c[1], c[2] ) ) ^
                   ( osg::Vec3d( a[0], a[1], a[2] ) - osg::Vec3d( c[0], c[1], c[2] ) );
    Vec3d N ( n.x(), n.y(), n.z() );
    N.normalize();
    Vec3d rayDir = a - p;
    rayDir.normalize();

    double t = ( N.dot( a - p ) ) / ( N.dot( rayDir ) );
    if( t < 0 )
      return false;
  }
#else
  {
    osg::Plane plane ( osg::Vec3d( a[0], a[1], a[2] ),
                       osg::Vec3d( b[0], b[1], b[2] ),
                       osg::Vec3d( c[0], c[1], c[2] ) );

    std::vector< osg::Vec3d > vertices;
    vertices.push_back( osg::Vec3d( p[0], p[1], p[2] ) );
    
    int result = plane.intersect( vertices );
    if( result >= 0 )
      return true;
  }
  {
    osg::Plane plane ( osg::Vec3d( a[0], a[1], a[2] ),
                       osg::Vec3d( c[0], c[1], c[2] ),
                       osg::Vec3d( d[0], d[1], d[2] ) );
    
    std::vector< osg::Vec3d > vertices;
    vertices.push_back( osg::Vec3d( p[0], p[1], p[2] ) );
   
    int result = plane.intersect( vertices );
    if( result < 0 )
      return false;
  }
#endif

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check to see if a given point lies within the hexahedron at 
//  position i, j, k
//
///////////////////////////////////////////////////////////////////////////////

bool CFDVizDocument::_contains( unsigned int i, unsigned int j, unsigned int k, Vec3d pos )
{
  // Get the points
  std::vector< CFDPoint > v;   
  Usul::Math::Vec3ui min( i, j, k ); 
  Usul::Math::Vec3ui max ( Usul::Math::Vec3ui( min[0] + 1, min[1] + 1, min[2] + 1 ) );
  v.resize( 8 );
  // v0 ( 0 0 0 )
  v.at( 0 ) = this->_getVertexAt( min[0], min[1], min[2] );
  // v1 ( 1 0 0 )
  v.at( 1 ) = this->_getVertexAt( max[0], min[1], min[2] );
  // v2 ( 0 1 0 )
  v.at( 2 ) = this->_getVertexAt( min[0], max[1], min[2] );
  // v3 ( 1 1 0 )
  v.at( 3 ) = this->_getVertexAt( max[0], max[1], min[2] );
  // v4 ( 0 0 1 )
  v.at( 4 ) = this->_getVertexAt( min[0], min[1], max[2] );
  // v5 ( 1 0 1 )
  v.at( 5 ) = this->_getVertexAt( max[0], min[1], max[2] );
  // v6 ( 0 1 1 )
  v.at( 6 ) = this->_getVertexAt( min[0], max[1], max[2] );
  // v7 ( 1 1 1 )
  v.at( 7 ) = this->_getVertexAt( max[0], max[1], max[2] );

  //matrix to project a point/matrix into the xy plane
  Usul::Math::Matrix44d xyproj ( 1.0, 0.0, 0.0, 0.0,
                                  0.0, 1.0, 0.0, 0.0,
                                  0.0, 0.0, 0.0, 0.0,
                                  0.0, 0.0, 0.0, 1.0 );

  // Check Face 0 1 3 2
  {
    Vec3d a = v.at( 0 ).pos;
    Vec3d b = v.at( 1 ).pos;
    Vec3d c = v.at( 3 ).pos;
    Vec3d d = v.at( 2 ).pos;
    
#if 1

    if( false == this->_rayQuadIntersect( a, b, c, d, pos ) )
      return false;
#else
    Vec3d ap = a - a;
    Vec3d bp = b - a;
    Vec3d cp = c - a;
    Vec3d dp = d - a;
    Vec3d pp = pos - a;



    double h = this->_determinant( bp, dp, cp );

    Usul::Math::Matrix44d m1 ( 1.0, 0.0, 1.0, 0.0,
                               0.0, 1.0, 1.0, 0.0,
                               0.0, 0.0, h  , 0.0,
                               0.0, 0.0, 0.0, 1.0 ); 
    Usul::Math::Matrix44d m2 ( bp[0], dp[0], cp[0], 0.0,
                               bp[1], dp[1], cp[1], 0.0,
                               bp[2], dp[2], cp[2], 0.0,
                               0.0  , 0.0  , 0.0  , 1.0 );
    Usul::Math::Matrix44d m2i;
    m2.inverse( m2i );
    m2.invert();
    Usul::Math::Matrix44d mfinal = m1 * m2;
    Usul::Math::Matrix44d psemi  ( pp[0], 0.0  , 0.0  , 0.0, 
                                   0.0  , pp[1], 0.0  , 0.0, 
                                   0.0  , 0.0  , pp[2], 0.0, 
                                   0.0  , 0.0  , 0.0  , 1.0 );
    Usul::Math::Matrix44d psemiInv;
    psemi.inverse( psemiInv );
    Usul::Math::Matrix44d pfinal = m1 * psemiInv;

    osg::Matrixd brot;
    brot.makeRotate( osg::Vec3d( bp[0], bp[1], bp[2] ), osg::Vec3d( mfinal[0], mfinal[2], mfinal[2] ) );
    osg::Matrixd drot;
    drot.makeRotate( osg::Vec3d( dp[0], dp[1], dp[2] ), osg::Vec3d( mfinal[4], mfinal[5], mfinal[6] ) );
    osg::Matrixd crot;
    crot.makeRotate( osg::Vec3d( cp[0], cp[1], cp[2] ), osg::Vec3d( mfinal[8], mfinal[9], mfinal[10] ) );

    osg::Matrixd prot = brot * drot * crot;
    osg::Vec3d osgppp = prot * osg::Vec3d( pp[0], pp[1], pp[2] );

    Vec3d ptest = m1 * pp;
    Vec3d ppp = Vec3d( osgppp.x(), osgppp.y(), osgppp.z() );

    //check to see if the translated point is above the translated surface
    //return false if it is not
    if( ppp[2] < ( h * ppp[0] * ppp[1] ) )
      return false;
    #endif
  }
  // Check Face 4 5 1 0
  {
    Vec3d a = v.at( 4 ).pos;
    Vec3d b = v.at( 5 ).pos;
    Vec3d c = v.at( 1 ).pos;
    Vec3d d = v.at( 0 ).pos;
#if 1
    if( false == this->_rayQuadIntersect( a, b, c, d, pos ) )
      return false;
#else
    Vec3d ap = a - a;
    Vec3d bp = b - a;
    Vec3d cp = c - a;
    Vec3d dp = d - a;
    Vec3d pp = pos - a;

    double h = this->_determinant( bp, dp, cp );

    Usul::Math::Matrix44d m1 ( 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, h, 0.0, 0.0, 0.0, 0.0, 1.0 ); 
    Usul::Math::Matrix44d m2 ( bp[0], dp[0], cp[0], 0.0, bp[1], dp[1], cp[1], 0.0, bp[2], dp[2], cp[2], 0.0, 0.0, 0.0, 0.0, 1.0 );
    Usul::Math::Matrix44d m2i;
    m2.inverse( m2i );
    Usul::Math::Matrix44d mfinal = m1 * m2i;
    Usul::Math::Matrix44d psemi  ( pp[0], 0.0, 0.0, 0.0, 
                                   pp[1], 0.0, 0.0, 0.0, 
                                   pp[2], 0.0, 0.0, 0.0, 
                                   0.0  , 0.0, 0.0, 1.0 );
    Usul::Math::Matrix44d psemiInv;
    psemi.inverse( psemiInv );
    Usul::Math::Matrix44d pfinal = m1 * psemiInv;

    osg::Matrixd brot;
    brot.makeRotate( osg::Vec3d( bp[0], bp[1], bp[2] ), osg::Vec3d( mfinal[0], mfinal[2], mfinal[2] ) );
    osg::Matrixd drot;
    drot.makeRotate( osg::Vec3d( dp[0], dp[1], dp[2] ), osg::Vec3d( mfinal[4], mfinal[5], mfinal[6] ) );
    osg::Matrixd crot;
    crot.makeRotate( osg::Vec3d( cp[0], cp[1], cp[2] ), osg::Vec3d( mfinal[8], mfinal[9], mfinal[10] ) );

    osg::Matrixd prot = brot * drot * crot;
    osg::Vec3d osgppp = prot * osg::Vec3d( pp[0], pp[1], pp[2] );

    Vec3d ptest = m1 * pp;
    Vec3d ppp = Vec3d( osgppp.x(), osgppp.y(), osgppp.z() );

    //check to see if the translated point is above the translated surface
    //return false if it is not
    //if( ppp[2] < ( h * ppp[0] * ppp[1] ) )
     // return false;
#endif
  }

  // Check Face 4 0 2 6
  {
    Vec3d a = v.at( 4 ).pos;
    Vec3d b = v.at( 0 ).pos;
    Vec3d c = v.at( 2 ).pos;
    Vec3d d = v.at( 6 ).pos;
#if 1
    if( false == this->_rayQuadIntersect( a, b, c, d, pos ) )
      return false;
#else
    Vec3d ap = a - a;
    Vec3d bp = b - a;
    Vec3d cp = c - a;
    Vec3d dp = d - a;
    Vec3d pp = pos - a;

    double h = this->_determinant( bp, dp, cp );

    Usul::Math::Matrix44d m1 ( 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, h, 0.0, 0.0, 0.0, 0.0, 1.0 ); 
    Usul::Math::Matrix44d m2 ( bp[0], dp[0], cp[0], 0.0, bp[1], dp[1], cp[1], 0.0, bp[2], dp[2], cp[2], 0.0, 0.0, 0.0, 0.0, 1.0 );
    Usul::Math::Matrix44d m2i;
    m2.inverse( m2i );
    Usul::Math::Matrix44d mfinal = m1 * m2i;
    Usul::Math::Matrix44d psemi  ( pp[0], 0.0, 0.0, 0.0, 
                                   pp[1], 0.0, 0.0, 0.0, 
                                   pp[2], 0.0, 0.0, 0.0, 
                                   0.0  , 0.0, 0.0, 1.0 );
    Usul::Math::Matrix44d psemiInv;
    psemi.inverse( psemiInv );
    Usul::Math::Matrix44d pfinal = m1 * psemiInv;

    osg::Matrixd brot;
    brot.makeRotate( osg::Vec3d( bp[0], bp[1], bp[2] ), osg::Vec3d( mfinal[0], mfinal[2], mfinal[2] ) );
    osg::Matrixd drot;
    drot.makeRotate( osg::Vec3d( dp[0], dp[1], dp[2] ), osg::Vec3d( mfinal[4], mfinal[5], mfinal[6] ) );
    osg::Matrixd crot;
    crot.makeRotate( osg::Vec3d( cp[0], cp[1], cp[2] ), osg::Vec3d( mfinal[8], mfinal[9], mfinal[10] ) );

    osg::Matrixd prot = brot * drot * crot;
    osg::Vec3d osgppp = prot * osg::Vec3d( pp[0], pp[1], pp[2] );

    Vec3d ptest = m1 * pp;
    Vec3d ppp = Vec3d( osgppp.x(), osgppp.y(), osgppp.z() );

    //check to see if the translated point is above the translated surface
    //return false if it is not
    //if( ppp[2] < ( h * ppp[0] * ppp[1] ) )
    //  return false;
#endif
  }

  // Check Face 2 3 7 6
  {
    Vec3d a = v.at( 2 ).pos;
    Vec3d b = v.at( 3 ).pos;
    Vec3d c = v.at( 7 ).pos;
    Vec3d d = v.at( 6 ).pos;
#if 1
    if( false == this->_rayQuadIntersect( a, b, c, d, pos ) )
      return false;
#else
    Vec3d ap = a - a;
    Vec3d bp = b - a;
    Vec3d cp = c - a;
    Vec3d dp = d - a;
    Vec3d pp = pos - a;

    double h = this->_determinant( bp, dp, cp );

    Usul::Math::Matrix44d m1 ( 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, h, 0.0, 0.0, 0.0, 0.0, 1.0 ); 
    //Usul::Math::Matrix44d m2 ( bp[0], dp[0], cp[0], 0.0, bp[1], dp[1], cp[1], 0.0, bp[2], dp[2], cp[2], 0.0, 0.0, 0.0, 0.0, 1.0 );
    Usul::Math::Matrix44d m2 ( bp[0], dp[0], cp[0], 0.0, bp[1], dp[1], cp[1], 0.0, bp[2], dp[2], cp[2], 0.0, 0.0, 0.0, 0.0, 1.0 );
    Usul::Math::Matrix44d m2i;
    m2.inverse( m2i );
    Usul::Math::Matrix44d mfinal = m1 * m2i;
    Usul::Math::Matrix44d psemi  ( pp[0], 0.0, 0.0, 0.0, 
                                   pp[1], 0.0, 0.0, 0.0, 
                                   pp[2], 0.0, 0.0, 0.0, 
                                   0.0  , 0.0, 0.0, 1.0 );
    Usul::Math::Matrix44d psemiInv;
    psemi.inverse( psemiInv );
    Usul::Math::Matrix44d pfinal = m1 * psemiInv;


    osg::Matrixd brot;
    brot.makeRotate( osg::Vec3d( bp[0], bp[1], bp[2] ), osg::Vec3d( mfinal[0], mfinal[2], mfinal[2] ) );
    osg::Matrixd drot;
    drot.makeRotate( osg::Vec3d( dp[0], dp[1], dp[2] ), osg::Vec3d( mfinal[4], mfinal[5], mfinal[6] ) );
    osg::Matrixd crot;
    crot.makeRotate( osg::Vec3d( cp[0], cp[1], cp[2] ), osg::Vec3d( mfinal[8], mfinal[9], mfinal[10] ) );

    osg::Matrixd prot = brot * drot * crot;
    osg::Vec3d osgppp = prot * osg::Vec3d( pp[0], pp[1], pp[2] );

    Vec3d ptest = m1 * pp;
    Vec3d ppp = Vec3d( osgppp.x(), osgppp.y(), osgppp.z() );

    //check to see if the translated point is above the translated surface
    //return false if it is not
    //if( ppp[2] < ( h * ppp[0] * ppp[1] ) )
    //  return false;
#endif
  }

  // Check Face 1 5 7 3
  {
    Vec3d a = v.at( 1 ).pos;
    Vec3d b = v.at( 5 ).pos;
    Vec3d c = v.at( 7 ).pos;
    Vec3d d = v.at( 3 ).pos;
#if 1
    if( false == this->_rayQuadIntersect( a, b, c, d, pos ) )
      return false;
#else
    Vec3d ap = a - a;
    Vec3d bp = b - a;
    Vec3d cp = c - a;
    Vec3d dp = d - a;
    Vec3d pp = pos - a;

    double h = this->_determinant( bp, dp, cp );

    Usul::Math::Matrix44d m1 ( 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, h, 0.0, 0.0, 0.0, 0.0, 1.0 ); 
    Usul::Math::Matrix44d m2 ( bp[0], dp[0], cp[0], 0.0, bp[1], dp[1], cp[1], 0.0, bp[2], dp[2], cp[2], 0.0, 0.0, 0.0, 0.0, 1.0 );
    Usul::Math::Matrix44d m2i;
    m2.inverse( m2i );
    Usul::Math::Matrix44d mfinal = m1 * m2i;
    Usul::Math::Matrix44d psemi  ( pp[0], 0.0, 0.0, 0.0, 
                                   pp[1], 0.0, 0.0, 0.0, 
                                   pp[2], 0.0, 0.0, 0.0, 
                                   0.0  , 0.0, 0.0, 1.0 );
    Usul::Math::Matrix44d psemiInv;
    psemi.inverse( psemiInv );
    Usul::Math::Matrix44d pfinal = m1 * psemiInv;

    osg::Matrixd brot;
    brot.makeRotate( osg::Vec3d( bp[0], bp[1], bp[2] ), osg::Vec3d( mfinal[0], mfinal[2], mfinal[2] ) );
    osg::Matrixd drot;
    drot.makeRotate( osg::Vec3d( dp[0], dp[1], dp[2] ), osg::Vec3d( mfinal[4], mfinal[5], mfinal[6] ) );
    osg::Matrixd crot;
    crot.makeRotate( osg::Vec3d( cp[0], cp[1], cp[2] ), osg::Vec3d( mfinal[8], mfinal[9], mfinal[10] ) );

    osg::Matrixd prot = brot * drot * crot;
    osg::Vec3d osgppp = prot * osg::Vec3d( pp[0], pp[1], pp[2] );

    Vec3d ptest = m1 * pp;
    Vec3d ppp = Vec3d( osgppp.x(), osgppp.y(), osgppp.z() );

    //check to see if the translated point is above the translated surface
    //return false if it is not
    //if( ppp[2] < ( h * ppp[0] * ppp[1] ) )
    //  return false;
#endif
  }

  // Check Face 5 4 6 7
  {
    Vec3d a = v.at( 5 ).pos;
    Vec3d b = v.at( 4 ).pos;
    Vec3d c = v.at( 6 ).pos;
    Vec3d d = v.at( 7 ).pos;
#if 1
    if( false == this->_rayQuadIntersect( a, b, c, d, pos ) )
      return false;
#else
    Vec3d ap = a - a;
    Vec3d bp = b - a;
    Vec3d cp = c - a;
    Vec3d dp = d - a;
    Vec3d pp = pos - a;

    double h = this->_determinant( bp, dp, cp );

    Usul::Math::Matrix44d m1 ( 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, h, 0.0, 0.0, 0.0, 0.0, 1.0 ); 
    Usul::Math::Matrix44d m2 ( bp[0], dp[0], cp[0], 0.0, bp[1], dp[1], cp[1], 0.0, bp[2], dp[2], cp[2], 0.0, 0.0, 0.0, 0.0, 1.0 );
    Usul::Math::Matrix44d m2i;
    m2.inverse( m2i );
    Usul::Math::Matrix44d mfinal = m1 * m2i;
    Usul::Math::Matrix44d psemi  ( pp[0], 0.0, 0.0, 0.0, 
                                   pp[1], 0.0, 0.0, 0.0, 
                                   pp[2], 0.0, 0.0, 0.0, 
                                   0.0  , 0.0, 0.0, 1.0 );
    Usul::Math::Matrix44d psemiInv;
    psemi.inverse( psemiInv );
    Usul::Math::Matrix44d pfinal = m1 * psemiInv;
    
    osg::Matrixd brot;
    brot.makeRotate( osg::Vec3d( bp[0], bp[1], bp[2] ), osg::Vec3d( mfinal[0], mfinal[2], mfinal[2] ) );
    osg::Matrixd drot;
    drot.makeRotate( osg::Vec3d( dp[0], dp[1], dp[2] ), osg::Vec3d( mfinal[4], mfinal[5], mfinal[6] ) );
    osg::Matrixd crot;
    crot.makeRotate( osg::Vec3d( cp[0], cp[1], cp[2] ), osg::Vec3d( mfinal[8], mfinal[9], mfinal[10] ) );

    osg::Matrixd prot = brot * drot * crot;
    osg::Vec3d osgppp = prot * osg::Vec3d( pp[0], pp[1], pp[2] );

    Vec3d ptest = m1 * pp;
    Vec3d ppp = Vec3d( osgppp.x(), osgppp.y(), osgppp.z() );

    //check to see if the translated point is above the translated surface
    //return false if it is not
    //if( ppp[2] < ( h * ppp[0] * ppp[1] ) )
    //  return false;
#endif
  }
  // the point lies within the hexahedral cell
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

double CFDVizDocument::_determinant( Vec3d a, Vec3d b, Vec3d c )
{
  double det = ( a[0] * ( ( b[1] * c[2] ) - ( b[2] * c[1] ) ) ) - 
               ( b[0] * ( ( a[1] * c[2] ) - ( a[2] * c[1] ) ) ) +
               ( c[0] * ( ( a[1] * b[2] ) - ( a[2] * b[1] ) ) );
  return det;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create bounding boxes that will narrow search for performance speedup
//
///////////////////////////////////////////////////////////////////////////////

void CFDVizDocument::_partition()
{
  std::cout << "Partition Grid Space..." << std::endl;

  for( unsigned int i = 0; i < _gridResolution[0] - 1; ++i )
  {
    Partition p;
    CFDPoint cp = this->_getVertexAt( i, 0, 0 );
    osg::Vec3d value( cp.pos[0], cp.pos[1], cp.pos[2] );
    p.bb.set( value, value );
    int xmin = Usul::Math::maximum( static_cast< int > ( i ) - 2, 0 );
    p.xmin = static_cast< unsigned int > ( xmin );
    p.xmax = Usul::Math::minimum( i + 3, _gridResolution[0] - 1 );
    p.ymin = 0;
    p.ymax = _gridResolution[1] - 1;
    p.zmin = 0;
    p.zmax = _gridResolution[2] - 1;
    for( unsigned int j = 0; j < _gridResolution[1]; ++j )
    { 
      for( unsigned int k = 0; k < _gridResolution[2]; ++k )
      {
        //partition grid space
        CFDPoint tp = this->_getVertexAt( i, j, k );
        osg::Vec3d bbmin = p.bb.corner( 0 );
        osg::Vec3d bbmax = p.bb.corner( 7 );
        osg::Vec3d min ( Usul::Math::minimum( tp.pos[0], bbmin.x() ),
                         Usul::Math::minimum( tp.pos[1], bbmin.y() ),
                         Usul::Math::minimum( tp.pos[2], bbmin.z() ) );

        osg::Vec3d max ( Usul::Math::maximum( tp.pos[0], bbmax.x() ),
                         Usul::Math::maximum( tp.pos[1], bbmax.y() ),
                         Usul::Math::maximum( tp.pos[2], bbmax.z() ) );

        tp = this->_getVertexAt( i + 1, j, k );
        min = osg::Vec3d( Usul::Math::minimum( tp.pos[0], min.x() ),
                          Usul::Math::minimum( tp.pos[1], min.y() ),
                          Usul::Math::minimum( tp.pos[2], min.z() ) );

        max = osg::Vec3d( Usul::Math::maximum( tp.pos[0], max.x() ),
                          Usul::Math::maximum( tp.pos[1], max.y() ),
                          Usul::Math::maximum( tp.pos[2], max.z() ) );
        

        p.bb.set( min, max );

      }

    }
    _partitions.push_back( p );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check bounding boxes to narrow search for performance speedup
//
///////////////////////////////////////////////////////////////////////////////

bool CFDVizDocument::_getPartition( Vec2d &iRange, Vec2d &jRange, Vec2d &kRange, Vec3d pos )
{
  for( unsigned int i = 0; i < _partitions.size(); ++i )
  { 
    Partition p = _partitions.at( i );
    osg::Vec3f fpos ( static_cast< float > ( pos[0] ),
                      static_cast< float > ( pos[1] ),
                      static_cast< float > ( pos[2] ) );
    osg::Vec3d dpos ( pos[0], pos[1], pos[2] );
    
    if( p.bb.contains( dpos ) )
    {
      iRange = Vec2d( p.xmin, p.xmax );
      jRange = Vec2d( p.ymin, p.ymax );
      kRange = Vec2d( p.zmin, p.zmax );
      return true;

    }
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Used for irregular grid lookups
//
///////////////////////////////////////////////////////////////////////////////

CFDVizDocument::Vec3d CFDVizDocument::_getDirectionAt2( Vec3d pos, Unknown *caller )
{
  Vec3d dir( 0, 0, 0 );

  unsigned int numVectors = _gridResolution[0] * _gridResolution[1] * _gridResolution[2]; 
  _progress.first = 0;
  _progress.second = numVectors;

  Usul::Policies::TimeBased update ( 1000 ); // Every second.

  Vec2d iRange ( 0, _gridResolution[0] - 1 );
  Vec2d jRange ( 0, _gridResolution[1] - 1 );
  Vec2d kRange ( 0, _gridResolution[2] - 1 );

  Vec2d ti, tj, tk;
  //this->_getPartition( ti, tj, tk, pos );

  for( unsigned int i = iRange[0]; i < iRange[1]; ++i )
  {
    for( unsigned int j = jRange[0]; j < jRange[1]; ++j )
    {
      for( unsigned int k = kRange[0]; k < kRange[1]; ++k )
      {
        
        if( true == this->_contains( i, j, k, pos ) )
        {
          dir = this->_getDirectionAt( i, j, k, pos );
        }
        // Feedback.
        this->_incrementProgress ( update(), caller );
      }
    }
  }

  return dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CFDVizDocument::Vec3d CFDVizDocument::_getDirectionAt( unsigned int i, unsigned int j, unsigned int k, Vec3d pos )
{

  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Vec3d p( 0.0, 0.0, 0.0 );

  p = pos;
  
  Vec3d c( 0, 0, 0 );
 
  std::vector< CFDPoint > v;   
  Usul::Math::Vec3ui min( i, j, k );
 
  Usul::Math::Vec3ui max ( Usul::Math::Vec3ui( min[0] + 1, min[1] + 1, min[2] + 1 ) );
  
  v.resize( 8 );
  // v0 ( 0 0 0 )
  v.at( 0 ) = this->_getVertexAt( min[0], min[1], min[2] );
  // v1 ( 1 0 0 )
  v.at( 1 ) = this->_getVertexAt( max[0], min[1], min[2] );
  // v2 ( 0 1 0 )
  v.at( 2 ) = this->_getVertexAt( min[0], max[1], min[2] );
  // v3 ( 1 1 0 )
  v.at( 3 ) = this->_getVertexAt( max[0], max[1], min[2] );
  // v4 ( 0 0 1 )
  v.at( 4 ) = this->_getVertexAt( min[0], min[1], max[2] );
  // v5 ( 1 0 1 )
  v.at( 5 ) = this->_getVertexAt( max[0], min[1], max[2] );
  // v6 ( 0 1 1 )
  v.at( 6 ) = this->_getVertexAt( min[0], max[1], max[2] );
  // v7 ( 1 1 1 )
  v.at( 7 ) = this->_getVertexAt( max[0], max[1], max[2] );
  
  // X-axis interpolation
  double b = p[0] / max[0];
  double a = 1 - b;

  //between 0 and 1
  Vec3d V01 ( 0, 0, 0 );
  V01[0] = ( v.at( 0 ).dir[0] * a ) + ( v.at( 1 ).dir[0] * b );
  V01[1] = ( v.at( 0 ).dir[1] * a ) + ( v.at( 1 ).dir[1] * b );
  V01[2] = ( v.at( 0 ).dir[2] * a ) + ( v.at( 1 ).dir[2] * b );
  
  //between 2 and 3
  Vec3d V23 ( 0, 0, 0 );
  V23[0] = ( v.at( 2 ).dir[0] * a ) + ( v.at( 3 ).dir[0] * b );
  V23[1] = ( v.at( 2 ).dir[1] * a ) + ( v.at( 3 ).dir[1] * b );
  V23[2] = ( v.at( 2 ).dir[2] * a ) + ( v.at( 3 ).dir[2] * b );
  
  //between 4 and 5
  Vec3d V45 ( 0, 0, 0 );
  V45[0] = ( v.at( 4 ).dir[0] * a ) + ( v.at( 5 ).dir[0] * b );
  V45[1] = ( v.at( 4 ).dir[1] * a ) + ( v.at( 5 ).dir[1] * b );
  V45[2] = ( v.at( 4 ).dir[2] * a ) + ( v.at( 5 ).dir[2] * b );
  
  //between 6 and 7
  Vec3d V67 ( 0, 0, 0 );
  V67[0] = ( v.at( 6 ).dir[0] * a ) + ( v.at( 7 ).dir[0] * b );
  V67[1] = ( v.at( 6 ).dir[1] * a ) + ( v.at( 7 ).dir[1] * b );
  V67[2] = ( v.at( 6 ).dir[2] * a ) + ( v.at( 7 ).dir[2] * b );
  
  // Y-axis interpolation
  b = p[1] / max[1];
  a = 1 - b;
  //between 01 and 23
  //  P' = a*P0 + b*P1
  
  Vec3d V0123 ( 0, 0, 0 );
  V0123[0] = ( V01[0] * a ) + ( V23[0] * b );
  V0123[1] = ( V01[1] * a ) + ( V23[1] * b );
  V0123[2] = ( V01[2] * a ) + ( V23[2] * b );
  
  //between 45 and 67
  Vec3d V4567 ( 0, 0, 0 );
  V4567[0] = ( V45[0] * a ) + ( V67[0] * b );
  V4567[1] = ( V45[1] * a ) + ( V67[1] * b );
  V4567[2] = ( V45[2] * a ) + ( V67[2] * b );
  
  // Z-axis interpolation
  b = p[2] / max[2];
  a = 1 - b;
  //Final value 
  
  c[0] = ( V0123[0] * a ) + ( V4567[0] * b );
  c[1] = ( V0123[1] * a ) + ( V4567[1] * b );
  c[2] = ( V0123[2] * a ) + ( V4567[2] * b );
  
  return c;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CFDVizDocument::Vec3d CFDVizDocument::_getDirectionAt( Vec3d pos )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Vec3d p( 0.0, 0.0, 0.0 );

  p = pos;
  
  Vec3d c( 0, 0, 0 );
  //FILE* debug = fopen( "V:/data/VolViz/colorat.txt", "a" );
  //FILE* debug2 = fopen( "V:/data/VolViz/voxelat.txt", "a" );
  std::vector< CFDPoint > v;   

  if( p[0] >= 0 && p[0] < _gridResolution[0] - 1 && p[1] >= 0 && p[1] < _gridResolution[1] - 1 && p[2] >= 0 && p[2] < _gridResolution[2] - 1 )
  {
    Usul::Math::Vec3ui min( static_cast< unsigned int > ( floor( p[0] ) ),
                            static_cast< unsigned int > ( floor( p[1] ) ), 
                            static_cast< unsigned int > ( floor( p[2] ) ) );
   
    Usul::Math::Vec3ui max ( Usul::Math::Vec3ui( min[0] + 1, min[1] + 1, min[2] + 1 ) );
    
    // get the colors at the voxel vertices
    v.resize( 8 );
    // v0 ( 0 0 0 )
    v.at( 0 ) = this->_getVertexAt( min[0], min[1], min[2] );
    // v1 ( 1 0 0 )
    v.at( 1 ) = this->_getVertexAt( max[0], min[1], min[2] );
    // v2 ( 0 1 0 )
    v.at( 2 ) = this->_getVertexAt( min[0], max[1], min[2] );
    // v3 ( 1 1 0 )
    v.at( 3 ) = this->_getVertexAt( max[0], max[1], min[2] );
    // v4 ( 0 0 1 )
    v.at( 4 ) = this->_getVertexAt( min[0], min[1], max[2] );
    // v5 ( 1 0 1 )
    v.at( 5 ) = this->_getVertexAt( max[0], min[1], max[2] );
    // v6 ( 0 1 1 )
    v.at( 6 ) = this->_getVertexAt( min[0], max[1], max[2] );
    // v7 ( 1 1 1 )
    v.at( 7 ) = this->_getVertexAt( max[0], max[1], max[2] );
    
    // X-axis interpolation
    double b = p[0] / max[0];
    double a = 1 - b;

    //between 0 and 1
    Vec3d V01 ( 0, 0, 0 );
    V01[0] = ( v.at( 0 ).dir[0] * a ) + ( v.at( 1 ).dir[0] * b );
    V01[1] = ( v.at( 0 ).dir[1] * a ) + ( v.at( 1 ).dir[1] * b );
    V01[2] = ( v.at( 0 ).dir[2] * a ) + ( v.at( 1 ).dir[2] * b );
    
    //between 2 and 3
    Vec3d V23 ( 0, 0, 0 );
    V23[0] = ( v.at( 2 ).dir[0] * a ) + ( v.at( 3 ).dir[0] * b );
    V23[1] = ( v.at( 2 ).dir[1] * a ) + ( v.at( 3 ).dir[1] * b );
    V23[2] = ( v.at( 2 ).dir[2] * a ) + ( v.at( 3 ).dir[2] * b );
    
    //between 4 and 5
    Vec3d V45 ( 0, 0, 0 );
    V45[0] = ( v.at( 4 ).dir[0] * a ) + ( v.at( 5 ).dir[0] * b );
    V45[1] = ( v.at( 4 ).dir[1] * a ) + ( v.at( 5 ).dir[1] * b );
    V45[2] = ( v.at( 4 ).dir[2] * a ) + ( v.at( 5 ).dir[2] * b );
    
    //between 6 and 7
    Vec3d V67 ( 0, 0, 0 );
    V67[0] = ( v.at( 6 ).dir[0] * a ) + ( v.at( 7 ).dir[0] * b );
    V67[1] = ( v.at( 6 ).dir[1] * a ) + ( v.at( 7 ).dir[1] * b );
    V67[2] = ( v.at( 6 ).dir[2] * a ) + ( v.at( 7 ).dir[2] * b );
    
    // Y-axis interpolation
    b = p[1] / max[1];
    a = 1 - b;
    //between 01 and 23
    //  P' = a*P0 + b*P1
    
    Vec3d V0123 ( 0, 0, 0 );
    V0123[0] = ( V01[0] * a ) + ( V23[0] * b );
    V0123[1] = ( V01[1] * a ) + ( V23[1] * b );
    V0123[2] = ( V01[2] * a ) + ( V23[2] * b );
    
    //between 45 and 67
    Vec3d V4567 ( 0, 0, 0 );
    V4567[0] = ( V45[0] * a ) + ( V67[0] * b );
    V4567[1] = ( V45[1] * a ) + ( V67[1] * b );
    V4567[2] = ( V45[2] * a ) + ( V67[2] * b );
    
    // Z-axis interpolation
    b = p[2] / max[2];
    a = 1 - b;
    //Final value 
    
    c[0] = ( V0123[0] * a ) + ( V4567[0] * b );
    c[1] = ( V0123[1] * a ) + ( V4567[1] * b );
    c[2] = ( V0123[2] * a ) + ( V4567[2] * b );
    //fprintf( debug, "pos=( %lf, %lf ,%lf)\tcolor=(%lf, %lf, %lf, %lf)\n", p[0], p[1], p[2], c[0], c[1], c[2], c[3] );  
  }

  //fclose( debug );
  //fclose( debug2 );
  return c;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CFDVizDocument::_initParticles()
{
  Particle p0, p1, p2;
  
    
#if 1
  p0.pos = Vec3d( 0.1, 1.5, 0.5 );
  p1.pos = Vec3d( 0.1, 1.5, 1.5 );
  p2.pos = Vec3d( 0.1, 1.5, 2.5 );

  p0.dir = Vec3d( 0.0, 0.0, 0.0 );
  p1.dir = Vec3d( 0.0, 0.0, 0.0 );
  p2.dir = Vec3d( 0.0, 0.0, 0.0 );

  p0.ttl = 10;
  p1.ttl = 10;
  p2.ttl = 10;

  _particles.push_back( p0 );
  osg::Vec3d pos ( p0.pos[0], p0.pos[1], p0.pos[2] );
  _streamLines->push_back( pos );
  _streamLines->push_back( pos );
  

  _particles.push_back( p1 );
  pos = osg::Vec3d( p1.pos[0], p1.pos[1], p1.pos[2] );
  _streamLines->push_back( pos );
  _streamLines->push_back( pos );

  _particles.push_back( p2 );
  pos = osg::Vec3d( p2.pos[0], p2.pos[1], p2.pos[2] );
  _streamLines->push_back( pos );
  _streamLines->push_back( pos );

  
#else
#if 0
  p0.pos = Vec3d( 0.5, 0.5, 0.5 );
  p0.dir = Vec3d( 0, 0, 0 );
  _particles.push_back( p0 );
  _streamLines->push_back( p0.pos );
  _streamLines->push_back( p0.pos );
#else
  double midz = floor( static_cast< double > ( _gridResolution[2] ) / 2 );
  double midy = floor( static_cast< double > ( _gridResolution[1] ) / 2 );
  CFDPoint c0 = this->_getVertexAt( _gridResolution[0], 
                static_cast< unsigned int > ( midy ),
                static_cast< unsigned int > ( midz ) );
  p0.pos = c0.pos;
  //p0.pos += .5;
  _particles.push_back( p0 );
  _streamLines->push_back( p0.pos );
  _streamLines->push_back( p0.pos );

#endif
#endif


}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * CFDVizDocument::_drawParticles( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _particleScene->removeChild( 0, _particleScene->getNumChildren() );
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );

  for( unsigned int i = 0; i < _particles.size(); ++i )
  {
    osg::Vec3d pos ( osg::Vec3d( _particles.at( i ).pos[0], _particles.at( i ).pos[1], _particles.at( i ).pos[2] ) );
    osg::Vec3d dir ( osg::Vec3d( _particles.at( i ).dir[0], _particles.at( i ).dir[1], _particles.at( i ).dir[2] ) );
    
    
    unsigned int size = _particles.size() * 2;
    _streamLines->push_back( _streamLines->at( _streamLines->size() - ( size - 1 ) ) );
    _streamLines->push_back( pos );
    
  }

  OsgTools::State::StateSet::setPointSize( geometry->getOrCreateStateSet(), 6 );
  OsgTools::State::StateSet::setLineWidth( geometry->getOrCreateStateSet(), 2 );
  OsgTools::State::StateSet::setLighting( geometry->getOrCreateStateSet(), false );
  osg::ref_ptr< osg::Vec4Array > colors ( new osg::Vec4Array() );
  colors->push_back( osg::Vec4f( 0.0, 0.0, 1.0, 1.0 ) );
  geometry->setColorArray( colors.get() );
  geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
  geometry->setVertexArray( _streamLines.get() );
  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINES, 0, _streamLines->size() ) );
  geode->addDrawable( geometry.get() );
  _particleScene->addChild( geode.get() );
  return _particleScene.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to set status bar and flush events.
//
///////////////////////////////////////////////////////////////////////////////

void CFDVizDocument::_setStatusBar ( const std::string &text, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Usul::Interfaces::IStatusBar::QueryPtr status ( caller );
  if ( status.valid() )
    status->setStatusBarText ( text, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * CFDVizDocument::_drawGrid( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  GroupPtr group ( new osg::Group );
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  
  // Set the progress numbers.
  std::cout << "Creating Grid Object..." << std::endl;
   unsigned int numVectors = _gridResolution[0] * _gridResolution[1] * _gridResolution[2]; 
  _progress.first = 0;
  _progress.second = numVectors;

  Usul::Policies::TimeBased update ( 1000 ); // Every second.
  
  
  for( unsigned int i = 0; i < _gridResolution[0] - 1; ++i )
  {
    for( unsigned int j = 0; j < _gridResolution[1] - 1; ++j )
    {
      for( unsigned int k = 0; k < _gridResolution[2] - 1; ++k )
      {
        CFDPoint c0 = this->_getVertexAt( i, j, k );
        CFDPoint c1 = this->_getVertexAt( i + 1, j, k );
        CFDPoint c2 = this->_getVertexAt( i, j + 1, k );
        CFDPoint c3 = this->_getVertexAt( i + 1, j + 1, k );
        CFDPoint c4 = this->_getVertexAt( i, j, k + 1 );
        CFDPoint c5 = this->_getVertexAt( i + 1, j, k + 1 );
        CFDPoint c6 = this->_getVertexAt( i, j + 1, k + 1 );
        CFDPoint c7 = this->_getVertexAt( i + 1, j + 1, k + 1 );

        osg::Vec3d p0 ( c0.pos[0], c0.pos[1], c0.pos[2] );
        osg::Vec3d p1 ( c1.pos[0], c1.pos[1], c1.pos[2] );
        osg::Vec3d p2 ( c2.pos[0], c2.pos[1], c2.pos[2] );
        osg::Vec3d p3 ( c3.pos[0], c3.pos[1], c3.pos[2] );
        osg::Vec3d p4 ( c4.pos[0], c4.pos[1], c4.pos[2] );
        osg::Vec3d p5 ( c5.pos[0], c5.pos[1], c5.pos[2] );
        osg::Vec3d p6 ( c6.pos[0], c6.pos[1], c6.pos[2] );
        osg::Vec3d p7 ( c7.pos[0], c7.pos[1], c7.pos[2] );

#if 0
        vertices->push_back( p0 );vertices->push_back( p2 );vertices->push_back( p3 );vertices->push_back( p1 );//vertices->push_back( p0 );
        vertices->push_back( p0 );vertices->push_back( p4 );vertices->push_back( p6 );vertices->push_back( p2 );//vertices->push_back( p0 );
        vertices->push_back( p0 );vertices->push_back( p1 );vertices->push_back( p5 );vertices->push_back( p4 );//vertices->push_back( p0 );
        vertices->push_back( p7 );vertices->push_back( p6 );vertices->push_back( p2 );vertices->push_back( p3 );//vertices->push_back( p7 );
        vertices->push_back( p7 );vertices->push_back( p3 );vertices->push_back( p1 );vertices->push_back( p5 );//vertices->push_back( p7 );
        vertices->push_back( p7 );vertices->push_back( p6 );vertices->push_back( p4 );vertices->push_back( p5 );//vertices->push_back( p7 );
#else
        vertices->push_back( p0 );vertices->push_back( p2 );vertices->push_back( p2 );vertices->push_back( p3 );vertices->push_back( p3 );vertices->push_back( p1 );vertices->push_back( p1 );vertices->push_back( p0 );
        vertices->push_back( p0 );vertices->push_back( p4 );vertices->push_back( p4 );vertices->push_back( p6 );vertices->push_back( p6 );vertices->push_back( p2 );vertices->push_back( p2 );vertices->push_back( p0 );
        vertices->push_back( p0 );vertices->push_back( p1 );vertices->push_back( p1 );vertices->push_back( p5 );vertices->push_back( p5 );vertices->push_back( p4 );vertices->push_back( p4 );vertices->push_back( p0 );
        vertices->push_back( p7 );vertices->push_back( p6 );vertices->push_back( p6 );vertices->push_back( p2 );vertices->push_back( p2 );vertices->push_back( p3 );vertices->push_back( p3 );vertices->push_back( p7 );
        vertices->push_back( p7 );vertices->push_back( p3 );vertices->push_back( p3 );vertices->push_back( p1 );vertices->push_back( p1 );vertices->push_back( p5 );vertices->push_back( p5 );vertices->push_back( p7 );
        vertices->push_back( p7 );vertices->push_back( p6 );vertices->push_back( p6 );vertices->push_back( p4 );vertices->push_back( p4 );vertices->push_back( p5 );vertices->push_back( p5 );vertices->push_back( p7 );
#endif
        // Feedback.
        this->_incrementProgress ( update(), caller );
      }
    }
  }
  
  OsgTools::State::StateSet::setLighting( geometry->getOrCreateStateSet(), false );
  //OsgTools::State::StateSet::setLineWidth( geometry->getOrCreateStateSet(), 2 );
  geometry->setVertexArray( vertices.get() );
  osg::ref_ptr< osg::Vec4Array > colors ( new osg::Vec4Array() );
  colors->push_back( osg::Vec4f( 0.80, 0.80, 0.80, 1.0 ) );
  geometry->setColorArray( colors.get() );
  geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

  //for( unsigned int i = 0; i < vertices->size(); i+=4 )
  //{
  //  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINES, i, i + 4 ) );      
  //}

  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINES, 0, vertices->size() ) );
  //geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINE_LOOP, 4, 8 ) );
  geode->addDrawable( geometry.get() );
  group->addChild( geode.get() );

  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CFDVizDocument::CFDPoint CFDVizDocument::_getVertexAt( unsigned int i, unsigned int j, unsigned int k )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  unsigned int index = k + ( j * _gridResolution[2] ) + ( i * _gridResolution[2] * _gridResolution[1] );
  return _vertices.at( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* CFDVizDocument::_buildVectorArrows( bool drawMirror, double arrowSize, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  osg::ref_ptr< osg::Group > group ( new osg::Group );
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  osg::ref_ptr< osg::DrawElementsUInt > indices ( new osg::DrawElementsUInt( GL_LINES ) );

  for( unsigned int i = 0; i < _vertices.size(); ++i )
  {
    
    osg::Vec3d start( _vertices.at( i ).pos[0], _vertices.at( i ).pos[1], _vertices.at( i ).pos[2] );
    osg::Vec3d end ( _vertices.at( i ).dir[0], _vertices.at( i ).dir[1], _vertices.at( i ).dir[2] );
    double magnitude = end.length();
    _values.push_back( magnitude );

    if( true == _firstPass )
    {
      _firstPass = false;
      _colorRange[0] = _colorRange[1] = magnitude;
    }
    else
    {
      _colorRange[0] = Usul::Math::minimum( _colorRange[0], magnitude );
      _colorRange[1] = Usul::Math::maximum( _colorRange[1], magnitude );
    }

    end.normalize();
    end *= arrowSize;
    vertices->push_back( start );
    vertices->push_back( start + end );
    
    if( true == drawMirror )
    {
      const osg::Vec3d startMirror( start.x(), start.y(), start.z() * -1 );
      const osg::Vec3d endMirror( end.x(), end.y(), end.z() * -1 );
      vertices->push_back( startMirror );
      vertices->push_back( startMirror + endMirror ); 
    
      indices->push_back( i * 4 );
      indices->push_back( ( i * 4 ) + 1 );
      indices->push_back( ( i * 4 ) + 2 );
      indices->push_back( ( i * 4 ) + 3 );
    }
    else
    {
      indices->push_back( i * 2 );
      indices->push_back( ( i * 2 ) + 1 );
    }
     
  }

  OsgTools::State::StateSet::setLineWidth( geometry->getOrCreateStateSet(), 2 );
  OsgTools::State::StateSet::setLighting( geometry->getOrCreateStateSet(), false );
  osg::ref_ptr< osg::Vec4Array > colors ( new osg::Vec4Array() );
  colors = this->_getColorArray( drawMirror );
  geometry->setColorArray( colors.get() );
  geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
  geometry->addPrimitiveSet( indices.get() );
  geometry->setVertexArray( vertices.get() );
  geode->addDrawable( geometry.get() );
  group->addChild( geode.get() );
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4Array * CFDVizDocument::_getColorArray( bool drawMirror )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  osg::ref_ptr< osg::Vec4Array > colors ( new osg::Vec4Array() );
  for( unsigned int i = 0; i < _values.size(); ++i )
  {
    Color4 color = this->_getInterpolatedColorValue( _values.at( i ) / _colorRange[1] );
    unsigned int numj = 0;
    if( true == drawMirror )
      numj = 4;
    else
      numj = 2;
    for( unsigned int j = 0; j < numj; ++j )
    {
      colors->push_back( osg::Vec4f( 0.0, 0.0, 0.0, 1.0 ) );
      colors->push_back( osg::Vec4f( static_cast< float > ( color[0] / 255 ),
                                     static_cast< float > ( color[1] / 255 ),
                                     static_cast< float > ( color[2] / 255 ),
                                     static_cast< float > ( color[3] ) ) );
      j++;
    }
  }

  return colors.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CFDVizDocument::_readColorTable( const std::string & filename, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  std::string directory = Usul::File::directory( filename, true );
  std::string basename = Usul::File::base( filename );

  std::string file = directory + basename + ".color";

  std::ifstream in;
  in.open( file.c_str() );
  _colorTable.resize( 256 );
  for( unsigned int i = 0; i < 256; i ++ )
  {
    double temp;
    char comma;

    in >> temp >> comma
       >> _colorTable.at( i )[0] >> comma 
       >> _colorTable.at( i )[1] >> comma
       >> _colorTable.at( i )[2] >> comma
       >> _colorTable.at( i )[3];
  }
  in.close();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CFDVizDocument::_readVertices( const std::string& filename, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  std::ifstream in;
  in.open( filename.c_str() );
  std::string temp;
  const unsigned int bufSize ( 2048 ); // should be big enough
  char buffer[bufSize];

  //trim header information
#if 0
  //skip the first line
  in.getline ( buffer, bufSize );
  unsigned int numsets = 0;
  in >> numsets;
  in.getline ( buffer, bufSize );
  //skip the object poly counts
  for( unsigned int i = 0; i < numsets; ++i )
  {
    in.getline ( buffer, bufSize );
  }
#else
  //skip the first line
  in.getline ( buffer, bufSize );
#endif
  //get the number of points
  unsigned int numPoints = 0;
  in >> numPoints;
  in.getline ( buffer, bufSize );
  // Set the progress numbers.
  _progress.first = 0;
  _progress.second = numPoints;

  Usul::Policies::TimeBased update ( 1000 ); // Every second.
  
  // skip the column header
  in.getline ( buffer, bufSize );

  //Feedback
  std::cout << "Reading vertices..." << std::endl;  

  //assuming number of columns is 4
  for( unsigned int i = 0; i <  numPoints ; ++i )
  {

    // 1st point for the triangle
    double x = 0, y = 0, z = 0, s = 0, u = 0, v = 0, w = 0, t = 0;

    in >> x >> y >> z >> s >> u >> v >> w >> t; 
    in.getline ( buffer, bufSize );

    CFDPoint p;
    p.pos = Vec3d( x, y, z );
    p.dir = Vec3d( u, v, w );
    p.value = s;

    _vertices.push_back( p );

    _vertices.push_back( p );
 
    // Feedback.
    this->_incrementProgress ( update(), progress );

  }

  
  in.close();

}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CFDVizDocument::Color4 CFDVizDocument::_getInterpolatedColorValue( double value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Color4 c( 0, 0, 0, 1 );

  double cvalue = value * 255;

  unsigned int minValue = static_cast< unsigned int > ( floor( cvalue ) );
  unsigned int maxValue = minValue + 1;

  minValue = Usul::Math::minimum( minValue, static_cast< unsigned int > ( 255 ) );
  maxValue = Usul::Math::minimum( maxValue, static_cast< unsigned int > ( 255 ) );

  
  Color4 cmin = _colorTable.at( minValue );
  Color4 cmax = _colorTable.at( maxValue );

  c = cmin + ( ( cmax - cmin ) * 0.5 );
 
  return c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void CFDVizDocument::_incrementProgress ( bool state, Unknown *progress )
{
  
  unsigned int &numerator   ( _progress.first  );
  unsigned int &denominator ( _progress.second );
  this->setProgressBar ( state, numerator, denominator, progress );
  ++numerator;
  USUL_ASSERT ( numerator <= denominator );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void CFDVizDocument::menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller )
{
  typedef MenuKit::ToggleButton ToggleButton;
  typedef MenuKit::Button       Button;
  typedef MenuKit::RadioButton  Radio;

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  MenuKit::Menu::RefPtr toolsMenu ( new MenuKit::Menu ( "CFD Tools", MenuKit::Menu::VERTICAL ) );
  toolsMenu->append ( new Radio ( new CFDAnimation( me.get() ) ) );
  
  menu.append( toolsMenu );

}



///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void CFDVizDocument::animation ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _isAnimating = state;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

bool CFDVizDocument::animation ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  return _isAnimating;
}