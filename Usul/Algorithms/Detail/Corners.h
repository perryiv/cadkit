
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_ALGORITHMS_DETAIL_CORNERS_H__
#define __USUL_ALGORITHMS_DETAIL_CORNERS_H__


namespace Usul {
namespace Algorithms {

  namespace Detail
  {
    struct Corners
    {
      enum Vertices
      {
        V0 = 0x01,
        V1 = 0x02,
        V2 = 0x04,
        V3 = 0x08,
        V4 = 0x10,
        V5 = 0x20,
        V6 = 0x40,
        V7 = 0x80
      };
    };
  }

}
}

#endif //__USUL_ALGORITHMS_DETAIL_CORNERS_H__ 