
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

class ossimPlanet;
class ossimPlanetTextureLayerGroup;
class ossimPlanetTextureLayer;

#include <map>

using namespace System;

namespace CadKit
{
  namespace Plugins
  {
    namespace Documents
    {
      namespace OSSIMPlanetDocument
      {
        namespace Glue
        {
	        public ref class OssimPlanet
	        {
          public:

            enum class TextureLayerStateCode
            {
               VALID          = 0,
               NO_SOURCE_DATA = 1,
               NO_GEOM        = 2,
               NO_OVERVIEWS   = 4,
               ALL = ( NO_SOURCE_DATA|
                       NO_GEOM|
                       NO_OVERVIEWS )
            };

            OssimPlanet();
            OssimPlanet( System::String ^ kwl );
            ~OssimPlanet();
            !OssimPlanet();

            CadKit::OSG::Glue::Node ^ root();

            CadKit::OSG::Glue::DatabasePager ^ databasePager();

            CadKit::OSG::Glue::MatrixManipulator ^ matrixManipulator();

            void                  addKeyWordList( System::String^ kwl );

            TextureLayerStateCode addImageLayer( System::String^ string );
            void                  hideImageLayer ( System::String^ string );
            void                  showImageLayer ( System::String^ string );

          protected:
            void _init();

          private:
            typedef std::map< std::string, ossimPlanetTextureLayer* > TextureLayers;
            ossimPlanet* _planet;
            ossimPlanetTextureLayerGroup* _textureLayerGroup;
            TextureLayers *_textureLayers;
	        };
        }
      }
    }
  }
}
