
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Auxiliary data reader.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_AUXILIARY_DATA_READER_CLASS_H_
#define _MODFLOW_MODEL_AUXILIARY_DATA_READER_CLASS_H_

#include "ModflowModel/Readers/BaseReader.h"


namespace Modflow {
namespace Readers {


class AuxiliaryData : public BaseReader
{
public:

  // Useful typedefs.
  typedef BaseReader BaseClass;

  // Type information.
  USUL_DECLARE_TYPE_ID ( AuxiliaryData );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( AuxiliaryData );

  // Construction.
  AuxiliaryData();

  // Read the file.
  virtual void            read ( ModflowDocument *doc, const std::string &file, Unknown *progress );

protected:

  // Use reference counting.
  virtual ~AuxiliaryData();

private:

  void                    _read ( ModflowDocument *doc, Unknown *progress, const std::string &file );

  // Do not copy.
  AuxiliaryData ( const AuxiliaryData & );
  AuxiliaryData &operator = ( const AuxiliaryData & );
};


} // namespace Readers
} // namespace Modflow


#endif // _MODFLOW_MODEL_AUXILIARY_DATA_READER_CLASS_H_
