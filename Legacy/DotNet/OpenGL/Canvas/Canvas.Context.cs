
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
    /// Call this to initialize OpenGL.
    /// </summary>
    public void initRenderingContext()
    {
      try
      {
        if (null != _renderContext)
          return;

        if (null == this.FindForm())
          return;

        #if no
        System.Console.WriteLine(System.String.Format("{0}.initRenderingContext()", this.GetType().ToString()));
        #endif

        this._initInnerPanel();
        System.IntPtr temp = this.InnerControl.Handle; // Make sure it's created.

        _renderContext = new CadKit.OpenGL.Glue.RenderContext(this.InnerControl, _pixelFormat);
        if (null == _renderContext)
          return;

        _renderContext.reference();
        _pixelFormat = _renderContext.pixelFormat();
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 4576493160: {0}\n{1}", e.Message, e.StackTrace);
      }
    }

    /// <summary>
    /// Call this to delete the current rendering context.
    /// </summary>
    private void _deleteRenderingContext()
    {
      if (null != _renderContext)
      {
        _renderContext.dereference();
        _renderContext = null;
      }
    }


    /// <summary>
    /// Make the context current.
    /// </summary>
    protected void _makeCurrent()
    {
      _renderContext.makeCurrent();
    }


    /// <summary>
    /// Swap buffers.
    /// </summary>
    protected void _swapBuffers()
    {
      _renderContext.swapBuffers();
    }
  }
}
