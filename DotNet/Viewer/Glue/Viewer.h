
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Glue code for OsgTools::Render::Viewer
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_VIEWER_GLUE_VIEWER_H__
#define __CADKIT_VIEWER_GLUE_VIEWER_H__

#include "OsgTools/Render/Viewer.h"

namespace CadKit 
{
  namespace Viewer 
  {
    namespace Glue
    {

      /// Class to wrap Helios Viewer.
      public ref class Viewer
      {
      public:

        Viewer();
        ~Viewer();
        !Viewer();

        enum class Type 
        { 
          NONE = 0,
          DRAG = 4,
          MOVE = 5
        };

        enum class ViewMode
        {
          NAVIGATION,
          PICK,
          SEEK
        };

        void                  backgroundColor ( float r, float g, float b);

        // Button Press/Release
        void                  buttonPress      ( float x, float y, bool left, bool middle, bool right );
        void                  buttonRelease    ( float x, float y, bool left, bool middle, bool right );

        void                  clear();

        // Create.
        void                  create();

        void                  camera ( CadKit::Interfaces::CameraOption option );

        // Handle Navigation.
        void                  handleNavigation ( float x, float y, bool left, bool middle, bool right, Type type );
        void                  handleSeek ( float x, float y, bool left );

        // Number of render passes.
        unsigned int          numRenderPasses();
        void                  numRenderPasses ( unsigned int );

        // Render the scene.
        void                  render();

        // Resize the viewport.
        void                  resize( int w, int h );

        // Set/get the scatter scale.
        double                scatterScale();
        void                  scatterScale ( double );

        // Set/get the mode.
        void                  setMode ( ViewMode mode );
        ViewMode              getMode ();

        /// Set the render context.
        property CadKit::OpenGL::Glue::RenderContext^ RenderContext
        {
          void set( CadKit::OpenGL::Glue::RenderContext^ context )
          {
            if( _viewer )
            {
              System::IntPtr contextImpl = context->unmanagedRenderContext();
              _viewer->context( reinterpret_cast < Usul::Interfaces::IUnknown* > ( contextImpl.ToPointer() ) );
            }
          }
        };

        /// Get the viewer.
        System::IntPtr viewer();

        /// Get/Set the scene.
        property CadKit::OSG::Glue::Node ^ Scene
        {
          CadKit::OSG::Glue::Node^ get()
          {
            CadKit::OSG::Glue::Node ^node = gcnew CadKit::OSG::Glue::Node();
            node->nodePtr ( System::IntPtr ( _viewer->scene() ) );
            return node;
          }
          void set ( CadKit::OSG::Glue::Node ^node )
          {
            _viewer->scene ( reinterpret_cast < osg::Node* > ( node->nodePtr().ToPointer() ) );
          }
        };

        // Write the current frame to an image file.
        bool          writeImageFile ( System::String^ filename );

        // Write the current scene to file.
        bool          writeSceneFile ( System::String^ filename );

        property System::String^ Directory
        {
          System::String^ get()
          {
            return gcnew System::String ( _viewer->frameDump().dir().c_str() );
          }
          void set ( System::String^ string )
          {
            _viewer->frameDump().dir ( this->toString( string ) );
          }
        }

        property System::String^ Filename
        {
          System::String^ get()
          {
            return gcnew System::String ( _viewer->frameDump().base().c_str() );
          }
          void set ( System::String^ string )
          {
            _viewer->frameDump().base ( this->toString( string ) );
          }
        };

        property System::String^ Extension
        {
          System::String^ get()
          {
            return gcnew System::String ( _viewer->frameDump().ext().c_str() );
          }
          void set ( System::String^ string )
          {
            _viewer->frameDump().ext ( this->toString( string ) );
          }
        };

        property bool DumpFrames
        {
          bool get()
          {
            return _viewer->frameDump().dump();
          }
          void set ( bool b )
          {
            _viewer->frameDump().digits( 10 );
            _viewer->frameDump().start( 0 );
            _viewer->frameDump().dump ( b );
          }
        };

        unsigned int            frameDumpWidth();
        unsigned int            frameDumpHeight();
        void                    frameDumpSize ( unsigned int width, unsigned int height );

        bool                    frameDumpUseFrameSize();
        void                    frameDumpUseFrameSize ( bool );

      protected:

        std::string toString( System::String^ source )
        {
          std::string answer;
          System::IntPtr ptr ( 0 );
          try
          {
            ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( source );
            char* s = (char*)(void*) ptr;
            answer.assign ( s );
          }
          finally
          {
            System::Runtime::InteropServices::Marshal::FreeHGlobal( ptr );
          }
          return answer;
        }

      private:
        OsgTools::Render::Viewer* _viewer;
      };
    }
  }
}


#endif // __CADKIT_VIEWER_GLUE_VIEWER_H__
