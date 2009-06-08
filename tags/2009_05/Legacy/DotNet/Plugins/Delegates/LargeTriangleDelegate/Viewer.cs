
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Delegates.LargeTriangleDelegate
{
  class Viewer : CadKit.Viewer.Viewer
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private object _mutex = new object();
    private CadKit.Interfaces.IDocument _document = null;

    /// <summary>
    /// Construct a view.
    /// </summary>
    public Viewer(object caller, CadKit.Interfaces.IDocument document) : base()
    {
      this.Icon = System.Windows.Forms.Application.OpenForms[0].Icon;
      this.MdiParent = caller as System.Windows.Forms.Form;

      _document = document;
      this.Text = (null == _document) ? this.ToString() : _document.Name;

      this.Load += this._load;
    }

    /// <summary>
    /// Called when the form is loaded.
    /// </summary>
    private void _load(object sender, System.EventArgs e)
    {
      this.init();      
    }
  }
}
