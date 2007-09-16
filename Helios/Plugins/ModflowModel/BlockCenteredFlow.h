
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Block-centered flow reader.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_BLOCK_CENTERED_FLOW_READER_CLASS_H_
#define _MODFLOW_MODEL_BLOCK_CENTERED_FLOW_READER_CLASS_H_

#include "BaseReader.h"


class BlockCenteredFlow : public BaseReader
{
public:

  // Useful typedefs.
  typedef BaseReader BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BlockCenteredFlow );

  // Construction.
  BlockCenteredFlow();

  // Read the file.
  virtual void            read ( ModflowDocument *doc, const std::string &file, Unknown *progress );

protected:

  // Use reference counting.
  virtual ~BlockCenteredFlow();

private:

  // Do not copy.
  BlockCenteredFlow ( const BlockCenteredFlow & );
  BlockCenteredFlow &operator = ( const BlockCenteredFlow & );
};


#endif // _MODFLOW_MODEL_BLOCK_CENTERED_FLOW_READER_CLASS_H_
