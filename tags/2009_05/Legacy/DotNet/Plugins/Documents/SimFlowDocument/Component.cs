
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Documents.SimFlowDocument
{
  public class Component :
    CadKit.Referenced.Base,
    CadKit.Interfaces.IPlugin,
    CadKit.Interfaces.IDocumentNew,
    CadKit.Interfaces.IDocumentOpen,
    CadKit.Interfaces.IFiltersOpen
  {
    /// <summary>
    /// Construct a component.
    /// </summary>
    private object _mutex = new object();

    /// <summary>
    /// Construct a component.
    /// </summary>
    public Component()
    {
    }

    /// <summary>
    /// Called when the plugin is loaded.
    /// </summary>
    void CadKit.Interfaces.IPlugin.start(object caller)
    {
      lock (_mutex)
      {
      }
    }

    void CadKit.Interfaces.IPlugin.finish(object caller)
    {
    }

    /// <summary>
    /// Create a new document.
    /// </summary>
    object CadKit.Interfaces.IDocumentNew.create(object caller)
    {
      // Re-entrant! Do not lock the mutex!
      return new CadKit.Plugins.Documents.SimFlowDocument.Document();
    }

    /// <summary>
    /// Open a document.
    /// </summary>
    object CadKit.Interfaces.IDocumentOpen.open(string file, object caller)
    {
      // Re-entrant! Do not lock the mutex!
      CadKit.Interfaces.IDocumentNew creator = (CadKit.Interfaces.IDocumentNew)this;
      CadKit.Interfaces.IFileOpen reader = (CadKit.Interfaces.IFileOpen)(creator.create(caller));
      reader.open(file, caller);
      return reader;
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Name
    {
      // Re-entrant! Do not lock the mutex!
      get { return CadKit.Plugins.Documents.SimFlowDocument.Document.TypeName; }
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Description
    {
      // Re-entrant! Do not lock the mutex!
      get { return "Document for working with SimFlow data"; }
    }

    /// <summary>
    /// See if we can open this document.
    /// </summary>
    bool CadKit.Interfaces.IDocumentOpen.canOpen(string name)
    {
      // Re-entrant! Do not lock the mutex!
      System.IO.FileInfo info = new System.IO.FileInfo(name);
      string ext = info.Extension.ToLower();
      return (".sfd" == ext);
    }

    /// <summary>
    /// Return the filters.
    /// </summary>
    CadKit.Interfaces.Filters CadKit.Interfaces.IFiltersOpen.Filters
    {
      // Re-entrant! Do not lock the mutex!
      get
      {
        CadKit.Interfaces.Filters filters = new CadKit.Interfaces.Filters();
        filters.Add(new CadKit.Interfaces.Filter("SimFlow Document (*.sfd)", "*.sfd"));
        return filters;
      }
    }
  }
}
