
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Documents.LargeTriangleDocument
{
  public class Document : 
    CadKit.Documents.Document,
    CadKit.Interfaces.IRead
  {
    /// <summary>
    /// Constants
    /// </summary>
    public const string TypeName = "Large Triangle Document";

    /// <summary>
    /// Construct a document.
    /// </summary>
    public Document()
    {
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    protected override string _typeName()
    {
      lock (_mutex) { return CadKit.Plugins.Documents.LargeTriangleDocument.Document.TypeName; }
    }

    /// <summary>
    /// Read the file.
    /// </summary>
    void CadKit.Interfaces.IRead.read ( string name, object caller )
    {
    }
  }
}
