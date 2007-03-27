
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef OSG_TOOLS_RENDER_CONSTANTS
#define OSG_TOOLS_RENDER_CONSTANTS

#include <string>

namespace OsgTools
{
  namespace Render
  {
    namespace Constants
    {
      // Canvas' group-map.
      const std::string BOUNDING_BOX                = "bounding_box";
      const std::string BOUNDING_SPHERE             = "bounding_sphere";
      const std::string TEXT_MATRIX                 = "text_matrix";
      const std::string SELECTION_BOX               = "selection_box";
      const std::string CLIPPING_PLANES             = "cliping_planes";
      const std::string AXES                        = "axes";
      const std::string LIGHT                       = "light";
      const std::string STAGE                       = "stage";


    };
  }
}

#endif // OSG_TOOLS_RENDER_CONSTANTS

