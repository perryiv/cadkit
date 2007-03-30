
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

#include "OsgTools/Render/Viewer.h"

#include "Minerva/Plugins/PlayMovie/PlayMovieComponent.h"

#include "Usul/Interfaces/ISceneIntersect.h"
#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IDistributedVR.h"
#include "Usul/Interfaces/IGeometryCenter.h"

#include "Minerva/Core/DataObjects/DataObject.h"
#include "Minerva/Core/DataObjects/UserData.h"
#include "Minerva/Core/DB/Connection.h"
#include "Minerva/Core/postGIS/Point.h"

#include "osgUtil/IntersectVisitor"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( PlayMovieComponent, PlayMovieComponent::BaseClass );


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
  case Usul::Interfaces::IPlayMovie::IID:
    return static_cast < Usul::Interfaces::IPlayMovie*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::ITool::IID:
    return static_cast < Usul::Interfaces::ITool* >( this );
  case Usul::Interfaces::IToolLifeTime::IID:
    return static_cast< Usul::Interfaces::IToolLifeTime* >( this );
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
          query << "SELECT asBinary(quad_position), height, width, lin_path, win_path, id FROM " << movieTable << " WHERE row_id=" << rowId;

          pqxx::result result ( connection->executeQuery ( query.str() ) );

          if( false == result.empty() )
          {
            Minerva::Core::postGIS::Point::RefPtr positionGeom ( new Minerva::Core::postGIS::Point ( connection.get(), movieTable, result[0]["id"].as < int > (), 4326, result[0][0] ) );

            Usul::Interfaces::IGeometryCenter::QueryPtr position ( positionGeom );

            if( position.valid() )
            {
              std::string path ( result[0]["lin_path"].as < std::string > () );
              
              // Send command.
              distributed->playMovie ( position->geometryCenter(), result[0]["width"].as < float > (), result[0]["height"].as < float > (), path );

              // Play movie on windows side. 
              Usul::Interfaces::IGroup::QueryPtr gr( caller );
              if( gr.valid() )
              {   
                osg::ref_ptr< osg::Group > group( gr->getGroup( "movie_node_4047781649" ) );
                std::string path ( result[0]["win_path"].as < std::string > () );

                std::istringstream in;

                float width    = result[0]["width"].as < float >();
                float height   = result[0]["height"].as < float >();
                
                osg::Vec3f widthVec  = osg::Vec3f( width, 0.0, 0.0 );
                osg::Vec3f heightVec = osg::Vec3f( 0.0,   0.0, height );

                osg::ref_ptr< osg::Node >  node ( playMovie ( position->geometryCenter(), widthVec, heightVec, path ) );

                if( node.valid() )
                {
                  group->removeChildren( 0, group->getNumChildren() );
                  group->addChild( node.get() );
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
#if 0
    else
    {
      Usul::Interfaces::IGroup::QueryPtr gr( caller );
      if( gr.valid() )
      {   
        osg::ref_ptr< osg::Group > group( gr->getGroup( "movie_node_4047781649" ) );
        osg::ref_ptr< osg::Node >  node ( playMovie( osg::Vec3f(), osg::Vec3f(), osg::Vec3f(), "test.avi" ) );

        if( node.valid() )
        {
          group->removeChildren( 0, group->getNumChildren() );
          group->addChild( node.get() );
        }    
       }
    }
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
  _movie->setMovie( position, widthVector, heightVector, fileName );
  _movie->buildScene();
  _movie->play();          

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
