
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Functions/SearchDirectory.h"
#include "Minerva/Core/Factory/Readers.h"
#include "Minerva/Core/Functions/ReadFile.h"
#include "Minerva/Core/Layers/RasterGroup.h"

#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Split.h"

#include "boost/filesystem/operations.hpp"
#include "Usul/File/Boost.h"

#include <set>


///////////////////////////////////////////////////////////////////////////////
//
//  Typedef.
//
///////////////////////////////////////////////////////////////////////////////

typedef std::set<std::string> Extensions;

///////////////////////////////////////////////////////////////////////////////
//
//  Search directory implementation.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  void searchDirectoryImpl ( Minerva::Core::Layers::RasterGroup& group, const std::string directory, bool showFoundLayers, const Extensions& extensions )
  {
    typedef boost::filesystem::directory_iterator Iterator;
  
    Iterator iter ( directory );
    Iterator end;
    for( ; iter != end; ++iter )
    {
      const boost::filesystem::path &path = BOOST_FILE_SYSTEM_ITERATOR_TO_PATH ( iter );
      const std::string name ( path.native_directory_string() );
      
      // Make a recursive call if its a directory.
      if ( boost::filesystem::is_directory ( BOOST_FILE_SYSTEM_ITERATOR_TO_STATUS ( iter ) ) )
      {
        Minerva::Core::Layers::RasterGroup::RefPtr subGroup ( new Minerva::Core::Layers::RasterGroup );
        subGroup->showLayer ( showFoundLayers );
        subGroup->name ( name );
        
        // Search this directory.
        searchDirectoryImpl ( *subGroup, name, showFoundLayers, extensions );
        
        // Add if we found files that we could load.
        if ( subGroup->size() > 0 )
          group.append ( subGroup.get() );
      }
      
      // Add it to our list if its a file and the extenstion matches.
      else if ( extensions.end() != extensions.find ( Usul::File::extension ( name ) ) )
      {
        const std::string extension ( Usul::File::extension ( name ) );
        const std::string base ( Usul::File::base ( name ) );
        
        /// Hack so ArcAscii grids don't get loaded more than once.
        if ( "adf" == extension && "w001001" != base )
          continue;
        
        try
        {
          // Read the file.
          Usul::Interfaces::IUnknown::QueryPtr unknown ( Minerva::Core::Functions::readFile ( name ) );
          
          // Set the name.
          Usul::Interfaces::ITreeNode::QueryPtr tn ( unknown );
          if ( tn.valid() )
            tn->setTreeNodeName ( base + "." + extension );
          
          // Hide by default.
          Usul::Interfaces::IBooleanState::QueryPtr state ( unknown );
          if ( state.valid() )
            state->setBooleanState ( showFoundLayers );
          
          // Add the unknown to the group.
          group.append ( Usul::Interfaces::IRasterLayer::QueryPtr ( unknown ) );
        }
        USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "2020505170" );
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Search a directory (and all sub-directories) and add all readable files to group.
//
///////////////////////////////////////////////////////////////////////////////

void Minerva::Core::Functions::searchDirectory ( Minerva::Core::Layers::RasterGroup& group, const std::string directory, bool showFoundLayers )
{
  typedef Minerva::Core::Factory::Readers::Filters Filters;
  typedef std::vector<std::string>     Strings;
  
  Filters filters ( Minerva::Core::Factory::Readers::instance().filters() );
  
  Extensions extensions;
  
  // Find all the extensions that we can load.
  for ( Filters::const_iterator iter = filters.begin(); iter != filters.end(); ++iter )
  {
    const std::string extension ( iter->second );
    
    Strings strings;
    Usul::Strings::split ( extension, ",", false, strings );
    for ( Strings::const_iterator iter = strings.begin(); iter != strings.end(); ++iter )
      extensions.insert ( Usul::File::extension ( *iter ) );
  }
  
  // Redirect.
  Detail::searchDirectoryImpl ( group, directory, showFoundLayers, extensions );
}
