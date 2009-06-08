
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Tools
{
  public static class Random
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private static System.Random _random = new System.Random();

    /// <summary>
    /// Return a random color.
    /// </summary>
    public static System.Drawing.Color color()
    {
      lock ("CadKit.Tools.Random.color")
      {
        System.Drawing.Color color = System.Drawing.Color.FromArgb ( 255, _random.Next ( 0, 255 ), _random.Next ( 0, 255 ), _random.Next ( 0, 255 ) );
        return color;
      }
    }
  }
}
