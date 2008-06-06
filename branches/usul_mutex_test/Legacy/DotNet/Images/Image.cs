
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
      catch (System.Exception e)
      {
        System.Console.WriteLine("Warning {0}: File '{1}' not loaded, {2}", 2384415761, file, e.Message);
        return null;
      }
    }


    /// <summary>
    /// Return a new icon, or null.
    /// </summary>
    public static System.Drawing.Icon icon(string file)
    {
      try
      {
        System.IO.FileInfo info = new System.IO.FileInfo(file);
        if ("ico" == info.Extension.ToLower())
        {
          System.Drawing.Icon icon = new System.Drawing.Icon(file);
          return icon;
        }
        else
        {
          System.Drawing.Image image = System.Drawing.Image.FromFile(file);
          System.Drawing.Bitmap bitmap = new System.Drawing.Bitmap(image);
          System.Drawing.Icon icon = System.Drawing.Icon.FromHandle(bitmap.GetHicon());
          return icon;
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Warning {0}: File '{1}' not loaded, {2}", 3270807086, file, e.Message);
        return null;
      }
    }
  }
}
