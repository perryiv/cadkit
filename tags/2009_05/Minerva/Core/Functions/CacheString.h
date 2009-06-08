
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions for building strings for cache filenames.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_FUNCTIONS_CACHE_STRING_H__
#define __MINERVA_CORE_FUNCTIONS_CACHE_STRING_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Extents.h"

#include "osg/Vec2d"

#include <string>

namespace Minerva {
namespace Core {
namespace Functions {
  
  typedef Minerva::Core::Extents<osg::Vec2d> Extents;

  MINERVA_EXPORT std::string makeDirectoryString ( const std::string &cacheDir, unsigned int width, unsigned int height, unsigned int level );
  MINERVA_EXPORT std::string makeExtentsString ( const Extents& extents );
  MINERVA_EXPORT std::string makeLevelString ( unsigned int level );

  

}
}
}


#endif // __MINERVA_CORE_FUNCTIONS_CACHE_STRING_H__
