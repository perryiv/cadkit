
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>

#include "Minerva/Plugins/PlayMovie/PlayMovieComponent.h"
#include "Minerva/Core/Layers/Layer.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/ISceneIntersect.h"
#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IDistributedVR.h"
#include "Usul/Interfaces/IGeometryCenter.h"
#include "Usul/Interfaces/IProjectCoordinates.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"
#include "Usul/Interfaces/IGroup.h"
#include "Usul/Interfaces/IToolLifeTime.h"

#include "Minerva/Core/DataObjects/DataObject.h"
#include "Minerva/Core/DataObjects/UserData.h"
#include "Minerva/Core/DB/Connection.h"
#include "Minerva/Core/postGIS/Point.h"

#include "osg/io_utils"

#include "osgUtil/IntersectVisitor"

using namespace osg;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( PlayMovieComponent, PlayMovieComponent::BaseClass );

// TODO: This should be moved into it's own file.
class MovieLayer : public Minerva::Core::Layers::Layer
{
	public:

	typedef Minerva::Core::Layers::Layer	BaseClass;

	USUL_DECLARE_REF_POINTERS ( MovieLayer );

	MovieLayer() : BaseClass(), 
		_position( 0.0, 0.0, 0.0 ),
		_width( 0.0, 0.0, 0.0 ),
		_height( 0.0, 0.0, 0.0 ), 
		_fileName( std::string( "" ) )		
	{					
	}

	virtual Usul::Interfaces::IUnknown* clone() const 
	{
    MovieLayer::RefPtr layer ( new MovieLayer );                  // Reference count is now 1
    Usul::Interfaces::IUnknown::QueryPtr unknown ( layer.get() ); // Reference count is now 2
    layer.release();                                              // Reference count is now 1
    return unknown.release();                                     // Reference count is now 0
	}	  

	void buildDataObjects( Usul::Interfaces::IUnknown* ){}

	void modify( Usul::Interfaces::IUnknown* ){} 

	void buildScene( osg::Group* gr )
	{
    if( gr )
    {
      // Call play movie function and pass in group, width, height, position, and path.
      Usul::Interfaces::IPlayMovie::QueryPtr playMoviePlug ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IPlayMovie::IID ) );

      if( playMoviePlug.valid() )
      {
        gr->addChild( playMoviePlug->playMovie( _position, _width, _height, _fileName ) );
      }
    }	
	}

	void setValues( const osg::Vec3f& position, const osg::Vec3f& width, const osg::Vec3f& height, const std::string& fileName )
	{
		_position = position;
		_width    = width;
		_height   = height;
		_fileName = fileName;
	}  

	protected:
	
	osg::Vec3f  _position;
	osg::Vec3f  _width;
	osg::Vec3f  _height;

	std::string _fileName;	 
};

///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

