
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __READER_WRITER_PDB_ENUMERATIONS_H__
#define __READER_WRITER_PDB_ENUMERATIONS_H__


namespace PDB
{
  enum Flags
  {
    LOAD_ATOMS    = 0x00000001,
    LOAD_BONDS    = 0x00000002,
    SHOW_ATOMS    = 0x00000004,
    SHOW_BONDS    = 0x00000008,
  };
};


#endif // __READER_WRITER_PDB_ENUMERATIONS_H__
