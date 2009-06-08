
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.OpenGL
{
  public partial class Canvas
  {
    /// <summary>
    /// Call this to get the pixel formats.
    /// </summary>
    public System.Data.DataTable availablePixelFormats()
    {
      try
      {
        if (null != _renderContext)
        {
          return CadKit.OpenGL.Glue.PixelFormat.query(_renderContext.deviceContext());
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1024033356: {0}\n{1}", e.Message, e.StackTrace);
      }
      return (new System.Data.DataTable());
    }
  }
}
