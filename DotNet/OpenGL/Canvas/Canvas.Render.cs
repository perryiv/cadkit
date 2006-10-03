
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
    /// Call to repaint.
    /// </summary>
    public void render()
    {
      // Re-initializes the OpenGL context if needed.
      this.initRenderingContext();

      // Resize the viewport.
      this.resizeOpenGL();

      // Paint the window.
      this.paintOpenGL();
    }
  }
}
