
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface ICreateLayer : IReferenced
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
    /// Should return a System.Windows.Forms.Control.
    /// Argument is an CadKit.Helios.OptionsPage or similar.
    /// </summary>
    object contents(object page);
  }
}
