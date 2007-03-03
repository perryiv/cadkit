
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IGuiDelegate : IReferenced
  {
    /// <summary>
    /// Set/get the document.
    /// </summary>
    object Document { get; set; }

    /// <summary>
    /// Create the default gui.
    /// </summary>
    void create(object caller);

    /// <summary>
    /// Returns true if the document type is handled.
    /// </summary>
    bool doesHandle(string type);
  }
}
