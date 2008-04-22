
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Documents.SceneDocument
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
    }


    /// <summary>
    /// Called when use of the plugin is finished.
    /// </summary>
    void CadKit.Interfaces.IPlugin.finish(object caller)
    {
    }


    /// <summary>
    /// Create a new document.
    /// </summary>
    object CadKit.Interfaces.IDocumentNew.create(object caller)
    {
      return new CadKit.Plugins.Documents.SceneDocument.Document();
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
      get { return CadKit.Plugins.Documents.SceneDocument.Document.TypeName; }
    }


    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Description
    {
      get { return "Document type for working with a scene"; }
    }


    /// <summary>
    /// See if we can open this document.
    /// </summary>
    bool CadKit.Interfaces.IDocumentOpen.canOpen(string name)
    {
      return CadKit.OSG.Glue.ReadFile.hasReader(name);
    }


    /// <summary>
    /// Return the open-filters.
    /// </summary>
    CadKit.Interfaces.Filters CadKit.Interfaces.IFiltersOpen.Filters
    {
      get { return CadKit.Plugins.Documents.SceneDocument.Document.FiltersOpen; }
    }
  }
}