PlayMovieComponent::PlayMovieComponent()
{
  _movie = Cinema::Core::Movie::instance();
  
  if( !_movie.valid() )
  {
    throw std::runtime_error ( "Error 1105724513: Failed to create or get an instance." );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

PlayMovieComponent::~PlayMovieComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////
Usul::Interfaces::IUnknown *PlayMovieComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  default:
    return 0x0;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Life time for the object.
//
///////////////////////////////////////////////////////////////////////////////
bool PlayMovieComponent::temporary()
{
  return false;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Function that gets executed on event of button release / pressed. 
//
///////////////////////////////////////////////////////////////////////////////
bool PlayMovieComponent::execute ( Unknown* caller, bool left, bool middle, bool right, bool motion, float x, float y, float z )
{ 
  #if 1
  std::cout << "execute: " << std::endl;
  Usul::Interfaces::ISceneIntersect::QueryPtr intersect ( caller );

  if( intersect.valid() )
  {
    osgUtil::Hit hit;
    if ( intersect->intersect( x, y, hit ) )
    {
      osg::ref_ptr < Minerva::Core::DataObjects::UserData > userdata ( 0x0 );
      for( osg::NodePath::reverse_iterator iter = hit._nodePath.rbegin(); iter != hit._nodePath.rend(); ++iter )
      {
        if( Minerva::Core::DataObjects::UserData *ud = dynamic_cast < Minerva::Core::DataObjects::UserData *> ( (*iter)->getUserData() ) )
          userdata = ud;
      }

      if( userdata.valid() )
      {
        std::string tablename ( userdata->_do->tableName() );
        unsigned int rowId ( userdata->_do->rowId() );

        /// Get the document.
        Usul::Interfaces::IGetDocument::QueryPtr getDocument ( caller );
        if( getDocument.valid () )
        {          
          // Query for IDistributedVR
          Usul::Interfaces::IDistributedVR::QueryPtr distributed ( getDocument->getDocument() );          

          Minerva::Core::DB::Connection::RefPtr connection ( userdata->_do->connection() );

          std::string movieTable ( "media" );
          std::ostringstream query;
          query << "SELECT asBinary(quad_position), height, width, asEWKT( quad_width ), asEWKT( quad_height ), lin_path, win_path, id FROM " << movieTable << " WHERE row_id=" << rowId;

          pqxx::result result ( connection->executeQuery ( query.str() ) );

          if( false == result.empty() )
          {
            Minerva::Core::postGIS::Point::RefPtr positionGeom ( new Minerva::Core::postGIS::Point ( connection.get(), movieTable, result[0]["id"].as < int > (), 4326, result[0][0] ) );

            Usul::Interfaces::IGeometryCenter::QueryPtr position ( positionGeom );

            unsigned int srid ( 0 );
            osg::Vec3 center ( position->geometryCenter( srid ) );
            Usul::Interfaces::IProjectCoordinates::QueryPtr project ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IProjectCoordinates::IID ) );
            Usul::Interfaces::IPlanetCoordinates::QueryPtr  planet  ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IPlanetCoordinates::IID ) );

            if( project.valid() )
            {
              Usul::Math::Vec3d orginal;
              orginal[0] = center[0];
              orginal[1] = center[1];
              orginal[2] = center[2];
              Usul::Math::Vec3d point;
              project->projectToSpherical( orginal, srid, point );

              if( planet.valid() )
              {
                planet->convertToPlanet( point, point );
                center.set ( point[0], point[1], point[2] );
              }
            }

            if( position.valid() )
            {
              std::string path ( result[0]["lin_path"].as < std::string > () );
              
              std::string quadWidth( result[0][3].as< std::string >() );
              std::string quadHeight( result[0][4].as< std::string >() );             
              
              // Send command.
              distributed->playMovie( center, 
                                      convertStringToPosition( quadWidth ), 
                                      convertStringToPosition( quadHeight ), 
                                      path );

              // Play movie on windows side. 
              Usul::Interfaces::IGroup::QueryPtr gr( getDocument->getDocument() );

              if( gr.valid() )
              {   
                osg::ref_ptr< osg::Group > group( gr->getGroup( "movie_node_4047781649" ) );
                std::string path ( result[0]["win_path"].as < std::string > () );                
             
                try
                {
                  osg::ref_ptr< osg::Node >  node ( playMovie ( center, convertStringToPosition( quadWidth ), convertStringToPosition( quadHeight ), path ) );
                  if( node.valid() )
                  {
                    group->removeChildren( 0, group->getNumChildren() );
                    group->addChild( node.get() );
                  }
                }
                catch( ... )
                {
                }                
              }
            }
          }
        }        
      }
      else
      {

      }
    }
#endif
#if 0
    //else    

    static int i = 0;    

    if( i == 0 )
    {
      Usul::Interfaces::IGroup::QueryPtr gr( caller );
      if( gr.valid() )
      { 
        osg::ref_ptr< osg::Group > group( gr->getGroup( "movie_node_4047781649" ) );
        osg::ref_ptr< osg::Node >  node ( playMovie( osg::Vec3f( 0.0, 0.0, 0.0 ), osg::Vec3f( 2.0, 0.0, 0.0 ), osg::Vec3f( 0.0, 0.0, 2.0 ), "C:\\movie\\other.mpg" ) );

        if( node.valid() && group.valid() )
        {
          group->removeChild( 0, 1 );
          group->addChild( node.get() );
        }    
       }
    }
    i = ( i + 1 ) % 2;
    
#endif 

  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Play movie. 
//
///////////////////////////////////////////////////////////////////////////////
void PlayMovieComponent::play()
{
  _movie->play();  
}

///////////////////////////////////////////////////////////////////////////////
//
//  Pause movie. 
//
///////////////////////////////////////////////////////////////////////////////
void PlayMovieComponent::pause()
{
  _movie->pause();  
}

///////////////////////////////////////////////////////////////////////////////
//
//  Restart movie. 
//
///////////////////////////////////////////////////////////////////////////////
void PlayMovieComponent::restart()
{
  _movie->restart();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Is the movie playing? 
//
///////////////////////////////////////////////////////////////////////////////
bool PlayMovieComponent::isPlaying()
{
  return _movie->isPlaying();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Is the movie paused? 
//
///////////////////////////////////////////////////////////////////////////////
bool PlayMovieComponent::isPaused()
{
  return _movie->isPaused();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set a movie as animated texture. 
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* PlayMovieComponent::playMovie( const osg::Vec3f& position, const osg::Vec3f& widthVector, const osg::Vec3f& heightVector, const std::string& fileName )
{
  // For test purposes. 
  //_movie->setMovie( osg::Vec3f( -2.0, 0.0, 0.0 ), osg::Vec3f( 2.0, 2.0, 0.0 ), osg::Vec3f( 0.0, 0.0, 2.0 ), "C:\\Aashish\\src\\bin\\test5.avi" );
  _movie->stop();
  _movie->setMovie( position, widthVector, heightVector, fileName );
  _movie->buildScene();
  
  //_movie->play();          

  return _movie->root();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IPlugin implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string PlayMovieComponent::getPluginName() const 
{
  return "Play Movie";
}

///////////////////////////////////////////////////////////////////////////////
//
//  Convert postGIS ( EWKT ) string to osg::Vec. 
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3f PlayMovieComponent::convertStringToPosition( const std::string& str )
{
  osg::Vec3f resultVec( 0.0, 0.0, 0.0 );
  std::string::size_type loc1, loc2, count; 
  
  loc1  = str.find( "(", 0 );
  loc2  = str.find( ")", 0 );
  count = loc2 - ( loc1 + 1 );
  
  std::string temp( str.substr( loc1+1, count ) );
  std::istringstream in( temp );
  
  if( in.good() )
  {
    in >> resultVec;
  }               
  
  return resultVec;
  
}
