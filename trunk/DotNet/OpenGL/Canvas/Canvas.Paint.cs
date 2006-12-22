
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
    /// Call this to paint using OpenGL.
    /// </summary>
    public void paintOpenGL()
    {
      try
      {
        if (null != _renderContext && _renderContext.valid())
        {
          if (null != this.BeginPaintEvent)
            this.BeginPaintEvent();

          // Do the drawing.
          this._paintOpenGL();

          if (null != this.EndPaintEvent)
            this.EndPaintEvent();
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

    public delegate void BeginPaintDelegate();
    public delegate void EndPaintDelegate();

    public event BeginPaintDelegate BeginPaintEvent;
    public event EndPaintDelegate EndPaintEvent;
  }
}
