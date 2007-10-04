
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_ALGORITHMS_DETAIL_ROTATIONS_H__
#define __USUL_ALGORITHMS_DETAIL_ROTATIONS_H__

#include "Usul/Algorithms/Detail/Corners.h"

#include <map>
#include <vector>

namespace Usul {
namespace Algorithms {

  namespace Detail
  {
    class Rotations
    {
    public:
      typedef std::map < Corners::Vertices, Corners::Vertices > RotationMap;
      typedef std::vector < RotationMap > Table;

      Rotations () : _table ()
      {
        // Reserve enough room.
        _table.reserve ( 24 );

        // Mapping 0.
        {
          RotationMap m0;
          m0[ Corners::V7 ] = Corners::V7;
          m0[ Corners::V6 ] = Corners::V6;
          m0[ Corners::V5 ] = Corners::V5;
          m0[ Corners::V4 ] = Corners::V4;
          m0[ Corners::V3 ] = Corners::V3;
          m0[ Corners::V2 ] = Corners::V2;
          m0[ Corners::V1 ] = Corners::V1;
          m0[ Corners::V0 ] = Corners::V0;
          _table.push_back ( m0 );
        }

        // Mapping 1.
        {
          RotationMap m1;
          m1[ Corners::V7 ] = Corners::V3;
          m1[ Corners::V6 ] = Corners::V2;
          m1[ Corners::V5 ] = Corners::V7;
          m1[ Corners::V4 ] = Corners::V6;
          m1[ Corners::V3 ] = Corners::V1;
          m1[ Corners::V2 ] = Corners::V0;
          m1[ Corners::V1 ] = Corners::V5;
          m1[ Corners::V0 ] = Corners::V4;
          _table.push_back ( m1 );
        }

        // Mapping 2.
        {
          RotationMap m2;
          m2[ Corners::V7 ] = Corners::V1;
          m2[ Corners::V6 ] = Corners::V0;
          m2[ Corners::V5 ] = Corners::V3;
          m2[ Corners::V4 ] = Corners::V2;
          m2[ Corners::V3 ] = Corners::V5;
          m2[ Corners::V2 ] = Corners::V4;
          m2[ Corners::V1 ] = Corners::V7;
          m2[ Corners::V0 ] = Corners::V6;
          _table.push_back ( m2 );
        }

        // Mapping 3.
        {
          RotationMap m3;
          m3[ Corners::V7 ] = Corners::V5;
          m3[ Corners::V6 ] = Corners::V4;
          m3[ Corners::V5 ] = Corners::V1;
          m3[ Corners::V4 ] = Corners::V0;
          m3[ Corners::V3 ] = Corners::V7;
          m3[ Corners::V2 ] = Corners::V6;
          m3[ Corners::V1 ] = Corners::V3;
          m3[ Corners::V0 ] = Corners::V2;
          _table.push_back ( m3 );
        }

        // Mapping 4.
        {
          RotationMap m4;
          m4[ Corners::V7 ] = Corners::V6;
          m4[ Corners::V6 ] = Corners::V2;
          m4[ Corners::V5 ] = Corners::V4;
          m4[ Corners::V4 ] = Corners::V0;
          m4[ Corners::V3 ] = Corners::V7;
          m4[ Corners::V2 ] = Corners::V3;
          m4[ Corners::V1 ] = Corners::V5;
          m4[ Corners::V0 ] = Corners::V1;
          _table.push_back ( m4 );
        }

        // Mapping 5.
        {
          RotationMap m5;
          m5[ Corners::V7 ] = Corners::V2;
          m5[ Corners::V6 ] = Corners::V3;
          m5[ Corners::V5 ] = Corners::V0;
          m5[ Corners::V4 ] = Corners::V1;
          m5[ Corners::V3 ] = Corners::V6;
          m5[ Corners::V2 ] = Corners::V7;
          m5[ Corners::V1 ] = Corners::V4;
          m5[ Corners::V0 ] = Corners::V5;
          _table.push_back ( m5 );
        }

        // Mapping 6.
        {
          RotationMap m6;
          m6[ Corners::V7 ] = Corners::V3;
          m6[ Corners::V6 ] = Corners::V7;
          m6[ Corners::V5 ] = Corners::V1;
          m6[ Corners::V4 ] = Corners::V5;
          m6[ Corners::V3 ] = Corners::V2;
          m6[ Corners::V2 ] = Corners::V6;
          m6[ Corners::V1 ] = Corners::V0;
          m6[ Corners::V0 ] = Corners::V4;
          _table.push_back ( m6 );
        }

        // Mapping 7.
        {
          RotationMap m7;
          m7[ Corners::V7 ] = Corners::V5;
          m7[ Corners::V6 ] = Corners::V7;
          m7[ Corners::V5 ] = Corners::V4;
          m7[ Corners::V4 ] = Corners::V6;
          m7[ Corners::V3 ] = Corners::V1;
          m7[ Corners::V2 ] = Corners::V3;
          m7[ Corners::V1 ] = Corners::V0;
          m7[ Corners::V0 ] = Corners::V2;
          _table.push_back ( m7 );
        }

        // Mapping 8.
        {
          RotationMap m8;
          m8[ Corners::V7 ] = Corners::V4;
          m8[ Corners::V6 ] = Corners::V5;
          m8[ Corners::V5 ] = Corners::V6;
          m8[ Corners::V4 ] = Corners::V7;
          m8[ Corners::V3 ] = Corners::V0;
          m8[ Corners::V2 ] = Corners::V1;
          m8[ Corners::V1 ] = Corners::V2;
          m8[ Corners::V0 ] = Corners::V3;
          _table.push_back ( m8 );
        }

        // Mapping 9.
        {
          RotationMap m9;
          m9[ Corners::V7 ] = Corners::V6;
          m9[ Corners::V6 ] = Corners::V4;
          m9[ Corners::V5 ] = Corners::V7;
          m9[ Corners::V4 ] = Corners::V5;
          m9[ Corners::V3 ] = Corners::V2;
          m9[ Corners::V2 ] = Corners::V0;
          m9[ Corners::V1 ] = Corners::V3;
          m9[ Corners::V0 ] = Corners::V1;
          _table.push_back ( m9 );
        }

        // Mapping 10.
        {
          RotationMap m10;
          m10[ Corners::V7 ] = Corners::V2;
          m10[ Corners::V6 ] = Corners::V0;
          m10[ Corners::V5 ] = Corners::V6;
          m10[ Corners::V4 ] = Corners::V4;
          m10[ Corners::V3 ] = Corners::V3;
          m10[ Corners::V2 ] = Corners::V1;
          m10[ Corners::V1 ] = Corners::V7;
          m10[ Corners::V0 ] = Corners::V5;
          _table.push_back ( m10 );
        }

        // Mapping 11.
        {
          RotationMap m11;
          m11[ Corners::V7 ] = Corners::V0;
          m11[ Corners::V6 ] = Corners::V1;
          m11[ Corners::V5 ] = Corners::V4;
          m11[ Corners::V4 ] = Corners::V5;
          m11[ Corners::V3 ] = Corners::V2;
          m11[ Corners::V2 ] = Corners::V3;
          m11[ Corners::V1 ] = Corners::V6;
          m11[ Corners::V0 ] = Corners::V7;
          _table.push_back ( m11 );
        }

        // Mapping 12.
        {
          RotationMap m12;
          m12[ Corners::V7 ] = Corners::V1;
          m12[ Corners::V6 ] = Corners::V3;
          m12[ Corners::V5 ] = Corners::V5;
          m12[ Corners::V4 ] = Corners::V7;
          m12[ Corners::V3 ] = Corners::V0;
          m12[ Corners::V2 ] = Corners::V2;
          m12[ Corners::V1 ] = Corners::V4;
          m12[ Corners::V0 ] = Corners::V6;
          _table.push_back ( m12 );
        }

        // Mapping 13.
        {
          RotationMap m13;
          m13[ Corners::V7 ] = Corners::V0;
          m13[ Corners::V6 ] = Corners::V4;
          m13[ Corners::V5 ] = Corners::V2;
          m13[ Corners::V4 ] = Corners::V6;
          m13[ Corners::V3 ] = Corners::V1;
          m13[ Corners::V2 ] = Corners::V5;
          m13[ Corners::V1 ] = Corners::V3;
          m13[ Corners::V0 ] = Corners::V7;
          _table.push_back ( m13 );
        }

        // Mapping 14.
        {
          RotationMap m14;
          m14[ Corners::V7 ] = Corners::V5;
          m14[ Corners::V6 ] = Corners::V1;
          m14[ Corners::V5 ] = Corners::V7;
          m14[ Corners::V4 ] = Corners::V3;
          m14[ Corners::V3 ] = Corners::V4;
          m14[ Corners::V2 ] = Corners::V0;
          m14[ Corners::V1 ] = Corners::V6;
          m14[ Corners::V0 ] = Corners::V2;
          _table.push_back ( m14 );
        }

        // Mapping 15.
        {
          RotationMap m15;
          m15[ Corners::V7 ] = Corners::V4;
          m15[ Corners::V6 ] = Corners::V6;
          m15[ Corners::V5 ] = Corners::V0;
          m15[ Corners::V4 ] = Corners::V2;
          m15[ Corners::V3 ] = Corners::V5;
          m15[ Corners::V2 ] = Corners::V7;
          m15[ Corners::V1 ] = Corners::V1;
          m15[ Corners::V0 ] = Corners::V3;
          _table.push_back ( m15 );
        }

        // Mapping 16.
        {
          RotationMap m16;
          m16[ Corners::V7 ] = Corners::V6;
          m16[ Corners::V6 ] = Corners::V7;
          m16[ Corners::V5 ] = Corners::V2;
          m16[ Corners::V4 ] = Corners::V3;
          m16[ Corners::V3 ] = Corners::V4;
          m16[ Corners::V2 ] = Corners::V5;
          m16[ Corners::V1 ] = Corners::V0;
          m16[ Corners::V0 ] = Corners::V1;
          _table.push_back ( m16 );
        }

        // Mapping 17.
        {
          RotationMap m17;
          m17[ Corners::V7 ] = Corners::V7;
          m17[ Corners::V6 ] = Corners::V5;
          m17[ Corners::V5 ] = Corners::V3;
          m17[ Corners::V4 ] = Corners::V1;
          m17[ Corners::V3 ] = Corners::V6;
          m17[ Corners::V2 ] = Corners::V4;
          m17[ Corners::V1 ] = Corners::V2;
          m17[ Corners::V0 ] = Corners::V0;
          _table.push_back ( m17 );
        }

        // Mapping 18.
        {
          RotationMap m18;
          m18[ Corners::V7 ] = Corners::V7;
          m18[ Corners::V6 ] = Corners::V3;
          m18[ Corners::V5 ] = Corners::V6;
          m18[ Corners::V4 ] = Corners::V2;
          m18[ Corners::V3 ] = Corners::V5;
          m18[ Corners::V2 ] = Corners::V1;
          m18[ Corners::V1 ] = Corners::V4;
          m18[ Corners::V0 ] = Corners::V0;
          _table.push_back ( m18 );
        }

        // Mapping 19.
        {
          RotationMap m19;
          m19[ Corners::V7 ] = Corners::V3;
          m19[ Corners::V6 ] = Corners::V1;
          m19[ Corners::V5 ] = Corners::V2;
          m19[ Corners::V4 ] = Corners::V0;
          m19[ Corners::V3 ] = Corners::V7;
          m19[ Corners::V2 ] = Corners::V5;
          m19[ Corners::V1 ] = Corners::V6;
          m19[ Corners::V0 ] = Corners::V4;
          _table.push_back ( m19 );
        }

        // Mapping 20.
        {
          RotationMap m20;
          m20[ Corners::V7 ] = Corners::V1;
          m20[ Corners::V6 ] = Corners::V5;
          m20[ Corners::V5 ] = Corners::V0;
          m20[ Corners::V4 ] = Corners::V4;
          m20[ Corners::V3 ] = Corners::V3;
          m20[ Corners::V2 ] = Corners::V7;
          m20[ Corners::V1 ] = Corners::V2;
          m20[ Corners::V0 ] = Corners::V6;
          _table.push_back ( m20 );
        }

        // Mapping 21.
        {
          RotationMap m21;
          m21[ Corners::V7 ] = Corners::V4;
          m21[ Corners::V6 ] = Corners::V0;
          m21[ Corners::V5 ] = Corners::V5;
          m21[ Corners::V4 ] = Corners::V1;
          m21[ Corners::V3 ] = Corners::V6;
          m21[ Corners::V2 ] = Corners::V2;
          m21[ Corners::V1 ] = Corners::V7;
          m21[ Corners::V0 ] = Corners::V3;
          _table.push_back ( m21 );
        }

        // Mapping 22.
        {
          RotationMap m22;
          m22[ Corners::V7 ] = Corners::V0;
          m22[ Corners::V6 ] = Corners::V2;
          m22[ Corners::V5 ] = Corners::V1;
          m22[ Corners::V4 ] = Corners::V3;
          m22[ Corners::V3 ] = Corners::V4;
          m22[ Corners::V2 ] = Corners::V6;
          m22[ Corners::V1 ] = Corners::V5;
          m22[ Corners::V0 ] = Corners::V7;
          _table.push_back ( m22 );
        }

        // Mapping 23.
        {
          RotationMap m23;
          m23[ Corners::V7 ] = Corners::V2;
          m23[ Corners::V6 ] = Corners::V6;
          m23[ Corners::V5 ] = Corners::V3;
          m23[ Corners::V4 ] = Corners::V7;
          m23[ Corners::V3 ] = Corners::V0;
          m23[ Corners::V2 ] = Corners::V4;
          m23[ Corners::V1 ] = Corners::V1;
          m23[ Corners::V0 ] = Corners::V5;
          _table.push_back ( m23 );
        }
      }

      const RotationMap& operator () ( unsigned int index ) const
      {
        return _table [ index ];
      }

    private:
      Table _table;
    };
  }
}
}

#endif //__USUL_ALGORITHMS_DETAIL_ROTATIONS_H__