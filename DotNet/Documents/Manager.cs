
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Documents
{
  public class Manager
  {
    /// <summary>
    /// Delegate.
    /// </summary>
    public delegate int ChooseBestOpenerDelegate(CadKit.Interfaces.IDocumentOpen[] openers, object caller);
    public delegate void ActiveDocumentChangedDelegate(CadKit.Interfaces.IDocument oldDoc, CadKit.Interfaces.IDocument newDoc);
    public delegate void ActiveViewChangedDelegate(CadKit.Interfaces.IDocumentView oldView, CadKit.Interfaces.IDocumentView newView);


    /// <summary>
    /// Local types.
    /// </summary>
    private class Documents : System.Collections.Generic.List<CadKit.Interfaces.IDocument> { }
    private class Openers : System.Collections.Generic.List<CadKit.Interfaces.IDocumentOpen> { }


    /// <summary>
    /// Data members.
    /// </summary>
    private static Manager _instance = null;
    private CadKit.Threads.Tools.Lock _lock = new CadKit.Threads.Tools.Lock();
    private Documents _documents = new Documents();
    private CadKit.Interfaces.IDocument _activeDoc = null;
    private CadKit.Interfaces.IDocumentView _activeView = null;
    private ActiveDocumentChangedDelegate _activeDocumentChanged = null;
    private ActiveViewChangedDelegate _activeViewChanged = null;


    /// <summary>
    /// Constructor
    /// </summary>
    private Manager()
    {
    }


    /// <summary>
    /// Single instance.
    /// </summary>
    public static Manager Instance
    {
      get
      {
        lock ("CadKit.Documents.Instance")
        {
          if (null == _instance)
            _instance = new Manager();
          return _instance;
        }
      }
    }


    /// <summary>
    /// Open the document. Either returns a document or throws.
    /// </summary>
    public CadKit.Interfaces.IDocument open(string name, ChooseBestOpenerDelegate del, object caller)
    {
      // Re-entrant! Do not lock the mutex!

      // Look for existing document with this name.
      CadKit.Interfaces.IDocument document = this.findDocument(name);
      if (null != document)
      {
        this.windowsForward(document, caller);
        return document;
      }

      // Determine the opener that handles this extension.
      CadKit.Interfaces.IDocumentOpen opener = this._findBestOpener(name, del, caller);
      if (null == opener)
      {
        System.IO.FileInfo info = new System.IO.FileInfo(name);
        throw new System.Exception("Error 4027629339: Failed to find plugin that opens files with extension '" + info.Extension + "'");
      }

      // Open the document. This may throw.
      document = (CadKit.Interfaces.IDocument)(opener.open(name, caller));

      // If we get to here then add the new document.
      this._addDocument(document);

      // Return new document.
      return document;
    }


    /// <summary>
    /// Remove the document.
    /// </summary>
    public void remove(CadKit.Interfaces.IDocument document)
    {
      if (null != document)
      {
        if (document == this.ActiveDocument)
        {
          // This also sets active document to null.
          this.ActiveView = null;
        }
        using (this.Lock.write())
        {
          while (true == _documents.Remove(document)) { }
        }
      }
    }


    /// <summary>
    /// Add the document.
    /// </summary>
    private void _addDocument(CadKit.Interfaces.IDocument document)
    {
      if (null != document)
      {
        using (this.Lock.write())
        {
          _documents.Add(document);
        }
      }
    }


    /// <summary>
    /// Bring this document's windows to the front.
    /// </summary>
    public void windowsForward(CadKit.Interfaces.IDocument doc, object caller)
    {
      // Re-entrant! Do not lock the mutex! This is a convenience function 
      // that does not alter any members.

      CadKit.Interfaces.IWindowsForward forward = doc as CadKit.Interfaces.IWindowsForward;
      if (null != forward)
      {
        forward.windowsForward(caller);
      }
    }


    /// <summary>
    /// Find the document given the name.
    /// </summary>
    public CadKit.Interfaces.IDocument findDocument(string name)
    {
      // Re-entrant! Do not lock the mutex!
      try
      {
        Documents docs = this.DocumentsCopy;
        foreach (CadKit.Interfaces.IDocument doc in docs)
        {
          if (null != doc && doc.Name == name)
          {
            return doc;
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1730402887: {0}", e.Message);
      }
      return null;
    }


    /// <summary>
    /// Find all plugins that open files with this extension.
    /// </summary>
    private CadKit.Interfaces.IDocumentOpen[] _findOpeners(string name)
    {
      // Re-entrant! Do not lock the mutex! When opening plugins they may 
      // call back to this class and try to lock the mutex.

      Openers openers = new Openers();
      try
      {
        CadKit.Interfaces.IDocumentOpen[] all = CadKit.Plugins.Manager.Instance.getAll<CadKit.Interfaces.IDocumentOpen>();
        System.IO.FileInfo info = new System.IO.FileInfo(name);
        foreach (CadKit.Interfaces.IDocumentOpen opener in all)
        {
          try
          {
            if (true == opener.canOpen(info.Extension))
            {
              openers.Add(opener);
            }
          }
          catch (System.Exception e)
          {
            System.Console.WriteLine("Error 2974600949: {0}", e.Message);
          }
        }
      }
      catch (System.Exception f)
      {
        System.Console.WriteLine("Error 3030701565: {0}", f.Message);
      }
      return openers.ToArray();
    }


    /// <summary>
    /// Determine appropriate opener. Pass null for the delegate to get first one.
    /// </summary>
    private CadKit.Interfaces.IDocumentOpen _findBestOpener(string name, ChooseBestOpenerDelegate del, object caller)
    {
      // Re-entrant! Do not lock the mutex! When opening plugins they may 
      // call back to this class and try to lock the mutex.

      CadKit.Interfaces.IDocumentOpen[] openers = this._findOpeners(name);
      if (null == openers || 0 == openers.Length)
        return null;
      return (null != del) ? openers[del(openers, caller)] : openers[0];
    }


    /// <summary>
    /// Return the number of documents.
    /// </summary>
    public int NumDocuments
    {
      get { using (this.Lock.read()) { return _documents.Count; } }
    }


    /// <summary>
    /// Get the active document.
    /// </summary>
    public CadKit.Interfaces.IDocument ActiveDocument
    {
      get { using (this.Lock.read()) { return _activeDoc; } }
    }


    /// <summary>
    /// Set/Get the active view.
    /// </summary>
    public CadKit.Interfaces.IDocumentView ActiveView
    {
      get { using (this.Lock.read()) { return _activeView; } }
      set { this._setActiveView(value); }
    }


    /// <summary>
    /// Set the active view.
    /// </summary>
    private void _setActiveView(CadKit.Interfaces.IDocumentView value)
    {
      CadKit.Interfaces.IDocumentView oldView = this.ActiveView;

      using (this.Lock.write())
      {
        _activeView = value;
      }

      this._setActiveDocument();

      if (null != this.ActiveViewChanged)
      {
        this.ActiveViewChanged(oldView, this.ActiveView);
      }
    }


    /// <summary>
    /// Set the active document.
    /// </summary>
    private void _setActiveDocument()
    {
      CadKit.Interfaces.IDocumentView activeView = this.ActiveView;
      CadKit.Interfaces.IDocument oldDoc = this.ActiveDocument;

      using (this.Lock.write())
      {
        _activeDoc = (null == activeView) ? null : activeView.Document;
      }

      if (null != this.ActiveDocumentChanged)
      {
        this.ActiveDocumentChanged(oldDoc, this.ActiveDocument);
      }
    }


    /// <summary>
    /// Get the active document changed delegate.
    /// </summary>
    public ActiveDocumentChangedDelegate ActiveDocumentChanged
    {
      get { using (this.Lock.read()) { return _activeDocumentChanged; } }
      set { using (this.Lock.write()) { _activeDocumentChanged = value; } }
    }


    /// <summary>
    /// Get the active view changed delegate.
    /// </summary>
    public ActiveViewChangedDelegate ActiveViewChanged
    {
      get { using (this.Lock.read()) { return _activeViewChanged; } }
      set { using (this.Lock.write()) { _activeViewChanged = value; } }
    }


    /// <summary>
    /// Hook up document with appropriate delegate.
    /// </summary>
    public void setGuiDelegate(CadKit.Interfaces.IDocument doc, object caller)
    {
      if (null != doc)
      {
        // Look for delegate.
        CadKit.Interfaces.IGuiDelegateCreate[] creators = CadKit.Plugins.Manager.Instance.getAll<CadKit.Interfaces.IGuiDelegateCreate>();
        foreach (CadKit.Interfaces.IGuiDelegateCreate creator in creators)
        {
          CadKit.Interfaces.IGuiDelegate gui = ((null != creator) ? (creator.create(caller) as CadKit.Interfaces.IGuiDelegate) : null);
          if (null != gui && true == gui.doesHandle(doc.TypeName))
          {
            doc.GuiDelegate = gui;
            gui.Document = doc;
          }
        }
      }
    }


    /// <summary>
    /// Get the lock.
    /// </summary>
    public CadKit.Threads.Tools.Lock Lock
    {
      get { return _lock; }
    }


    /// <summary>
    /// Get a copy of the documents container.
    /// </summary>
    private Documents DocumentsCopy
    {
      get
      {
        Documents docs = new Documents();
        using (this.Lock.read())
        {
          docs.AddRange(_documents);
        }
        return docs;
      }
    }
  }
}
