
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Documents
{
  abstract public class Document : 
    CadKit.Interfaces.IDocument,
    CadKit.Interfaces.IGuiCreate
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private bool _modified = false;
    protected object _mutex = new object();
    private string _name = "Untitled" + CadKit.Documents.Manager.Instance.NumDocuments.ToString();
    private bool _hasDefaultName = true;
    private CadKit.Interfaces.ICommandHistory _commands = null;
    private CadKit.Interfaces.IGuiDelegate _gui = null;

    /// <summary>
    /// Constructor
    /// </summary>
    protected Document()
    {
    }

    /// <summary>
    /// Set/get the command history.
    /// </summary>
    public CadKit.Interfaces.ICommandHistory CommandHistory
    {
      get { lock (_mutex) { return _commands; } }
      set { lock (_mutex) { _commands = value; } }
    }

    /// <summary>
    /// Get/set the gui-delegate.
    /// </summary>
    object CadKit.Interfaces.IDocument.GuiDelegate
    {
      get { lock (_mutex) { return _gui; } }
      set { lock (_mutex) { _gui = value as CadKit.Interfaces.IGuiDelegate; } }
    }

    /// <summary>
    /// Return true if this document has been modified 
    /// after the last time it was saved.
    /// </summary>
    bool CadKit.Interfaces.IDocument.Modified
    {
      get { lock (_mutex) { return _modified; } }
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

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IDocument.TypeName
    {
      get { lock (_mutex) { return this._typeName(); } }
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    protected abstract string _typeName();

    /// <summary>
    /// Create new gui.
    /// </summary>
    void CadKit.Interfaces.IGuiCreate.create(object caller)
    {
      if (null != _gui)
      {
        _gui.create(caller);
      }
    }
  }
}
