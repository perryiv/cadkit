
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public class OptionsPageDelegates
  {
    /// <summary>
    /// Delegate for applying settings.
    /// </summary>
    public delegate void ApplyDelegate(CadKit.Interfaces.IOptionsForm form, CadKit.Interfaces.IOptionsPage page);
  }

  public interface IOptionsPage : IReferenced
  {
    /// <summary>
    /// Set/get the apply delegate.
    /// </summary>
    CadKit.Interfaces.OptionsPageDelegates.ApplyDelegate Apply { get; set; }

    /// <summary>
    /// Apply the settings.
    /// </summary>
    void apply(CadKit.Interfaces.IOptionsForm form);

    /// <summary>
    /// Get the name.
    /// </summary>
    string Name { get; }

    /// <summary>
    /// Get the image. Should be a System.Drawing.Image.
    /// </summary>
    object Image { get; }

    /// <summary>
    /// Get the contents. Should be a System.Windows.Forms.Control.
    /// </summary>
    object Contents { get; }
  }
}
