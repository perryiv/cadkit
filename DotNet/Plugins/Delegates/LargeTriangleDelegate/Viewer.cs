
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Delegates.LargeTriangleDelegate
{
  class Viewer : System.Windows.Forms.Form
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private object _mutex = new object();

    /// <summary>
    /// Construct a view.
    /// </summary>
    public Viewer() : base()
    {
      CadKit.Viewer.Viewer viewer = new CadKit.Viewer.Viewer();
      viewer.init();
      this.Controls.Add(viewer);
      viewer.Dock = System.Windows.Forms.DockStyle.Fill;
    }
  }
}
