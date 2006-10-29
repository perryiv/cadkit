
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
      lock ( _mutex )
      {
        CadKit.Interfaces.IProgressBar progress = caller as CadKit.Interfaces.IProgressBar;
        if (null != progress)
        {
          int total = progress.Range;
          for (int i = 0; i < total; ++i)
          {
            progress.Value = i;
            System.Threading.Thread.Sleep(1000);
          }
          progress.Value = total;
        }
      }
    }
  }
}
