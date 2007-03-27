
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to look for an option.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_HAS_OPTION_H_
#define _OSG_TOOLS_HAS_OPTION_H_

#include "Usul/Strings/Case.h"


namespace OsgTools {
    namespace Options {
        
        
        ///////////////////////////////////////////////////////////////////////////////
        //
        //  See if the option exist in the map.
        //
        ///////////////////////////////////////////////////////////////////////////////
        
        template < class ContainerType > 
        bool has ( const ContainerType &container, const std::string &category, const std::string &option )
    {
            // Convert option to upper-case.
            const std::string upper ( Usul::Strings::upperCase ( option ) );
            
            // Try with given category.
            {
                typename ContainerType::const_iterator i ( container.find ( category ) );
                if ( ( container.end() != i ) && ( Usul::Strings::upperCase ( i->second ) == upper ) )
                    return true;
            }
            
            // Try with upper-case category.
            {
                typename ContainerType::const_iterator i ( container.find ( Usul::Strings::upperCase ( category ) ) );
                if ( ( container.end() != i ) && ( Usul::Strings::upperCase ( i->second ) == upper ) )
                    return true;
            }
            
            // Try with lower-case category.
            {
                typename ContainerType::const_iterator i ( container.find ( Usul::Strings::lowerCase ( category ) ) );
                if ( ( container.end() != i ) && ( Usul::Strings::upperCase ( i->second ) == upper ) )
                    return true;
            }
            
            // If we get to here then we did not find the option.
            return false;
    }
        
        
    }; // namespace Options
}; // namespace OsgTools


#endif // _OSG_TOOLS_HAS_OPTION_H_
