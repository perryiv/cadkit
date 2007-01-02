
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Documents.OSSIMPlanetDocument
{
  public class Component :
    CadKit.Interfaces.IPlugin,
    CadKit.Interfaces.IDocumentNew,
    CadKit.Interfaces.IDocumentOpen,
    CadKit.Interfaces.IFiltersOpen,
    CadKit.Interfaces.IGuiDelegateCreate
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

    /// <summary>
    /// Called when use of the plugin is finished.
    /// </summary>
    void CadKit.Interfaces.IPlugin.finish(object caller)
    {
      lock (_mutex)
      {
      }
    }

    /// <summary>
    /// Create a new document.
    /// </summary>
    object CadKit.Interfaces.IDocumentNew.create(object caller)
    {
      // Re-entrant! Do not lock the mutex!
      return new CadKit.Plugins.Documents.OSSIMPlanetDocument.Document();
    }

    /// <summary>
    /// Open a document.
    /// </summary>
    object CadKit.Interfaces.IDocumentOpen.open(string file, object caller)
    {
      // Re-entrant! Do not lock the mutex!
      CadKit.Interfaces.IDocumentNew creator = (CadKit.Interfaces.IDocumentNew)this;
      CadKit.Interfaces.IRead reader = (CadKit.Interfaces.IRead)(creator.create(caller));
      reader.read(file, caller);
      return reader;
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Name
    {
      // Re-entrant! Do not lock the mutex!
      get { return CadKit.Plugins.Documents.OSSIMPlanetDocument.Document.TypeName; }
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Description
    {
      // Re-entrant! Do not lock the mutex!
      get { return "Document type for working with OSSIM Planet"; }
    }

    /// <summary>
    /// See if we can open this document.
    /// </summary>
    bool CadKit.Interfaces.IDocumentOpen.canOpen(string name)
    {
      // Re-entrant! Do not lock the mutex!
      System.IO.FileInfo info = new System.IO.FileInfo(name);
      string ext = info.Extension.ToLower();
      return (".kwl" == ext);
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
        filters.Add(new CadKit.Interfaces.Filter("OSSIM Key Word List (*.kwl)", "*.kwl"));
        return filters;
      }
    }

    /// <summary>
    /// Create new gui-delegate.
    /// </summary>
    object CadKit.Interfaces.IGuiDelegateCreate.create(object caller)
    {
      return new CadKit.Plugins.Documents.OSSIMPlanetDocument.Delegate();
    }
  }
}
