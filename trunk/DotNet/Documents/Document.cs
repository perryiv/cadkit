
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
    CadKit.Referenced.Base,
    CadKit.Interfaces.IDocument,
    CadKit.Interfaces.IDocumentIcon,
    CadKit.Interfaces.IGuiCreate,
    CadKit.Interfaces.IWindowsForward,
    CadKit.Interfaces.INotifyChanged
  {
    /// <summary>
    /// Local types.
    /// </summary>
    private class DocViews : System.Collections.Generic.List<CadKit.Interfaces.IDocumentView> { }
    private class CommandHistoryHandle : CadKit.Interfaces.ScopedReference<CadKit.Interfaces.ICommandHistory> { }


    /// <summary>
    /// Data members.
    /// </summary>
    private bool _modified = false;
    private string _name = "Untitled" + CadKit.Documents.Manager.Instance.NumDocuments.ToString();
    private bool _hasDefaultName = true;
    private CommandHistoryHandle _commands = new CommandHistoryHandle();
    private CadKit.Interfaces.IGuiDelegate _gui = null;
    private DocViews _views = new DocViews();
    private CadKit.Interfaces.NotifyChangedDelegate _notifyChanged = null;
    private object _icon = null;


    /// <summary>
    /// Constructor
    /// </summary>
    protected Document()
    {
    }


    /// <summary>
    /// Destructor
    /// </summary>
    ~Document()
    {
    }


    /// <summary>
    /// Called when the reference count goes to zero.
    /// </summary>
    protected override void _cleanup()
    {
      try
      {
        this._closeAllViews();
        if (true == _commands.Valid)
        {
          _commands.Value.clear();
          _commands.Value = null;
        }
        if (null != _gui)
        {
          _gui.Document = null;
          _gui = null;
        }
        if (null != _views)
        {
          _views.Clear();

          // Not setting to null here is a work-around for a bug related to 
          // the fact that the views, documents, and document-manager are 
          // too tightly coupled.
          _views = new DocViews();
        }
        base._cleanup();
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3201769786: {0}", e.Message);
      }
    }


    /// <summary>
    /// Close all the views.
    /// </summary>
    private void _closeAllViews()
    {
      try
      {
        // Make a copy of the views because, as each one closes, 
        // it removes itself from the list.
        CadKit.Interfaces.IDocumentView[] views = this.views();

        if (null != views)
        {
          // Loop through the views.
          foreach (CadKit.Interfaces.IDocumentView view in views)
          {
            try
            {
              view.close();
              this.remove(view);
            }
            catch (System.Exception e1)
            {
              System.Console.WriteLine("Error 1770634745: {0}", e1.Message);
            }
          }
        }
      }
      catch (System.Exception e2)
      {
        System.Console.WriteLine("Error 4443490050: {0}", e2.Message);
      }
    }


    /// <summary>
    /// Add the view.
    /// </summary>
    void CadKit.Interfaces.IDocument.add(CadKit.Interfaces.IDocumentView view)
    {
      this.add(view);
    }


    /// <summary>
    /// Close the document.
    /// </summary>
    void CadKit.Interfaces.IDocument.close()
    {
      this.close();
    }


    /// <summary>
    /// Close the document.
    /// </summary>
    public void close()
    {
      //this._cleanup();
    }


    /// <summary>
    /// Add the view.
    /// </summary>
    void add(CadKit.Interfaces.IDocumentView view)
    {
      if (null != view && false == this.contains(view))
      {
        using (this.Lock.write())
        {
          _views.Add(view);
        }
      }
    }


    /// <summary>
    /// Remove the view.
    /// </summary>
    void CadKit.Interfaces.IDocument.remove(CadKit.Interfaces.IDocumentView view)
    {
      this.remove(view);
    }


    /// <summary>
    /// Remove the view.
    /// </summary>
    public void remove(CadKit.Interfaces.IDocumentView view)
    {
      while (true == this.contains(view))
      {
        using (this.Lock.write())
        {
          _views.Remove(view);
        }
      }
      if (0 == this.NumViews)
      {
        CadKit.Documents.Manager.Instance.remove(this);
      }
    }


    /// <summary>
    /// Return the number of views.
    /// </summary>
    public uint NumViews
    {
      get { using (this.Lock.read()) { return (uint)_views.Count; } }
    }


    /// <summary>
    /// Does the document contain the view?
    /// </summary>
    bool CadKit.Interfaces.IDocument.contains(CadKit.Interfaces.IDocumentView view)
    {
      return this.contains(view);
    }


    /// <summary>
    /// Does the document contain the view?
    /// </summary>
    public bool contains(CadKit.Interfaces.IDocumentView view)
    {
      using (this.Lock.read())
      {
        return _views.Contains(view);
      }
    }


    /// <summary>
    /// Get the array of views.
    /// </summary>
    CadKit.Interfaces.IDocumentView[] CadKit.Interfaces.IDocument.views()
    {
      return this.views();
    }


    /// <summary>
    /// Get the array of views.
    /// </summary>
    public CadKit.Interfaces.IDocumentView[] views()
    {
      using (this.Lock.read())
      {
        return ((null == _views) ? null : _views.ToArray());
      }
    }


    /// <summary>
    /// Set/get the command history.
    /// </summary>
    public CadKit.Interfaces.ICommandHistory CommandHistory
    {
      get { using (this.Lock.read()) { return _commands.Value; } }
      set { using (this.Lock.write()) { _commands.Value = value; } }
    }


    /// <summary>
    /// Get/set the gui-delegate.
    /// </summary>
    CadKit.Interfaces.IGuiDelegate CadKit.Interfaces.IDocument.GuiDelegate
    {
      get { return this.GuiDelegate; }
      set { this.GuiDelegate = value; }
    }


    /// <summary>
    /// Get/set the gui-delegate.
    /// </summary>
    CadKit.Interfaces.IGuiDelegate GuiDelegate
    {
      get { using (this.Lock.read()) { return _gui; } }
      set { using (this.Lock.write()) { _gui = value; } }
    }


    /// <summary>
    /// Return true if this document has been modified 
    /// after the last time it was saved.
    /// </summary>
    bool CadKit.Interfaces.IDocument.Modified
    {
      get { return this.Modified; }
      set { this.Modified = value; }
    }


    /// <summary>
    /// Return true if this document has been modified 
    /// after the last time it was saved.
    /// </summary>
    protected bool Modified
    {
      get { using (this.Lock.read()) { return _modified; } }
      set
      {
        using (this.Lock.write())
        {
          _modified = value;
        }
        CadKit.Interfaces.NotifyChangedDelegate notify = this.NotifyChanged;
        if (null != notify)
        {
          notify(this, this);
        }
      }
    }


    /// <summary>
    /// Get/set the name of this document. This may be the default 
    /// name (e.g., "Untitled1").
    /// </summary>
    string CadKit.Interfaces.IDocument.Name
    {
      get { return this.Name; }
      set { this.Name = value; }
    }

    /// <summary>
    /// Get/set the name of this document. This may be the default 
    /// name (e.g., "Untitled1").
    /// </summary>
    public string Name
    {
      // Was causing deadlock when opening many files at once.
      get { using (this.Lock.read()) { return _name; } }
      set { using (this.Lock.write()) { _name = value; } }
    }


    /// <summary>
    /// True if a name has not been assigned (e.g., "Untitled1").
    /// </summary>
    bool CadKit.Interfaces.IDocument.HasDefaultName
    {
      get { return this.HasDefaultName; }
    }


    /// <summary>
    /// True if a name has not been assigned (e.g., "Untitled1").
    /// </summary>
    protected bool HasDefaultName
    {
      get { using (this.Lock.read()) { return _hasDefaultName; } }
      set { using (this.Lock.write()) { _hasDefaultName = value; } }
    }


    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IDocument.TypeName
    {
      get { return this._typeName(); }
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
      CadKit.Interfaces.IGuiDelegate gui = this.GuiDelegate;
      {
        if (null != gui)
        {
          // Potential deadlock if this calls BeginInvoke and
          // tries to write to this instance.
          gui.create(caller);
        }
      }
    }


    /// <summary>
    /// Bring the windows forward.
    /// </summary>
    void CadKit.Interfaces.IWindowsForward.windowsForward(object caller)
    {
      CadKit.Interfaces.IWindowsForward forward = this.GuiDelegate as CadKit.Interfaces.IWindowsForward;
      if (null != forward)
      {
        forward.windowsForward(caller);
      }
    }


    /// <summary>
    /// Get/set the display-list-use-changed delegate.
    /// </summary>
    CadKit.Interfaces.NotifyChangedDelegate CadKit.Interfaces.INotifyChanged.NotifyChanged
    {
      get { return this.NotifyChanged; }
      set { this.NotifyChanged = value; }
    }


    /// <summary>
    /// Get/set the display-list-use-changed delegate.
    /// </summary>
    public CadKit.Interfaces.NotifyChangedDelegate NotifyChanged
    {
      get { using (this.Lock.read()) { return _notifyChanged; } }
      set { using (this.Lock.write()) { _notifyChanged = value; } }
    }


    /// <summary>
    /// Get/Set the icon.
    /// </summary>
    object CadKit.Interfaces.IDocumentIcon.Icon
    {
      get { return this.Icon; }
      set { this.Icon = value; }
    }


    /// <summary>
    /// Get/Set the icon.
    /// </summary>
    public object Icon
    {
      get { using (this.Lock.read()) { return _icon; } }
      set { using (this.Lock.write()) { _icon = value; } }
    }
  }
}
