
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_DATA_SOURCES_CONNECTION_H__
#define __MINERVA_DATA_SOURCES_CONNECTION_H__

#include "Minerva/DataSources/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <string>
#include <vector>

namespace Minerva {
namespace DataSources {

class Result;

class MINERVA_DATA_SOURCES_EXPORT Connection : public Usul::Base::Referenced
{
public:
  
  typedef Usul::Base::Referenced BaseClass;
  typedef std::pair<std::string,std::string> StringPair;
  typedef std::vector <StringPair> Values;
  
  USUL_DECLARE_REF_POINTERS ( Connection );

  Connection();
  
  virtual Result* executeQuery ( const std::string& query ) const = 0;
  
  /// Execute insert query.
  void executeInsertQuery ( const std::string& tableName, const Values& values );

protected:
  
  virtual ~Connection();
  
};

}
}

#endif // __MINERVA_DATA_SOURCES_CONNECTION_H__
