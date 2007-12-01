
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

#ifdef __CLR_VER
#undef __CLR_VER
#endif

#include "Minerva/Document/MinervaDocument.h"

#ifndef __CLR_VER
#define __CLR_VER
#endif

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

            void                  open( System::String^ filename );
            void                  save();
            void                  saveAs( System::String^ filename );
            void                  exportFile( System::String^ filename );
		        
            void                  removeLayer ( CadKit::Interfaces::ILayer ^layer );
            void                  hideLayer   ( CadKit::Interfaces::ILayer ^layer );
            void                  showLayer   ( CadKit::Interfaces::ILayer ^layer );
            void                  addLayer    ( CadKit::Interfaces::ILayer ^layer, CadKit::Threads::Jobs::Progress ^progess );
            void                  modifyLayer ( CadKit::Interfaces::ILayer ^layer, CadKit::Threads::Jobs::Progress ^progess );

            void                  viewLayerExtents ( CadKit::Interfaces::ILayer ^layer );
      
            void                  setLayerOperation( System::String^ optype, int val, CadKit::Interfaces::ILayer^ layer );

            void                  timestepType ( CadKit::Interfaces::AnimateTimestep type );
            CadKit::Interfaces::AnimateTimestep  timestepType ();
            void                  startAnimation( float speed, bool accumulate, bool timeWindow, int numDays );
            void                  stopAnimation();
            void                  pauseAnimation();

            void                  animationSpeed ( double d );
            double                animationSpeed ();

            void                  viewer( CadKit::Viewer::Glue::Viewer ^viewer );

            bool                  showLegend();
            void                  showLegend( bool b );

            void                  percentScreenWidth ( float );
            float                 percentScreenWidth();

            void                           addToFavorites( CadKit::Interfaces::ILayer^ layer );
            CadKit::Interfaces::ILayer^    createFavorite( System::String^ name );

            property array<System::String^ > ^ Favorites
            {
              array<System::String^ > ^ get();
            }


            property array<CadKit::Interfaces::ILayer^ > ^ Layers
            {
              array<CadKit::Interfaces::ILayer^ > ^ get();
            }

            void                  setActive ( bool b );

          private:
            ::Minerva::Document::MinervaDocument *_document;
	        };
        }
      }
    }
  }
}

