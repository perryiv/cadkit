
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IOptionsForm : IReferenced
  {
    /// <summary>
    /// Add a page.
    /// </summary>
    void add(CadKit.Interfaces.IOptionsPage pageObject);

    /// <summary>
    /// Convenience function to add a page.
    /// </summary>
    CadKit.Interfaces.IOptionsPage newPage(string name, string iconFile);

    /// <summary>
    /// Make a page. The image should be a System.Drawing.Image.
    /// </summary>
    CadKit.Interfaces.IOptionsPage newPage(string name, object image);

    /// <summary>
    /// Get the number of pages.
    /// </summary>
    int NumPages { get; }
  }
}
