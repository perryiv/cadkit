
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
    /// Call this to resize OpenGL.
    /// </summary>
    public void resizeOpenGL()
    {
      try
      {
        if (null != _renderContext)
        {
          this._resizeOpenGL();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2136628702: {0}\n{1}", e.Message, e.StackTrace);
      }
    }

    /// <summary>
    /// Call this to resize OpenGL.
    /// </summary>
    protected virtual void _resizeOpenGL()
    {
    }
  }
}
