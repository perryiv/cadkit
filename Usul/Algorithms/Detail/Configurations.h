
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_ALGORITHMS_DETAIL_CONFIGURATION_H__
#define __USUL_ALGORITHMS_DETAIL_CONFIGURATION_H__

#include "Usul/Algorithms/Detail/Corners.h"

#include <vector>

namespace Usul {
namespace Algorithms {

  namespace Detail
  {
    struct Triangle
    {
      Triangle ( Corners::Vertices p0, Corners::Vertices p1, Corners::Vertices p2, Corners::Vertices p3, Corners::Vertices p4, Corners::Vertices p5 ) :
      _p0 ( p0 ),
      _p1 ( p1 ),
      _p2 ( p2 ),
      _p3 ( p3 ),
      _p4 ( p4 ),
      _p5 ( p5 )
      {
      }

      Corners::Vertices _p0, _p1, _p2, _p3, _p4, _p5;
    };

    class Configurations
    {
    public:
      typedef std::vector < Triangle > Configuration;
      typedef std::vector < Configuration > ConfigurationList;

      Configurations () : _configurations ()
      {
        // C0
        _configurations.push_back ( Configuration() );

        // C1
        {
          Configuration c1;
          c1.push_back ( Triangle ( Corners::V4, Corners::V5, Corners::V4, Corners::V0, Corners::V4, Corners::V6  ) );
          _configurations.push_back ( c1 );
        }

        // C2
        {
          Configuration c2;
          c2.push_back ( Triangle ( Corners::V5, Corners::V1, Corners::V4, Corners::V0, Corners::V4, Corners::V6 ) );
          c2.push_back ( Triangle ( Corners::V5, Corners::V1, Corners::V4, Corners::V6, Corners::V5, Corners::V7 ) );
          _configurations.push_back ( c2 );
        }

        // C3
        {
          Configuration c3;
          c3.push_back ( Triangle ( Corners::V4, Corners::V5, Corners::V4, Corners::V0, Corners::V4, Corners::V6 ) );
          c3.push_back ( Triangle ( Corners::V6, Corners::V7, Corners::V7, Corners::V3, Corners::V7, Corners::V5 ) );
          _configurations.push_back ( c3 );
        }

        // C4
        {
          Configuration c4;
          c4.push_back ( Triangle ( Corners::V4, Corners::V5, Corners::V4, Corners::V0, Corners::V4, Corners::V6 ) );
          c4.push_back ( Triangle ( Corners::V1, Corners::V3, Corners::V3, Corners::V7, Corners::V3, Corners::V2 ) );
          _configurations.push_back ( c4 );
        }

        // C5
        {
          Configuration c5;
          c5.push_back ( Triangle ( Corners::V5, Corners::V7, Corners::V1, Corners::V3, Corners::V0, Corners::V2 ) );
          c5.push_back ( Triangle ( Corners::V5, Corners::V7, Corners::V0, Corners::V2, Corners::V0, Corners::V4 ) );
          c5.push_back ( Triangle ( Corners::V5, Corners::V7, Corners::V0, Corners::V4, Corners::V4, Corners::V5 ) );
          _configurations.push_back ( c5 );
        }

        // C6
        {
          Configuration c6;
          c6.push_back ( Triangle ( Corners::V5, Corners::V1, Corners::V4, Corners::V0, Corners::V4, Corners::V6 ) );
          c6.push_back ( Triangle ( Corners::V5, Corners::V1, Corners::V4, Corners::V6, Corners::V5, Corners::V7 ) );
          c6.push_back ( Triangle ( Corners::V1, Corners::V3, Corners::V3, Corners::V7, Corners::V3, Corners::V2 ) );
          _configurations.push_back ( c6 );
        }

        // C7
        {
          Configuration c7;
          c7.push_back ( Triangle ( Corners::V2, Corners::V6, Corners::V6, Corners::V7, Corners::V4, Corners::V6 ) );
          c7.push_back ( Triangle ( Corners::V4, Corners::V5, Corners::V5, Corners::V7, Corners::V5, Corners::V1 ) );
          c7.push_back ( Triangle ( Corners::V1, Corners::V3, Corners::V3, Corners::V7, Corners::V2, Corners::V3 ) );
          _configurations.push_back ( c7 );
        }

        // C8
        {
          Configuration c8;
          c8.push_back ( Triangle ( Corners::V7, Corners::V5, Corners::V3, Corners::V1, Corners::V6, Corners::V4 ) );
          c8.push_back ( Triangle ( Corners::V2, Corners::V0, Corners::V6, Corners::V4, Corners::V3, Corners::V1 ) );
          _configurations.push_back ( c8 );
        }

        // C9
        {
          Configuration c9;
          c9.push_back ( Triangle ( Corners::V2, Corners::V6, Corners::V4, Corners::V6, Corners::V4, Corners::V5 ) );
          c9.push_back ( Triangle ( Corners::V2, Corners::V6, Corners::V4, Corners::V5, Corners::V2, Corners::V3 ) );
          c9.push_back ( Triangle ( Corners::V4, Corners::V5, Corners::V1, Corners::V5, Corners::V2, Corners::V3 ) );
          c9.push_back ( Triangle ( Corners::V1, Corners::V5, Corners::V1, Corners::V3, Corners::V2, Corners::V3 ) );
          _configurations.push_back ( c9 );
        }

        // C10
        {
          Configuration c10;
          c10.push_back ( Triangle ( Corners::V4, Corners::V5, Corners::V0, Corners::V4, Corners::V2, Corners::V6 ) );
          c10.push_back ( Triangle ( Corners::V4, Corners::V5, Corners::V2, Corners::V6, Corners::V6, Corners::V7 ) );
          c10.push_back ( Triangle ( Corners::V1, Corners::V5, Corners::V3, Corners::V7, Corners::V0, Corners::V1 ) );
          c10.push_back ( Triangle ( Corners::V3, Corners::V7, Corners::V2, Corners::V3, Corners::V0, Corners::V1 ) );
          _configurations.push_back ( c10 );
        }

        // C11
        {
          Configuration c11;
          c11.push_back ( Triangle ( Corners::V4, Corners::V6, Corners::V4, Corners::V5, Corners::V0, Corners::V2 ) );
          c11.push_back ( Triangle ( Corners::V4, Corners::V5, Corners::V3, Corners::V7, Corners::V0, Corners::V2 ) );
          c11.push_back ( Triangle ( Corners::V3, Corners::V7, Corners::V2, Corners::V3, Corners::V0, Corners::V2 ) );
          c11.push_back ( Triangle ( Corners::V4, Corners::V5, Corners::V1, Corners::V5, Corners::V3, Corners::V7 ) );
          _configurations.push_back ( c11 );
        }

        // C12
        {
          Configuration c12;
          c12.push_back ( Triangle ( Corners::V5, Corners::V7, Corners::V1, Corners::V3, Corners::V0, Corners::V2 ) );
          c12.push_back ( Triangle ( Corners::V5, Corners::V7, Corners::V0, Corners::V2, Corners::V0, Corners::V4 ) );
          c12.push_back ( Triangle ( Corners::V5, Corners::V7, Corners::V0, Corners::V4, Corners::V4, Corners::V5 ) );
          c12.push_back ( Triangle ( Corners::V4, Corners::V6, Corners::V6, Corners::V2, Corners::V6, Corners::V7 ) );
          _configurations.push_back ( c12 );
        }

        // C13
        {
          Configuration c13;
          c13.push_back ( Triangle ( Corners::V4, Corners::V5, Corners::V4, Corners::V0, Corners::V4, Corners::V6 ) );
          c13.push_back ( Triangle ( Corners::V6, Corners::V7, Corners::V7, Corners::V3, Corners::V7, Corners::V5 ) );
          c13.push_back ( Triangle ( Corners::V0, Corners::V2, Corners::V2, Corners::V3, Corners::V2, Corners::V6 ) );
          c13.push_back ( Triangle ( Corners::V0, Corners::V1, Corners::V1, Corners::V5, Corners::V1, Corners::V3 ) );
          _configurations.push_back ( c13 );
        }

        // C14
        {
          Configuration c14;
          c14.push_back ( Triangle ( Corners::V0, Corners::V4, Corners::V4, Corners::V5, Corners::V2, Corners::V6 ) );
          c14.push_back ( Triangle ( Corners::V4, Corners::V5, Corners::V1, Corners::V3, Corners::V2, Corners::V6 ) );
          c14.push_back ( Triangle ( Corners::V1, Corners::V3, Corners::V2, Corners::V3, Corners::V2, Corners::V6 ) );
          c14.push_back ( Triangle ( Corners::V4, Corners::V5, Corners::V5, Corners::V7, Corners::V1, Corners::V3 ) );
          _configurations.push_back ( c14 );
        }

        // C15
        {
          Configuration c15;
          c15.push_back ( Triangle ( Corners::V2, Corners::V6, Corners::V4, Corners::V6, Corners::V4, Corners::V5 ) );
          c15.push_back ( Triangle ( Corners::V2, Corners::V6, Corners::V4, Corners::V5, Corners::V2, Corners::V3 ) );
          c15.push_back ( Triangle ( Corners::V4, Corners::V5, Corners::V1, Corners::V5, Corners::V2, Corners::V3 ) );
          c15.push_back ( Triangle ( Corners::V1, Corners::V5, Corners::V1, Corners::V3, Corners::V2, Corners::V3 ) );
          c15.push_back ( Triangle ( Corners::V6, Corners::V7, Corners::V7, Corners::V3, Corners::V7, Corners::V5 ) );
          _configurations.push_back ( c15 );
        }

        // C16
        {
          Configuration c16;
          c16.push_back ( Triangle ( Corners::V5, Corners::V7, Corners::V4, Corners::V6, Corners::V3, Corners::V7 ) );
          c16.push_back ( Triangle ( Corners::V4, Corners::V6, Corners::V2, Corners::V3, Corners::V3, Corners::V7 ) );
          c16.push_back ( Triangle ( Corners::V4, Corners::V6, Corners::V0, Corners::V4, Corners::V2, Corners::V3 ) );
          c16.push_back ( Triangle ( Corners::V0, Corners::V4, Corners::V1, Corners::V3, Corners::V2, Corners::V3 ) );
          c16.push_back ( Triangle ( Corners::V0, Corners::V4, Corners::V1, Corners::V5, Corners::V1, Corners::V3 ) );
          _configurations.push_back ( c16 );
        }

        // C17
        {
          Configuration c17;
          c17.push_back ( Triangle ( Corners::V5, Corners::V7, Corners::V0, Corners::V2, Corners::V1, Corners::V3 ) );
          c17.push_back ( Triangle ( Corners::V5, Corners::V7, Corners::V0, Corners::V4, Corners::V0, Corners::V2 ) );
          c17.push_back ( Triangle ( Corners::V5, Corners::V7, Corners::V4, Corners::V5, Corners::V0, Corners::V4 ) );
          _configurations.push_back ( c17 );
        }

        // C18
        {
          Configuration c18;
          c18.push_back ( Triangle ( Corners::V4, Corners::V5, Corners::V4, Corners::V6, Corners::V0, Corners::V4 ) );
          c18.push_back ( Triangle ( Corners::V1, Corners::V3, Corners::V2, Corners::V3, Corners::V7, Corners::V3 ) );
          _configurations.push_back ( c18 );
        }

        // C19
        {
          Configuration c19;
          c19.push_back ( Triangle ( Corners::V6, Corners::V7, Corners::V4, Corners::V6, Corners::V0, Corners::V4 ) );
          c19.push_back ( Triangle ( Corners::V6, Corners::V7, Corners::V0, Corners::V4, Corners::V3, Corners::V7 ) );
          c19.push_back ( Triangle ( Corners::V0, Corners::V4, Corners::V5, Corners::V7, Corners::V3, Corners::V7 ) );
          c19.push_back ( Triangle ( Corners::V0, Corners::V4, Corners::V4, Corners::V5, Corners::V5, Corners::V7 ) );
          _configurations.push_back ( c19 );
        }

        // C20
        {
          Configuration c20;
          c20.push_back ( Triangle ( Corners::V5, Corners::V1, Corners::V4, Corners::V6, Corners::V4, Corners::V0 ) );
          c20.push_back ( Triangle ( Corners::V5, Corners::V1, Corners::V5, Corners::V7, Corners::V4, Corners::V6 ) );
          _configurations.push_back ( c20 );
        }

        // C21
        {
          Configuration c21;
          c21.push_back ( Triangle ( Corners::V4, Corners::V5, Corners::V4, Corners::V6, Corners::V0, Corners::V4 ) );
          _configurations.push_back ( c21 );
        }

        // C22
        {
          Configuration c22;
          _configurations.push_back ( c22 );
        }
      }

      const Configuration& operator () ( unsigned int index ) const
      {
        return _configurations.at ( index );
      }

      template < class RotationMap >
      void apply ( Configuration& c, const RotationMap& m )
      {
        for ( Configuration::iterator iter = c.begin (); iter != c.end (); ++iter )
        {
          iter->_p0 = m.find ( iter->_p0 )->second;
          iter->_p1 = m.find ( iter->_p1 )->second;
          iter->_p2 = m.find ( iter->_p2 )->second;
          iter->_p3 = m.find ( iter->_p3 )->second;
          iter->_p4 = m.find ( iter->_p4 )->second;
          iter->_p5 = m.find ( iter->_p5 )->second;
        }
      }

    private:
      ConfigurationList _configurations;
    };
  }
}
}

#endif // __USUL_ALGORITHMS_DETAIL_CONFIGURATION_H__