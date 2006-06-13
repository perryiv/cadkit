
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.LargeTriangleDocument
{
  public class Document : CadKit.Interfaces.IDocument
  {
    /// <summary>
    /// Construct a document.
    /// </summary>
    private Document()
    {
    }

    /// <summary>
    /// Return a new document.
    /// </summary>
    public static CadKit.Interfaces.IDocument createDocument()
    {
      return new Document();
    }

    /// <summary>
    /// Set/get the modified flag.
    /// </summary>
    bool CadKit.Interfaces.IDocument.Modified
    {
      get { return _modified; }
      set { _modified = value; }
    }

    /// <summary>
    /// Data members.
    /// </summary>
    bool _modified = false;
  }
}
