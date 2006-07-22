
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IOptionsPageAdd
  {
    /// <summary>
    /// Get the name of the page.
    /// </summary>
    string Name { get; }

    /// <summary>
    /// Get the image (System.Drawing.Image) or the name 
    /// of the image file (string).
    /// </summary>
    object Image { get; }

    /// <summary>
    /// Should be a System.Windows.Forms.Control.
    /// </summary>
    object Contents { get; }
  }
}
