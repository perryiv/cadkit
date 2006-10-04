
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
      const int NONE = 0;
      const int DRAG = 4;
      const int MOVE = 5;

      /// Helper class to get osg::GUIEventAdapter type.
      public ref class Type
      {
      public: 
        static int type( bool mouse )
        {
          if( mouse )
            return DRAG;
          else
            return MOVE;

          return NONE;
        }
      };

      /// Class to wrap Helios Viewer.
      public ref class Viewer
      {
      public:

        Viewer();

        void backgroundColor ( float r, float g, float b);

        // Button Press/Release
        void                  buttonPress      ( float x, float y, bool left, bool middle, bool right );
        void                  buttonRelease    ( float x, float y, bool left, bool middle, bool right );

        void                  clear();

        // Create.
        void                  create();

        // Handle Navigation.
        void                  handleNavigation ( float x, float y, bool left, bool middle, bool right, int type );

        void render();
        void resize( int w, int h );

        OsgTools::Render::Viewer*  viewer();

        unsigned int viewerPtr();      

      private:
        OsgTools::Render::Viewer* _viewer;
      };
    }
  }
}


#endif // __CADKIT_VIEWER_GLUE_VIEWER_H__
