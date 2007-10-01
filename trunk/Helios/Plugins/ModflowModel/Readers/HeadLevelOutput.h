
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Head-level output reader.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_HEAD_LEVEL_OUTPUT_READER_CLASS_H_
#define _MODFLOW_MODEL_HEAD_LEVEL_OUTPUT_READER_CLASS_H_

#include "Helios/Plugins/ModflowModel/Readers/BaseReader.h"


namespace Modflow {
namespace Readers {


class HeadLevelOutput : public BaseReader
{
public:

  // Useful typedefs.
  typedef BaseReader BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( HeadLevelOutput );

  // Construction.
  HeadLevelOutput();

  // Read the file.
  virtual void            read ( ModflowDocument *doc, const std::string &file, Unknown *progress );

protected:

  // Use reference counting.
  virtual ~HeadLevelOutput();

private:

  // Do not copy.
  HeadLevelOutput ( const HeadLevelOutput & );
  HeadLevelOutput &operator = ( const HeadLevelOutput & );
};


} // namespace Readers
} // namespace Modflow


#endif // _MODFLOW_MODEL_HEAD_LEVEL_OUTPUT_READER_CLASS_H_
