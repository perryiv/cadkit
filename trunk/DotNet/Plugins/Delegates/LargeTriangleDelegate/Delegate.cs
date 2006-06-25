
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.LargeTriangleDelegate
{
  public class Delegate : CadKit.Interfaces.IGuiDelegate
  {
    /// <summary>
    /// Create the default gui.
    /// </summary>
    CadKit.Interfaces.IDocument _document = null;

    /// <summary>
    /// Construct a document.
    /// </summary>
    public Delegate()
    {
    }

    /// <summary>
    /// Set the document.
    /// </summary>
    void CadKit.Interfaces.IGuiDelegate.setDocument(CadKit.Interfaces.IDocument doc)
    {
      _document = doc;
    }

    /// <summary>
    /// Create the default gui.
    /// </summary>
    void CadKit.Interfaces.IGuiDelegate.createDefaultGui(object caller)
    {
    }
  }
}
