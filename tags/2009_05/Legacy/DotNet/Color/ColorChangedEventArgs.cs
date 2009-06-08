
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
    public class ColorChangedEventArgs : System.EventArgs
    {
      public ColorChangedEventArgs( System.Drawing.Color color ) : base()
      {
        _color = color;
      }

      public System.Drawing.Color Color
      {
        get { return _color; }
      }

      private System.Drawing.Color _color = System.Drawing.Color.Black;
    }
  }
}
