
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helios program class. Handles startup, shutdown, and branding.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_CORE_PROGRAM_H_
#define _CADKIT_HELIOS_QT_CORE_PROGRAM_H_

#include "Helios/Qt/Core/Export.h"

#include "Usul/Threads/Manager.h"


namespace CadKit {
namespace Helios {
namespace Core {


struct HELIOS_QT_CORE_EXPORT Program
{
  static void run ( int argc, char **argv,
                    Usul::Threads::Manager::FactoryFunction *threadFactory,
                    const std::string &program, 
                    const std::string &version,
                    const std::string &vendor, 
                    const std::string &url, 
                    const std::string &icon, 
                    const std::string &splash,
                    const std::string &plugins,
                    const std::string &manual,
                    unsigned int poolSize,
                    int &result );
};


} // namespace Core
} // namespace Helios
} // namespace CadKit


#endif // _CADKIT_HELIOS_QT_CORE_PROGRAM_H_
