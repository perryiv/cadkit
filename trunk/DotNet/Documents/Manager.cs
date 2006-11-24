
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
    /// Delegate to choose which opener to use.
    /// </summary>
    public delegate int ChooseBestOpener(CadKit.Interfaces.IDocumentOpen[] openers, object caller);

    /// <summary>
    /// Open the document. Either returns a document or throws.
    /// </summary>
    public CadKit.Interfaces.IDocument open(string name, ChooseBestOpener del, object caller)
    {
      lock (_mutex)
      {
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
        document = opener.open(name, caller) as CadKit.Interfaces.IDocument;

        // If we get to here then add the new document.
        _documents.Add(document);

        // Return new document.
        return document;
      }
    }

    /// <summary>
    /// Bring this document's windows to the front.
    /// </summary>
    public void windowsForward(CadKit.Interfaces.IDocument doc, object caller)
    {
      lock (_mutex)
      {
        CadKit.Interfaces.IWindowsForward forward = doc as CadKit.Interfaces.IWindowsForward;
        if (null != forward)
        {
          forward.windowsForward(caller);
        }
      }
    }

    /// <summary>
    /// Find the document given the name.
    /// </summary>
    public CadKit.Interfaces.IDocument findDocument(string name)
    {
      lock (_mutex)
      {
        try
        {
          foreach (CadKit.Interfaces.IDocument doc in _documents)
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
    }

    /// <summary>
    /// Find all plugins that open files with this extension.
    /// </summary>
    protected CadKit.Interfaces.IDocumentOpen[] _findOpeners(string name)
    {
      lock (_mutex)
      {
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
    }

    /// <summary>
    /// Determine appropriate opener. Pass null for the delegate to get first one.
    /// </summary>
    protected CadKit.Interfaces.IDocumentOpen _findBestOpener(string name, ChooseBestOpener del, object caller)
    {
      lock (_mutex)
      {
        CadKit.Interfaces.IDocumentOpen[] openers = this._findOpeners(name);
        if (null == openers || 0 == openers.Length)
          return null;
        return (null != del) ? openers[del(openers, caller)] : openers[0];
      }
    }

    /// <summary>
    /// Return the number of documents.
    /// </summary>
    public int NumDocuments
    {
      get { lock (_mutex) { return _documents.Count; } }
    }

    /// <summary>
    /// Get the active document.
    /// </summary>
    public CadKit.Interfaces.IDocument ActiveDocument
    {
      get { lock (_mutex) { return _activeDoc; } }
    }

    /// <summary>
    /// Set/Get the active view.
    /// </summary>
    public object ActiveView
    {
      get { lock (_mutex) { return _activeView; } }
      set
      {
        lock (_mutex)
        {
          _activeView = value;
          CadKit.Interfaces.IDocumentView view = _activeView as CadKit.Interfaces.IDocumentView;
          if (null != view)
          {
            _activeDoc = view.Document;
          }
        }
      }
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
    /// Local types.
    /// </summary>
    class Documents : System.Collections.Generic.List<CadKit.Interfaces.IDocument> { }
    class Openers : System.Collections.Generic.List<CadKit.Interfaces.IDocumentOpen> { }

    /// <summary>
    /// Data members.
    /// </summary>
    private static Manager _instance = null;
    private object _mutex = new object();
    private Documents _documents = new Documents();
    private static CadKit.Interfaces.IDocument _activeDoc = null;
    private static object _activeView = null;
  }
}
