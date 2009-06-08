
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_DATA_SOURCES_PG_RESULT_H__
#define __MINERVA_DATA_SOURCES_PG_RESULT_H__

#include "Minerva/DataSources/PG/Export.h"

#include "Minerva/DataSources/Result.h"

#include "Usul/Base/Referenced.h"

#include <string>

typedef struct pg_result PGresult;

namespace Minerva {
namespace DataSources {
namespace PG {

class MINERVA_POSTGRES_EXPORT Result : public Minerva::DataSources::Result
{
public:
  
  typedef Minerva::DataSources::Result BaseClass;
  typedef Minerva::DataSources::BinaryString BinaryString;
  
  USUL_DECLARE_REF_POINTERS ( Result );
  
  Result ( PGresult * );
  
  /// Get the column name for the given column index.
  virtual std::string columnName ( unsigned int which ) const;
  
  /// Get the column index for the given name.
  unsigned int columnIndex ( const std::string& columnName ) const;
  
  /// Get the number of rows in the result.
  virtual unsigned int numRows() const;
  
  /// Get the number of columns.
  virtual unsigned int numColumns() const;

  /// Prepare the next row.
  virtual bool prepareNextRow();
  
  virtual bool isNull ( const std::string& columnName ) const;
  virtual bool isNull ( unsigned int which ) const;
  
  virtual std::string asString ( const std::string& columnName ) const;
  virtual std::string asString ( unsigned int which ) const;

  virtual bool asBool ( const std::string& columnName ) const;
  virtual bool asBool ( unsigned int which ) const;
  
  virtual unsigned int asUInt ( const std::string& columnName ) const;
  virtual unsigned int asUInt ( unsigned int which ) const;
  
  virtual int asInt ( const std::string& columnName ) const;
  virtual int asInt ( unsigned int which ) const;
  
  virtual float asFloat ( const std::string& columnName ) const;
  virtual float asFloat ( unsigned int which ) const;
  
  virtual double asDouble ( const std::string& columnName ) const;
  virtual double asDouble ( unsigned int which ) const;
  
  virtual BinaryString* asBlob ( const std::string& columnName ) const;
  virtual BinaryString* asBlob ( unsigned int which ) const;
  
protected:
  
  Result();
  virtual ~Result();
  
  char* _getValue ( unsigned int row, unsigned int column ) const;
  int _getLength ( unsigned int row, unsigned int column ) const;
  bool _isNull ( unsigned int row, unsigned int column ) const;
  
private:
  
  PGresult *_result;
  int _currentRow;
  
};
  
}
}
}

#endif // __MINERVA_DATA_SOURCES_PG_RESULT_H__
