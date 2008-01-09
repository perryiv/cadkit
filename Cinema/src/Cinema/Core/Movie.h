
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

#ifndef __CINEMA_MOVIE_H__
#define __CINEMA_MOVIE_H__

#include <string> 

#include "osg/Referenced"

#include "osg/Group"
#include "osg/Geometry"

#include "MovieHandler.h"
#include "Export.h"

#include <iostream>

namespace Cinema
{
  namespace Core
  {
    class CINEMA_CORE_EXPORT Movie : public osg::Referenced
    {
      public: 
        
        enum ScreenShape
        {
          QUAD = 0          
        };    
        
        static Movie*                     instance();

        virtual osg::Node*                root();

        bool                              dirty() const;
        void                              dirty( bool );

        bool                              allowRotation() const;
        void                              allowRotation( bool );        

        bool                              usePositionAsCenter() const;
        void                              usePositionAsCenter( bool );
    
        ScreenShape                       screenShape() const;
        void                              screenShape( Cinema::Core::Movie::ScreenShape );
        
        const float&                      screenWidth() const;
        void                              screenWidth( const float& );
  
        const float&                      screenHeight() const;
        void                              screenHeight( const float& );        

        const osg::Vec3f&                 screenWidthVector() const;
        void                              screenWidthVector( const osg::Vec3f& );

        const osg::Vec3f&                 screenHeightVector() const;
        void                              screenHeightVector( const osg::Vec3f& );

        const osg::Vec3f&                 screenPosition() const;
        void                              screenPosition( osg::Vec3f );
      
        void                              fileName( const std::string& );

        virtual void                      setMovie( const osg::Vec3f&, const std::string& );
        virtual void                      setMovie( const osg::Vec3f&, const osg::Vec3f&, const osg::Vec3f&, const std::string& );
        virtual void                      setMovie( const float&, const float&, const osg::Vec3f&, const std::string& );


        virtual void                      play();

        virtual void                      stop();

        virtual void                      pause();

        virtual void                      restart();

        virtual void                      loop( bool );

        virtual bool                      isPlaying();

        virtual bool                      isPaused();

        virtual void                      buildScene();

      protected: 

        Movie() : 
          osg::Referenced(),
	        mDirty		                  ( true ),  
          mAllowRotation              ( false ), 
          mUseTextureRectangle        ( true ), 
          mBuildGeometry              ( true ), 
          mUsePositionAsCenterPoint   ( false ), 
          mScreenShape                ( QUAD ), 
          mScreenWidth                ( 0.0 ),           
          mScreenHeight               ( 0.0 ),
          mScreenWidthVector          ( osg::Vec3f() ),
          mScreenHeightVector         ( osg::Vec3f() ),
          mPosition                   ( osg::Vec3f( 0.0, 0.0, 0.0 ) ), 
          mFileName                   (),
          mRoot                       ( new osg::Group() ),           
          mMovieHandler               ( new MovieHandler() )
        {
          if( !mRoot.valid() )
          {
            throw "Error 4085586713: Failed to initialize Movie.";
          }         
        }    

        Movie( osg::Vec3f position, const std::string& fileName ) :
	        mDirty		               ( true ),  
          mAllowRotation           ( false ), 
          mUseTextureRectangle     ( true ),
          mBuildGeometry           ( true ), 
          mUsePositionAsCenterPoint( false ), 
          mScreenShape             ( QUAD ), 
          mScreenWidth             ( 0.0 ), 
          mScreenHeight            ( 0.0 ),
          mScreenWidthVector       ( osg::Vec3f() ),
          mScreenHeightVector      ( osg::Vec3f() ),
          mPosition                ( position ), 
          mFileName                (), 
          mRoot                    ( new osg::Group() ),          
          mMovieHandler            ( new MovieHandler() )
        {
          if( !mRoot.valid() )
          {
            throw "Error 7174831750: Failed to initialize Movie.";
          }
        }

        Movie( float screenWidth, float screenHeight, osg::Vec3f position, const std::string& fileName ) : 
          mDirty		               ( true ), 
          mAllowRotation           ( false ), 
          mUseTextureRectangle     ( true ),
          mBuildGeometry           ( true ), 
          mUsePositionAsCenterPoint( false ), 
          mScreenShape             ( QUAD ), 
          mScreenWidth             ( screenWidth ), 
          mScreenHeight            ( screenHeight ), 
          mScreenWidthVector       ( osg::Vec3f() ),
          mScreenHeightVector      ( osg::Vec3f() ),
          mPosition                ( position ), 
          mFileName                ( fileName ), 
          mRoot                    ( new osg::Group() ),          
          mMovieHandler            ( new MovieHandler() )
        {
          if( !mRoot.valid() )
          {
            throw "Error 2654299413: Failed to initialize Movie.";
          }          
        }

        virtual ~Movie()
        {
          mMovieHandler->cleanUp();
        }       

        osg::Geometry*                          buildGeometry( osg::Image* );        

        osg::Geometry*                          buildQuad( osg::Image* );        

        bool                                    mDirty;

        bool                                    mAllowRotation;
        
        bool                                    mUseTextureRectangle;  

        bool                                    mBuildGeometry;

        bool                                    mUsePositionAsCenterPoint;

        ScreenShape                             mScreenShape;

        float                                   mScreenWidth;
        float                                   mScreenHeight;

        osg::Vec3f                              mScreenWidthVector;
        osg::Vec3f                              mScreenHeightVector;

        osg::Vec3f                              mPosition;        

        std::string                             mFileName;  

        osg::ref_ptr< osg::Group >              mRoot;
    
        osg::ref_ptr< MovieHandler >            mMovieHandler;

        static osg::ref_ptr< Movie >            mInstance;
    };
  }
}

#endif //__MOVIE_H__
