
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

#include "osgGA/GUIEventAdapter"

namespace CadKit
{
  namespace Viewer
  {
    namespace Glue
    {
      class TimeoutSpin;

      /// Class to wrap Helios Viewer.
      public ref class Viewer
      {
      public:

        ref class RenderPasses : System::Collections::Generic::List<unsigned int>{};
        typedef System::Drawing::Color Color;
        typedef CadKit::Interfaces::PolygonMode PolygonMode;
        typedef CadKit::Interfaces::ShadeModel ShadeModel;
        typedef CadKit::Interfaces::TextureEnvironment TexEnv;
        typedef CadKit::Interfaces::TextureMode TexMode;
        delegate void TimeoutDelegate ( bool start, double span );
        typedef void (*NativeTimeoutCallback) ( bool start, double span );
        delegate void SpinDelegate ( System::Object^ sender, System::EventArgs^ args );

        Viewer();
        ~Viewer();
        !Viewer();

        enum class Type 
        { 
          NONE = 0,
          PUSH = osgGA::GUIEventAdapter::PUSH,
          RELEASE = osgGA::GUIEventAdapter::RELEASE,
          DRAG = osgGA::GUIEventAdapter::DRAG,
          MOVE = osgGA::GUIEventAdapter::MOVE
        };

        enum class ViewMode
        {
          NAVIGATION,
          PICK,
          SEEK
        };

        // These have to be the same as OsgTools::Builders::GradientBackground::Corners
        enum class Corners
        {
          BOTTOM_LEFT  = 0x00000001,
          BOTTOM_RIGHT = 0x00000002,
          TOP_LEFT     = 0x00000004,
          TOP_RIGHT    = 0x00000008,
          LEFT         = BOTTOM_LEFT  | TOP_LEFT,
          RIGHT        = BOTTOM_RIGHT | TOP_RIGHT,
          BOTTOM       = BOTTOM_LEFT  | BOTTOM_RIGHT,
          TOP          = TOP_LEFT     | TOP_RIGHT,
          ALL          = BOTTOM       | TOP
        };

        // Get list of available render passes.
        RenderPasses^         availableRenderPasses();

        void                  backgroundColor ( Color^ color );
        Color^                backgroundColor();
        void                  backgroundCorners ( Corners corners );
        Corners               backgroundCorners();

        // Button Press/Release
        void                  buttonPress      ( float x, float y, bool left, bool middle, bool right, Type type );
        void                  buttonRelease    ( float x, float y, bool left, bool middle, bool right, Type type );
        void                  mouseMove        ( float x, float y, bool left, bool middle, bool right, Type type );

        void                  clear();

        // Create.
        void                  create();

        void                  camera ( CadKit::Interfaces::CameraOption option );

        // Set/get the mode.
        void                  mode ( ViewMode mode );
        ViewMode              mode();

        // Number of render passes.
        unsigned int          numRenderPasses();
        void                  numRenderPasses ( unsigned int );
  
        // Set/query/remove the polygon mode.
        PolygonMode::Mode     polygonMode    ( PolygonMode::Face face );
        void                  polygonMode    ( PolygonMode::Mode mode, PolygonMode::Face face );
        void                  polygonMode    ( PolygonMode::Mode mode );

        // Render the scene.
        void                  render();

        // Resize the viewport.
        void                  resize( int w, int h );

        // Set/get the scatter scale.
        double                scatterScale();
        void                  scatterScale ( double );

        // Set/query/remove the shading model.
        ShadeModel::Model     shadeModel();
        void                  shadeModel ( ShadeModel::Model model );

        // Set/query/remove the texture environment.
        TexEnv::Mode          textureEnvironment();
        void                  textureEnvironment ( TexEnv::Mode mode );

        // Set/get the texture mode.
        bool                  textureMode ( TexMode::Mode mode );
        void                  textureMode ( TexMode::Mode mode, bool state );

        // Set all the display-lists to on/off.
        bool                  useDisplayLists();
        void                  useDisplayLists ( bool state );
        void                  updateDisplayListUse();

        /// Set the render context.
        property CadKit::OpenGL::Glue::RenderContext^ RenderContext
        {
          void set( CadKit::OpenGL::Glue::RenderContext^ context );
        };

        /// Get the viewer.
        System::IntPtr viewer();

        /// Get/Set the scene.
        property CadKit::OSG::Glue::Node ^ Scene
        {
          CadKit::OSG::Glue::Node^ get();
          void set ( CadKit::OSG::Glue::Node ^node );
        };

        /// Get/Set the matrix manipulator.
        property CadKit::OSG::Glue::MatrixManipulator ^ MatrixManipulator
        {
          CadKit::OSG::Glue::MatrixManipulator^ get();
          void set ( CadKit::OSG::Glue::MatrixManipulator ^node );
        };

        // Write the current frame to an image file.
        bool          writeImageFile ( System::String^ filename );

        // Write the current model or scene to file.
        bool          writeModelFile ( System::String^ filename );
        bool          writeSceneFile ( System::String^ filename );

        // Get/Set frame dump directory.
        property System::String^ Directory
        {
          System::String^ get();
          void set ( System::String^ string );
        }

        // Get/Set filename.
        property System::String^ Filename
        {
          System::String^ get();
          void set ( System::String^ string );
        };

        // Get/Set extension.
        property System::String^ Extension
        {
          System::String^ get();
          void set ( System::String^ string );
        };

        // Get/Set DumpFrames.
        property bool DumpFrames
        {
          bool get();
          void set ( bool b );
        };

        float                     frameDumpScale();
        void                      frameDumpScale ( float scale );
        void                      computeNearFar( bool b );

      protected:

        NativeTimeoutCallback     _makeProgressCallback();
        void                      _onSpinTick ( System::Object^ sender, System::EventArgs^ args );
        void                      _timeoutNotify ( bool start, double span );
        std::string               _toString ( System::String^ source );

      private:

        OsgTools::Render::Viewer* _viewer;
        TimeoutSpin *_proxy;
        System::Runtime::InteropServices::GCHandle _pin;
        System::Windows::Forms::Timer ^_timeout;
      };
    }
  }
}


#endif // __CADKIT_VIEWER_GLUE_VIEWER_H__
