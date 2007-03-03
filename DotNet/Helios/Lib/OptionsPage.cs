
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
  public class OptionsPage : 
    CadKit.Referenced.Base,
    CadKit.Interfaces.IOptionsPage
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private object _mutex = new object();
    private string _name = null;
    private System.Drawing.Image _image = null;
    private System.Windows.Forms.Panel _contents = new System.Windows.Forms.Panel();
    private CadKit.Interfaces.OptionsPageDelegates.ApplyDelegate _apply = null;

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
    string CadKit.Interfaces.IOptionsPage.Name { get { lock (_mutex) { return _name; } } }

    /// <summary>
    /// Get the image.
    /// </summary>
    object CadKit.Interfaces.IOptionsPage.Image { get { lock (_mutex) { return _image; } } }

    /// <summary>
    /// Get the contents.
    /// </summary>
    object CadKit.Interfaces.IOptionsPage.Contents { get { lock (_mutex) { return _contents; } } }

    /// <summary>
    /// Set/get the apply delegate.
    /// </summary>
    CadKit.Interfaces.OptionsPageDelegates.ApplyDelegate CadKit.Interfaces.IOptionsPage.Apply
    {
      get { lock (_mutex) { return _apply; } }
      set { lock (_mutex) { _apply = value; } }
    }

    /// <summary>
    /// Apply the settings.
    /// </summary>
    void CadKit.Interfaces.IOptionsPage.apply ( CadKit.Interfaces.IOptionsForm form )
    {
      lock (_mutex)
      {
        if (null != _apply)
          _apply(form, this);
      }
    }
  }
}
