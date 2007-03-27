
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
    public class HSV
    {
      public int h = 0;
      public int s = 0;
      public int v = 0;

      public HSV()
      {
      }

      public HSV( int hue, int saturation, int value )
      {
        h = hue;
        s = saturation;
        v = value;
      }

      public HSV( HSV hsv )
      {
        h = hsv.h;
        s = hsv.s;
        v = hsv.v;
      }

      public HSV( System.Drawing.Color color )
      {
        this._set( new CadKit.Color.RGB( color ) );
      }

      public HSV( CadKit.Color.RGB rgb )
      {
        this._set( rgb );
      }

      private void _set( CadKit.Color.RGB rgb )
      {
        int[] hsv = CadKit.Color.HSV._convert( rgb );
        if ( null != hsv && 3 == hsv.Length )
        {
          h = hsv[0];
          s = hsv[1];
          v = hsv[2];
        }
      }

      /// <summary>
      /// Adapted from: http://msdn.microsoft.com/msdnmag/issues/03/07/GDIColorPicker/
      /// </summary>
      private static int[] _convert( CadKit.Color.RGB rgb )
      {
				// In this function, R, G, and B values must be scaled 
				// to be between 0 and 1.
				// HSV.Hue will be a value between 0 and 360, and 
				// HSV.Saturation and value are between 0 and 1.
				// The code must scale these to be between 0 and 255 for
				// the purposes of this application.

				double min;
				double max;
				double delta;

        double r = ( double ) rgb.r / 255;
        double g = ( double ) rgb.g / 255;
        double b = ( double ) rgb.b / 255;

				double h;
				double s;
				double v;

        min = System.Math.Min( System.Math.Min( r, g ), b );
        max = System.Math.Max( System.Math.Max( r, g ), b );
				v = max;
				delta = max - min;
				if ( max == 0 || delta == 0 ) 
				{
					// R, G, and B must be 0, or all the same.
					// In this case, S is 0, and H is undefined.
					// Using H = 0 is as good as any...
					s = 0;
					h = 0;
				} 
				else 
				{
					s = delta / max;
					if ( r == max ) 
					{
						// Between Yellow and Magenta
						h = (g - b) / delta;
					} 
					else if ( g == max ) 
					{
						// Between Cyan and Yellow
						h = 2 + (b - r) / delta;
					} 
					else 
					{
						// Between Magenta and Cyan
						h = 4 + (r - g) / delta;
					}

				}
				// Scale h to be between 0 and 360. 
				// This may require adding 360, if the value
				// is negative.
				h *= 60;
				if ( h < 0 ) 
				{
					h += 360;
				}

				// Scale to the requirements of this 
				// application. All values are between 0 and 255.
        return new int[] { ( int ) ( h / 360 * 255 ), ( int ) ( s * 255 ), ( int ) ( v * 255 ) };
      }
    }
  }
}
