
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  This class is not implemented yet.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_POSTGIS_TEXT_PARSER_H__
#define __MINERVA_POSTGIS_TEXT_PARSER_H__

#include "Minerva/Core/Export.h"

namespace Minerva {
namespace Core {
  
  namespace Geometry { class Geometry; }
  
namespace postGIS {

class MINERVA_EXPORT TextParser
{
public:
  typedef Minerva::Core::Geometry::Geometry Geometry;
  TextParser();
  ~TextParser();

  Geometry*     operator() ( const char * buffer );
};

}
}
}

#endif // __MINERVA_POSTGIS_TEXT_PARSER_H__