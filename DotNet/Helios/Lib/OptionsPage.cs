
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Options dialog that resembles Mozilla's.
//  See: http://www.differentpla.net/node/340
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios
{
  public class OptionsPage
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private object _mutex = new object();
    private string _name = null;
    private System.Drawing.Image _image = null;
    private System.Windows.Forms.Panel _page = new System.Windows.Forms.Panel();
    private NotifyDelegate _apply = null;

    /// <summary>
    /// Constructor
    /// </summary>
    public OptionsPage(string name, System.Drawing.Image image)
    {
      _name = name;
      _image = image;
    }

    /// <summary>
    /// Get the name.
    /// </summary>
    public string Name { get { lock (_mutex) { return _name; } } }

    /// <summary>
    /// Get the image.
    /// </summary>
    public System.Drawing.Image Image { get { lock (_mutex) { return _image; } } }

    /// <summary>
    /// Get the page.
    /// </summary>
    public System.Windows.Forms.Control Page { get { lock (_mutex) { return _page; } } }

    here down was mid-idea...

    /// <summary>
    /// Delegate for applying settings.
    /// </summary>
    public delegate void ApplyDelegate ( CadKit.Helios.OptionsForm form, CadKit.Helios.OptionsPage page );

    /// <summary>
    /// Set/get the apply delegate.
    /// </summary>
    public ApplyDelegate Apply
    {
      get { lock (_mutex) { return _apply; } }
      set { lock (_mutex) { _apply = value; } }
    }

    /// <summary>
    /// Apply the settings.
    /// </summary>
    public virtual void apply()
    {
    }
  }
}
