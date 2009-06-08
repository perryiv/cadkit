
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

#ifndef __CINEMA_MOVIE_HANDLER_H__
#define __CINEMA_MOVIE_HANDLER_H__

#include <vector> 

#include "osg/ImageStream"
#include "osg/NodeVisitor"
#include "osg/Texture2D"
#include "osg/TextureRectangle"

#include "osg/Geode"
#include "osg/Group"
#include "osg/MatrixTransform"
#include "osg/Billboard"

#include "Export.h"

#include <iostream> 

namespace Cinema
{
  namespace Core
  {
    typedef std::vector< osg::ref_ptr< osg::ImageStream > > ImageStreamList;

    class CINEMA_CORE_EXPORT FindImageStreamsVisitor : public osg::NodeVisitor
    {
      public:
        FindImageStreamsVisitor( ImageStreamList& imageStreamList ): mImageStreamList( imageStreamList ) {}
            
        virtual void apply( osg::Geode& geode ) 
        {   	        
            apply( geode.getStateSet() );

            for( unsigned int i = 0; i < geode.getNumDrawables(); ++i )
            {
                apply( geode.getDrawable(i)->getStateSet() );
            }
        
            traverse( geode );
        }

        virtual void apply( osg::MatrixTransform& transform )
        {   	        
            apply( transform.getStateSet() );
  
            for( size_t i=0; i < transform.getNumChildren(); ++i )
            {
              osg::ref_ptr< osg::Geode > ge ( dynamic_cast< osg::Geode* >( transform.getChild( i ) ) );
              if( ge.valid() )
              {
                apply( *( ge.get() ) );
              }              
              else
              {
                osg::ref_ptr< osg::Billboard > bb ( dynamic_cast< osg::Billboard* >( transform.getChild( i ) ) );
                if( bb.valid()  )
                {
                  apply( *( bb.get() ) );
                }
                else
                {   
                  apply( *( transform.getChild( i ) ) );
                }
              }
            }

		        traverse( transform );
        }

        virtual void apply( osg::Billboard& billboard )
        {   	        
            apply( billboard.getStateSet() );

            for( unsigned int i = 0; i < billboard.getNumDrawables(); ++i )
            {
                apply( billboard.getDrawable(i)->getStateSet() );
            }

            traverse( billboard );
        }

        virtual void apply( osg::Node& node )
        {   	    
            apply( node.getStateSet() );

            apply( *( node.asGroup() ) );
            apply( *( node.asTransform() ) );

            traverse( node );
        }

        virtual void apply( osg::Transform& transform )
        {
          apply( transform.getStateSet() );
          apply( *( transform.asMatrixTransform() ) );

          traverse( transform );
        }
       
    	virtual void apply( osg::Group& group )
	    { 
		    apply( group.getStateSet() );

        for( size_t i=0; i < group.getNumChildren(); ++i )
        {          
          osg::ref_ptr< osg::Geode > ge ( dynamic_cast< osg::Geode* >( group.getChild( i ) ) );
          if( ge.valid() )
          {
            apply( *( ge.get() ) );
          }
          else
          {
            osg::ref_ptr< osg::Billboard > bb ( dynamic_cast< osg::Billboard* >( group.getChild( i ) ) );
            if( bb.valid()  )
            {
              apply( *( bb.get() ) );
            }
            else
            {                
              apply( *( group.getChild( i ) ) );
            }
          }
        }
		    traverse( group );
	    }
 
      inline void apply( osg::StateSet* stateSet )
      {          
          if( !stateSet ) return;          
      
          osg::ref_ptr< osg::StateAttribute > attr = stateSet->getTextureAttribute( 0,osg::StateAttribute::TEXTURE );
          if ( attr.valid() )
          {
            osg::ref_ptr< osg::Texture2D > texture2D = dynamic_cast< osg::Texture2D* >( attr.get() );
            if (texture2D.valid() ) apply( dynamic_cast< osg::ImageStream* >( texture2D->getImage() ) );

            osg::ref_ptr< osg::TextureRectangle > textureRec = dynamic_cast< osg::TextureRectangle* >( attr.get() );
            if ( textureRec.valid() ) apply( dynamic_cast< osg::ImageStream* >( textureRec->getImage() ) );
          }
      }
        
      inline void apply( osg::ImageStream* imageStream )
      {     
        if( imageStream )
        {
  
            mImageStreamList.push_back( imageStream ); 
            
            // No need for this as of now. 
            //s_imageStream = imageStream;
        }
      }

      ImageStreamList& mImageStreamList;
    };

    class CINEMA_CORE_EXPORT MovieHandler : public osg::Referenced
    {
      public: 

        enum Action 
        {
          PLAY  = 0, 
          PAUSE, 
          STOP, 
          RESTART, 
          REWIND,
          FORWARD
        };

        enum Mode
        {
          LOOP    = 0, 
          NO_LOOP,
        };

        MovieHandler() : 
          osg::Referenced (), 
          mAction         ( PLAY ), 
          mMode           ( LOOP )
        {
        }

        void              set( osg::Node* );

        Action            action() const;
        void              action( Action );

        Mode              mode() const;
        void              mode( Mode );

        void              update();

        void              cleanUp();

        bool              isPlaying();

        bool              isPaused();

      protected:

        ~MovieHandler()
        {
          cleanUp();
        }

      protected: 

        ImageStreamList   mImageStreamList;

        Action            mAction;

        Mode              mMode;
    };
  }
}

#endif // __CINEMA_MOVIE_HANDLER_H__

