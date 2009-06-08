
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
//  Head-level output reader.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_HEAD_LEVEL_OUTPUT_READER_CLASS_H_
#define _MODFLOW_MODEL_HEAD_LEVEL_OUTPUT_READER_CLASS_H_

#include "ModflowModel/Readers/BaseReader.h"

#include "Usul/Types/Types.h"


namespace Modflow {
namespace Readers {


class HeadLevelOutput : public BaseReader
{
public:

  // Useful typedefs.
  typedef BaseReader BaseClass;
  typedef Usul::Types::Uint64 TimeStamp;

  // Type information.
  USUL_DECLARE_TYPE_ID ( HeadLevelOutput );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( HeadLevelOutput );

  // Construction.
  HeadLevelOutput();

  // Read the file.
  virtual void            read ( ModflowDocument *doc, const std::string &file, Unknown *progress );

protected:

  // Use reference counting.
  virtual ~HeadLevelOutput();

  bool                    _getTimeStamp ( const std::string &, TimeStamp & );

private:

  // Do not copy.
  HeadLevelOutput ( const HeadLevelOutput & );
  HeadLevelOutput &operator = ( const HeadLevelOutput & );
};


} // namespace Readers
} // namespace Modflow


#endif // _MODFLOW_MODEL_HEAD_LEVEL_OUTPUT_READER_CLASS_H_
