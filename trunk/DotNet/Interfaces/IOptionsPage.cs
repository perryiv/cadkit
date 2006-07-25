
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IOptionsPage
  {
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
