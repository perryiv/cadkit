
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A write-only, sharable log file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_LOG_CLASS_H_
#define _USUL_FILE_LOG_CLASS_H_

#include "Usul/Base/Object.h"

#include "Usul/Interfaces/ILog.h"

#include <fstream>
#include <string>


namespace Usul {
namespace File {


class USUL_EXPORT Log : public Usul::Base::Object,
                        public Usul::Interfaces::ILog
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( Log );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction.
  Log ( const std::string &file, bool lazyOpen );

  // The file name.
  std::string               file() const;

  // Write the string.
  virtual void              write ( const std::string &s, bool appendNewLine = true, bool prependEventCount = true );

protected:

  // Use reference counting.
  virtual ~Log();

private:

  // No copying or assignment.
  Log ( const Log & );
  Log &operator = ( const Log & );

  void                      _destroy();

  void                      _open();

  std::string _file;
  std::ofstream _out;
  unsigned long _count;
};


} // namespace Base
} // namespace Usul


#endif // _USUL_FILE_LOG_CLASS_H_
