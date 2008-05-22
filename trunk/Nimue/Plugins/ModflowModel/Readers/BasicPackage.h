
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
//  Basic package file reader.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_BASIC_PACKAGE_READER_CLASS_H_
#define _MODFLOW_MODEL_BASIC_PACKAGE_READER_CLASS_H_

#include "ModflowModel/Readers/BaseReader.h"


namespace Modflow {
namespace Readers {


class BasicPackage : public BaseReader
{
public:

  // Useful typedefs.
  typedef BaseReader BaseClass;

  // Type information.
  USUL_DECLARE_TYPE_ID ( BasicPackage );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BasicPackage );

  // Construction.
  BasicPackage();

  // Read the file.
  virtual void            read ( ModflowDocument *doc, const std::string &file, Unknown *progress );

protected:

  // Use reference counting.
  virtual ~BasicPackage();

private:

  // Do not copy.
  BasicPackage ( const BasicPackage & );
  BasicPackage &operator = ( const BasicPackage & );
};


} // namespace Readers
} // namespace Modflow


#endif // _MODFLOW_MODEL_BASIC_PACKAGE_READER_CLASS_H_
