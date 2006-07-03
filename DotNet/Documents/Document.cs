
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Documents
{
  abstract public class Document : CadKit.Interfaces.IDocument
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private bool _modified = false;
    protected object _mutex = new object();
    private string _name = "Untitled" + CadKit.Documents.Manager.Instance.NumDocuments.ToString();
    private bool _hasDefaultName = true;

    /// <summary>
    /// Constructor
    /// </summary>
    protected Document()
    {
    }

    /// <summary>
    /// Return true if this document has been modified 
    /// after the last time it was saved.
    /// </summary>
    bool CadKit.Interfaces.IDocument.Modified
    {
      get { lock (_mutex) { return _modified; } }
      set { lock (_mutex) { _modified = value; } }
    }

    /// <summary>
    /// Get/set the name of this document. This may be the default 
    /// name (e.g., "Untitled1").
    /// </summary>
    string CadKit.Interfaces.IDocument.Name
    {
      get { lock (_mutex) { return _name; } }
      set { lock (_mutex) { _name = value; } }
    }

    /// <summary>
    /// True if a name has not been assigned (e.g., "Untitled1".
    /// </summary>
    bool CadKit.Interfaces.IDocument.HasDefaultName
    {
      get { lock (_mutex) { return _hasDefaultName; } }
    }
  }
}
