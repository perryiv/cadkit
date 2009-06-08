
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_DATA_SOURCES_RESULT_H__
#define __MINERVA_DATA_SOURCES_RESULT_H__

#include "Minerva/DataSources/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <string>

namespace Minerva {
namespace DataSources {

class BinaryString;
  
class MINERVA_DATA_SOURCES_EXPORT Result : public Usul::Base::Referenced
{
public:

  typedef Usul::Base::Referenced BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( Result );

  Result();
  
  /// Get the column name for the given column index.
  virtual std::string columnName ( unsigned int which ) const = 0;
  
  /// Is the result empty?
  bool empty() const;

  /// Get the number of rows in the result.
  virtual unsigned int numRows() const = 0;

  /// Get the number of columns.
  virtual unsigned int numColumns() const = 0;

  /// Prepare the next row.
  virtual bool prepareNextRow() = 0;
  
  virtual bool isNull ( const std::string& columnName ) const = 0;
  virtual bool isNull ( unsigned int which ) const = 0;
  
  virtual std::string asString ( const std::string& columnName ) const = 0;
  virtual std::string asString ( unsigned int which ) const = 0;
  
  virtual bool asBool ( const std::string& columnName ) const = 0;
  virtual bool asBool ( unsigned int which ) const = 0;
  
  virtual unsigned int asUInt ( const std::string& columnName ) const = 0;
  virtual unsigned int asUInt ( unsigned int which ) const = 0;
  
  virtual int asInt ( const std::string& columnName ) const = 0;
  virtual int asInt ( unsigned int which ) const = 0;
  
  virtual float asFloat ( const std::string& columnName ) const = 0;
  virtual float asFloat ( unsigned int which ) const = 0;
  
  virtual double asDouble ( const std::string& columnName ) const = 0;
  virtual double asDouble ( unsigned int which ) const = 0;

  virtual BinaryString* asBlob ( const std::string& columnName ) const = 0;
  virtual BinaryString* asBlob ( unsigned int which ) const = 0;
  
protected:
  
  virtual ~Result();

};

}
}

#endif // __MINERVA_DATA_SOURCES_RESULT_H__
