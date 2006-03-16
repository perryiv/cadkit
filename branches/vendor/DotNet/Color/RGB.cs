
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
  namespace Color
  {
    public class RGB
    {
      public int r = 0;
      public int g = 0;
      public int b = 0;

      public RGB()
      {
      }

      public RGB( int red, int green, int blue )
      {
        r = red;
        g = green;
        b = blue;
      }

      public RGB( RGB rgb )
      {
        r = rgb.r;
        g = rgb.g;
        b = rgb.b;
      }

      public RGB( System.Drawing.Color color )
      {
        this._set( color );
      }

      public RGB( CadKit.Color.HSV hsv )
      {
        this._set( hsv );
      }

      private void _set( System.Drawing.Color color )
      {
        if ( null != color )
        {
          r = color.R;
          g = color.G;
          b = color.B;
        }
      }

      private void _set( CadKit.Color.HSV hsv )
      {
        int[] rgb = CadKit.Color.RGB._convert( hsv );
        if ( null != rgb && 3 == rgb.Length )
        {
          r = rgb[0];
          g = rgb[1];
          b = rgb[2];
        }
      }

      /// <summary>
      /// Adapted from: http://msdn.microsoft.com/msdnmag/issues/03/07/GDIColorPicker/
      /// </summary>
      private static int[] _convert( CadKit.Color.HSV hsv )
      {
        // HSV contains values scaled as in the color wheel:
        // that is, all from 0 to 255. 

        // for ( this code to work, HSV.Hue needs
        // to be scaled from 0 to 360 (it//s the angle of the selected
        // point within the circle). HSV.Saturation and HSV.value must be 
        // scaled to be between 0 and 1.

        double h;
        double s;
        double v;

        double r = 0;
        double g = 0;
        double b = 0;

        // Scale Hue to be between 0 and 360. Saturation
        // and value scale to be between 0 and 1.
        h = ( ( double ) hsv.h / 255 * 360 ) % 360;
        s = ( double ) hsv.s / 255;
        v = ( double ) hsv.v / 255;

        if ( s == 0 )
        {
          // If s is 0, all colors are the same.
          // This is some flavor of gray.
          r = v;
          g = v;
          b = v;
        }
        else
        {
          double p;
          double q;
          double t;

          double fractionalSector;
          int sectorNumber;
          double sectorPos;

          // The color wheel consists of 6 sectors.
          // Figure out which sector you//re in.
          sectorPos = h / 60;
          sectorNumber = ( int ) ( System.Math.Floor( sectorPos ) );

          // get the fractional part of the sector.
          // That is, how many degrees into the sector
          // are you?
          fractionalSector = sectorPos - sectorNumber;

          // Calculate values for the three axes
          // of the color. 
          p = v * ( 1 - s );
          q = v * ( 1 - ( s * fractionalSector ) );
          t = v * ( 1 - ( s * ( 1 - fractionalSector ) ) );

          // Assign the fractional colors to r, g, and b
          // based on the sector the angle is in.
          switch ( sectorNumber )
          {
            case 0:
              r = v;
              g = t;
              b = p;
              break;

            case 1:
              r = q;
              g = v;
              b = p;
              break;

            case 2:
              r = p;
              g = v;
              b = t;
              break;

            case 3:
              r = p;
              g = q;
              b = v;
              break;

            case 4:
              r = t;
              g = p;
              b = v;
              break;

            case 5:
              r = v;
              g = p;
              b = q;
              break;
          }
        }
        // return an RGB structure, with values scaled
        // to be between 0 and 255.
        return new int[] { ( int ) ( r * 255 ), ( int ) ( g * 255 ), ( int ) ( b * 255 ) };
      }
    }
  }
}
