
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning ( disable : 4561 )
#include "Minerva/Scene/SceneManager.h"
#include "OsgTools/Render/Viewer.h"

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

            void dirtyScene();

            void startAnimation(float speed, bool accumulate, bool dateTimeStep, bool timeWindow, int numDays);

            void stopAnimation();

            void viewer( CadKit::Viewer::Glue::Viewer ^viewer );

            void resize ( int h, int w );

            void                  addKeyWordList( System::String^ kwl );
          private:
            OsgTools::Render::Viewer *_viewer;
            ::Minerva::Scene::SceneManager *_sceneManager;
	        };
        }
      }
    }
  }
}
