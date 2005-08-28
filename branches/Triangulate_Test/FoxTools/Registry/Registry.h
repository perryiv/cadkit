
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Registry functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_REGISTRY_FUNCTIONS_H_
#define _FOX_TOOLS_REGISTRY_FUNCTIONS_H_

#include "FoxTools/Export/Export.h"
#include "FoxTools/Headers/Registry.h"

#include "Usul/Headers/SStream.h"

namespace FoxTools {
namespace Registry {



///////////////////////////////////////////////////////////////////////////////
//
//  Read the value.
//
///////////////////////////////////////////////////////////////////////////////




FOX_TOOLS_EXPORT std::string    read ( const std::string &section, const std::string &key, const std::string &defaultValue );
FOX_TOOLS_EXPORT unsigned int   read ( const std::string &section, const std::string &key, unsigned int       defaultValue );
FOX_TOOLS_EXPORT int            read ( const std::string &section, const std::string &key, int                defaultValue );
FOX_TOOLS_EXPORT bool           read ( const std::string &section, const std::string &key, bool               defaultValue );
FOX_TOOLS_EXPORT float          read ( const std::string &section, const std::string &key, float              defaultValue );
FOX_TOOLS_EXPORT double         read ( const std::string &section, const std::string &key, double             defaultValue );



///////////////////////////////////////////////////////////////////////////////
//
//  Write the value.
//
///////////////////////////////////////////////////////////////////////////////

FOX_TOOLS_EXPORT void           write ( const std::string &section, const std::string &key, const std::string &value );
FOX_TOOLS_EXPORT void           write ( const std::string &section, const std::string &key, unsigned int       value );
FOX_TOOLS_EXPORT void           write ( const std::string &section, const std::string &key, int                value );
FOX_TOOLS_EXPORT void           write ( const std::string &section, const std::string &key, bool               value );
FOX_TOOLS_EXPORT void           write ( const std::string &section, const std::string &key, float              value );
FOX_TOOLS_EXPORT void           write ( const std::string &section, const std::string &key, double             value );


}; // namespace Registry
}; // namespace FoxTools


#endif // _FOX_TOOLS_REGISTRY_FUNCTIONS_H_
