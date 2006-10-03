
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.OpenGL
{
  public partial class Canvas : System.Windows.Forms.UserControl
  {
    /// <summary>
    /// Call this to paint using OpenGL.
    /// </summary>
    public void paintOpenGL()
    {
      try
      {
        if (null != _renderContext && _renderContext.valid())
        {
          // Make the context current.
          this._renderContext.makeCurrent();

          // Do the drawing.
          this._paintOpenGL();

          // Swap the buffers.
          this._renderContext.swapBuffers();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 9052564260: {0}\n{1}", e.Message, e.StackTrace);
      }
    }

    /// <summary>
    /// Call this to paint using OpenGL.
    /// </summary>
    protected virtual void _paintOpenGL()
    {
    }
  }
}
