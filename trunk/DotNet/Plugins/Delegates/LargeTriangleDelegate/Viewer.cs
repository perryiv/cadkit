
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Delegates.LargeTriangleDelegate
{
  class Viewer : WeifenLuo.WinFormsUI.DockContent
  {
    /// <summary>
    /// Construct a view.
    /// </summary>
    public Viewer() : base()
    {
      //this.PersistentName = this.GetType().ToString();
      CadKit.OpenGL.Canvas canvas = new CadKit.OpenGL.Canvas();
      this.Controls.Add(canvas);
      canvas.Dock = System.Windows.Forms.DockStyle.Fill;
    }
  }
}
