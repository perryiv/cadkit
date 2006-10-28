
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

        enum class CameraOption { FIT, RESET, FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM };

        enum class ViewMode
        {
          NAVIGATION,
          PICK,
          SEEK
        };

        void backgroundColor ( float r, float g, float b);

        // Button Press/Release
        void                  buttonPress      ( float x, float y, bool left, bool middle, bool right );
        void                  buttonRelease    ( float x, float y, bool left, bool middle, bool right );

        void                  clear();

        // Create.
        void                  create();

        void                  camera ( CameraOption );

        // Handle Navigation.
        void                  handleNavigation ( float x, float y, bool left, bool middle, bool right, Type type );
        void                  handleSeek ( float x, float y, bool left );

        void                  render();
        void                  resize( int w, int h );

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
            node->nodePtr ( reinterpret_cast < unsigned int > ( _viewer->scene() ) );
            return node;
          }
          void set ( CadKit::OSG::Glue::Node ^node )
          {
            _viewer->scene ( reinterpret_cast < osg::Node* > ( node->nodePtr() ) );
          }
        };

      private:
        OsgTools::Render::Viewer* _viewer;
      };
    }
  }
}


#endif // __CADKIT_VIEWER_GLUE_VIEWER_H__
