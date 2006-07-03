
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Images
{
  public class Image : System.Object
  {
    /// <summary>
    /// Return a new image, or null.
    /// </summary>
    public static System.Drawing.Image load(string file)
    {
      try
      {
        System.Drawing.Image image = System.Drawing.Image.FromFile(file);
        return image;
      }
      catch (System.IO.FileNotFoundException)
      {
        System.Console.WriteLine("Warning {0}: File '{1}' not found", 2384415761, file);
        return null;
      }
    }
  }
}
