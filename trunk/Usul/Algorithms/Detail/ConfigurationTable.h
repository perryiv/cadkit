
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_ALGORITHMS_DETAIL_CONFIGURATION_TABLE_H__
#define __USUL_ALGORITHMS_DETAIL_CONFIGURATION_TABLE_H__

#include <vector>

namespace Usul {
namespace Algorithms {
namespace Detail {


class ConfigurationTable
{
public:
  typedef std::pair < unsigned char, unsigned char > Entry;
  typedef std::vector < Entry > Entries;
  typedef std::vector < Entries > Table;

  ConfigurationTable () : _table ()
  {
    // Row 0.
    Entry r0 [ 16 ] = { Entry ( 0,  0 ), Entry ( 3,  1 ), Entry ( 5,  1 ), Entry ( 3,  2 ), 
                        Entry ( 2,  1 ), Entry ( 15, 2 ), Entry ( 2,  3 ), Entry ( 16, 5 ), 
                        Entry ( 14, 1 ), Entry ( 19, 3 ), Entry ( 17, 2 ), Entry ( 15, 5 ), 
                        Entry ( 2,  2 ), Entry ( 17, 5 ), Entry ( 3,  5 ), Entry ( 3,  8 ) };

    /// Row 1.
    Entry r1 [ 16 ] = { Entry ( 0,  1 ), Entry ( 4,  2 ), Entry ( 20, 3  ), Entry ( 6,  5 ), 
                        Entry ( 10, 3 ), Entry ( 7,  5 ), Entry ( 11, 7  ), Entry ( 0,  9 ), 
                        Entry ( 0,  4 ), Entry ( 20, 6 ), Entry ( 22, 6  ), Entry ( 0,  11 ), 
                        Entry ( 16, 6 ), Entry ( 7, 14 ), Entry ( 3,  12 ), Entry ( 10, 17 ) };

    /// Row 2.
    Entry r2 [ 16 ] = { Entry ( 6,  1 ), Entry ( 3,  3  ), Entry ( 6,  2  ), Entry ( 0,  5 ), 
                        Entry ( 2,  4 ), Entry ( 15, 6  ), Entry ( 6,  6  ), Entry ( 0,  14 ), 
                        Entry ( 6,  3 ), Entry ( 2,  7  ), Entry ( 23, 5  ), Entry ( 4,  9 ), 
                        Entry ( 2,  6 ), Entry ( 17, 12 ), Entry ( 9,  11 ), Entry ( 11, 17 ) };

    /// Row 3.
    Entry r3 [ 16 ] = { Entry ( 0,  2  ), Entry ( 5,  5  ), Entry ( 4,  5  ), Entry ( 0,  8 ), 
                        Entry ( 11, 6  ), Entry ( 6,  11 ), Entry ( 4,  12 ), Entry ( 2,  17 ), 
                        Entry ( 0,  6  ), Entry ( 5,  12 ), Entry ( 4,  14 ), Entry ( 14, 17 ), 
                        Entry ( 9,  10 ), Entry ( 1,  16 ), Entry ( 8,  16 ), Entry ( 1,  20 ) };

    /// Row 4.
    Entry r4 [ 16 ] = { Entry ( 1,  1  ), Entry ( 4,  3  ), Entry ( 1,  4  ), Entry ( 5,  6 ), 
                        Entry ( 13, 2  ), Entry ( 20, 5  ), Entry ( 13, 6  ), Entry ( 7,  11 ), 
                        Entry ( 1,  3  ), Entry ( 5,  7  ), Entry ( 17, 6  ), Entry ( 15, 12 ), 
                        Entry ( 8,  5  ), Entry ( 3,  9  ), Entry ( 3,  14 ), Entry ( 1,  17 ) };

    /// Row 5.
    Entry r5 [ 16 ] = { Entry ( 7,  2  ), Entry ( 18, 5  ), Entry ( 7,  6  ), Entry ( 6,  14 ), 
                        Entry ( 19, 5  ), Entry ( 7,  8  ), Entry ( 19, 12 ), Entry ( 21, 17 ), 
                        Entry ( 10, 6  ), Entry ( 18, 12 ), Entry ( 0,  10 ), Entry ( 9,  16 ), 
                        Entry ( 14, 11 ), Entry ( 22, 17 ), Entry ( 12, 16 ), Entry ( 9,  20 ) };

    /// Row 6.
    Entry r6 [ 16 ] = { Entry ( 7,  3  ), Entry ( 8,  7  ), Entry ( 21, 6  ), Entry ( 0,  12 ), 
                        Entry ( 18, 6  ), Entry ( 20, 12 ), Entry ( 1,  10 ), Entry ( 14, 16 ), 
                        Entry ( 0,  7  ), Entry ( 1,  13 ), Entry ( 23, 12 ), Entry ( 4,  15 ), 
                        Entry ( 8,  12 ), Entry ( 3,  15 ), Entry ( 4,  16 ), Entry ( 0,  19 ) };

    /// Row 7.
    Entry r7 [ 16 ] = { Entry ( 12, 5  ), Entry ( 1,  9  ), Entry ( 1,  11 ), Entry ( 13, 17 ), 
                        Entry ( 12, 14 ), Entry ( 9,  17 ), Entry ( 23, 16 ), Entry ( 14, 20 ), 
                        Entry ( 12, 12 ), Entry ( 1,  15 ), Entry ( 19, 16 ), Entry ( 18, 19 ), 
                        Entry ( 3,  16 ), Entry ( 12, 19 ), Entry ( 3,  18 ), Entry ( 8,  21 ) };

    /// Row 8.
    Entry r8 [ 16 ] = { Entry ( 8,  1  ), Entry ( 3,  4  ), Entry ( 12, 3  ), Entry ( 3,  6  ), 
                        Entry ( 18, 3  ), Entry ( 19, 6  ), Entry ( 1,  7  ), Entry ( 16, 12 ), 
                        Entry ( 14, 2  ), Entry ( 23, 6  ), Entry ( 9,  5  ), Entry ( 9,  14 ), 
                        Entry ( 13, 5  ), Entry ( 3,  11 ), Entry ( 8,  9  ), Entry ( 12, 17 ) };

    /// Row 9.
    Entry r9 [ 16 ] = { Entry ( 0,  3  ), Entry ( 4,  6  ), Entry ( 3,  7  ), Entry ( 6,  12 ), 
                        Entry ( 4,  7  ), Entry ( 7,  12 ), Entry ( 0,  13 ), Entry ( 0,  15 ), 
                        Entry ( 14, 6  ), Entry ( 3,  10 ), Entry ( 9,  12 ), Entry ( 18, 16 ), 
                        Entry ( 13, 12 ), Entry ( 21, 16 ), Entry ( 8,  15 ), Entry ( 7,  19 ) };

    /// Row 10.
    Entry r10 [ 16 ] = { Entry ( 9,  2  ), Entry ( 12, 6  ), Entry ( 22, 5  ), Entry ( 4,  11 ), 
                         Entry ( 9,  6  ), Entry ( 2,  10 ), Entry ( 22, 12 ), Entry ( 10, 16 ), 
                         Entry ( 21, 5  ), Entry ( 21, 12 ), Entry ( 9,  8  ), Entry ( 19, 17 ), 
                         Entry ( 13, 14 ), Entry ( 7,  16 ), Entry ( 18, 17 ), Entry ( 7,  20 ) };

    /// Row 11.
    Entry r11 [ 16 ] = { Entry ( 1,  5  ), Entry ( 1,  14 ), Entry ( 5,  9  ), Entry ( 8,  17 ), 
                         Entry ( 1,  12 ), Entry ( 17, 16 ), Entry ( 5,  15 ), Entry ( 1,  19 ), 
                         Entry ( 10, 11 ), Entry ( 13, 16 ), Entry ( 20, 17 ), Entry ( 13, 20 ), 
                         Entry ( 5,  16 ), Entry ( 1,  18 ), Entry ( 4,  19 ), Entry ( 1,  21 ) };

    /// Row 12.
    Entry r12 [ 16 ] = { Entry ( 1,  2  ), Entry ( 8,  6  ), Entry ( 1,  6  ), Entry ( 7,  10 ), 
                         Entry ( 14, 5  ), Entry ( 14, 14 ), Entry ( 14, 12 ), Entry ( 0,  16 ), 
                         Entry ( 2,  5  ), Entry ( 2,  12 ), Entry ( 13, 11 ), Entry ( 11, 16 ), 
                         Entry ( 2,  8  ), Entry ( 4,  17 ), Entry ( 5,  17 ), Entry ( 0,  20 ) };

    /// Row 13.
    Entry r13 [ 16 ] = { Entry ( 11, 5  ), Entry ( 12, 11 ), Entry ( 11, 12 ), Entry ( 2,  16 ), 
                         Entry ( 2,  9  ), Entry ( 23, 17 ), Entry ( 2,  15 ), Entry ( 6,  19 ), 
                         Entry ( 2,  14 ), Entry ( 6,  16 ), Entry ( 15, 16 ), Entry ( 2,  18 ), 
                         Entry ( 0,  17 ), Entry ( 6,  20 ), Entry ( 3,  19 ), Entry ( 6,  21 ) };

    /// Row 14.
    Entry r14 [ 16 ] = { Entry ( 10, 5  ), Entry ( 10, 12 ), Entry ( 10, 14 ), Entry ( 16, 16 ), 
                         Entry ( 2,  11 ), Entry ( 22, 16 ), Entry ( 20, 16 ), Entry ( 0,  18 ), 
                         Entry ( 11, 9  ), Entry ( 11, 15 ), Entry ( 7,  17 ), Entry ( 10, 19 ), 
                         Entry ( 6,  17 ), Entry ( 20, 19 ), Entry ( 4,  20 ), Entry ( 0,  21 ) };

    /// Row 15.
    Entry r15 [ 16 ] = { Entry ( 1,  8  ), Entry ( 3,  17 ), Entry ( 17, 17 ), Entry ( 2,  20 ), 
                         Entry ( 15, 17 ), Entry ( 17, 20 ), Entry ( 19, 19 ), Entry ( 14, 21 ), 
                         Entry ( 16, 17 ), Entry ( 2,  19 ), Entry ( 15, 20 ), Entry ( 2,  21 ), 
                         Entry ( 3,  20 ), Entry ( 5,  21 ), Entry ( 3,  21 ), Entry ( 0,  22 ) };

    _table.push_back ( Entries ( r0, r0 + 16 ) );
    _table.push_back ( Entries ( r1, r1 + 16 ) );
    _table.push_back ( Entries ( r2, r2 + 16 ) );
    _table.push_back ( Entries ( r3, r3 + 16 ) );
    _table.push_back ( Entries ( r4, r4 + 16 ) );
    _table.push_back ( Entries ( r5, r5 + 16 ) );
    _table.push_back ( Entries ( r6, r6 + 16 ) );
    _table.push_back ( Entries ( r7, r7 + 16 ) );
    _table.push_back ( Entries ( r8, r8 + 16 ) );
    _table.push_back ( Entries ( r9, r9 + 16 ) );
    _table.push_back ( Entries ( r10, r10 + 16 ) );
    _table.push_back ( Entries ( r11, r11 + 16 ) );
    _table.push_back ( Entries ( r12, r12 + 16 ) );
    _table.push_back ( Entries ( r13, r13 + 16 ) );
    _table.push_back ( Entries ( r14, r14 + 16 ) );
    _table.push_back ( Entries ( r15, r15 + 16 ) );
  }

  Entry operator () ( unsigned char value ) const
  {
    unsigned char msb ( value & 0xF0 );
    unsigned char lsb ( value & 0x0F );

    msb = msb >> 4;

    return _table [ msb ] [ lsb ];
  }

private:
  Table _table;
};


}
}
}


#endif // __USUL_ALGORITHMS_DETAIL_CONFIGURATION_TABLE_H__
