
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IDocumentView : IReferenced
  {
    /// <summary>
    /// Get/set the document.
    /// </summary>
    CadKit.Interfaces.IDocument Document { get; set; }

    /// <summary>
    /// Close the view.
    /// </summary>
    void close();
  }
}
