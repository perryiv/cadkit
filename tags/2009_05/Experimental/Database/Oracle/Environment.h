
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Encapsulates database environment.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _ORACLE_LITE_WRAP_ENVIRONMENT_H_
#define _ORACLE_LITE_WRAP_ENVIRONMENT_H_

#include "Database/Oracle/Export.h"

namespace oracle { namespace occi { class Environment; } }


namespace CadKit {
namespace Database {
namespace Oracle {


class ORACLE_WRAP_EXPORT Environment
{
public:

  static void                     destroy();

  static Environment &            instance();

  oracle::occi::Environment *     env();

protected:

  Environment();
  ~Environment();

private:

  // Can not copy or assign.
  Environment ( const Environment & );
  Environment &operator = ( const Environment & );

  void                            _destroy();

  oracle::occi::Environment *_env;
};


} // namespace Oracle
} // namespace Database
} // namespace CadKit


#endif // _ORACLE_LITE_WRAP_ENVIRONMENT_H_
