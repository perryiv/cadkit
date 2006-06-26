
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
    /// Data members.
    /// </summary>
    bool _modified = false;
    private object _mutex = new object();

    /// <summary>
    /// Construct a document.
    /// </summary>
    public Document()
    {
    }

    /// <summary>
    /// Set/get the modified flag.
    /// </summary>
    bool CadKit.Interfaces.IDocument.Modified
    {
      get
      {
        lock (_mutex)
        {
          return _modified;
        }
      }
      set
      {
        lock (_mutex)
        {
          _modified = value;
        }
      }
    }
  }
}
