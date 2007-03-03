
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning ( disable : 4561 )
#include "Minerva/Core/Scene/SceneManager.h"
#include "OsgTools/Render/Viewer.h"
#include "Magrathea/Planet.h"

using namespace System;

namespace DT
{
  namespace Minerva
  {
    namespace Plugins
    {
      namespace Document
      {
        namespace Glue 
        {
	        public ref class DllGlue
	        {
          public:

            DllGlue();
            ~DllGlue();
            !DllGlue();
		        
            void removeLayer( CadKit::Interfaces::ILayer ^layer );
            void showLayer( CadKit::Interfaces::ILayer ^layer, CadKit::Threads::Jobs::Progress ^progess );
            void modifyLayer( CadKit::Interfaces::ILayer ^layer, CadKit::Threads::Jobs::Progress ^progess );

            void viewLayerExtents( CadKit::Interfaces::ILayer ^layer );

            void                  addKeyWordList( System::String^ kwl );
            void                  addLayer( CadKit::Interfaces::IOssimLayer ^ layer );
            void                  removeLayer( CadKit::Interfaces::IOssimLayer ^ layer );
      
            void                  setLayerOperation( System::String^ optype, int val, CadKit::Interfaces::ILayer^ layer );


            void dirtyScene();

            void startAnimation(float speed, bool accumulate, bool dateTimeStep, bool timeWindow, int numDays);

            void stopAnimation();

            void viewer( CadKit::Viewer::Glue::Viewer ^viewer );

            void resize ( int h, int w );

            bool                  elevationEnabled();
            void                  elevationEnabled( bool val );

            bool                  hudEnabled();
            void                  hudEnabled( bool val );

            bool                  ephemerisFlag();
            void                  ephemerisFlag( bool val );

            float                 elevationExag();
            void                  elevationExag( float elevExagVal );

            int                   elevationPatchSize();
            void                  elevationPatchSize( float elevEstimateVal );

            int                   levelDetail();
            void                  levelDetail( float levelDetailVal );

            System::String^       elevationCacheDir();
            void                  elevationCacheDir( System::String^ directory );

            bool                  latLongGrid();
            void                  latLongGrid( bool b );

            bool                  showLegend();
            void                  showLegend( bool b );

          private:
            OsgTools::Render::Viewer *_viewer;
            ::Minerva::Core::Scene::SceneManager *_sceneManager;
            Magrathea::Planet *_planet;
	        };
        }
      }
    }
  }
}

