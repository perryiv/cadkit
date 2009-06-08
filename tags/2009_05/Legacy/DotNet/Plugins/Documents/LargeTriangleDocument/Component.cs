
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Documents.LargeTriangleDocument
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
    private CadKit.Threads.Tools.Lock _lock = new CadKit.Threads.Tools.Lock();


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
      using (_lock.write())
      {
      }
    }


    /// <summary>
    /// Called when use of the plugin is finished.
    /// </summary>
    void CadKit.Interfaces.IPlugin.finish(object caller)
    {
      using (_lock.write())
      {
      }
    }


    /// <summary>
    /// Create a new document.
    /// </summary>
    object CadKit.Interfaces.IDocumentNew.create(object caller)
    {
      return new CadKit.Plugins.Documents.LargeTriangleDocument.Document();
    }


    /// <summary>
    /// Open a document.
    /// </summary>
    object CadKit.Interfaces.IDocumentOpen.open(string file, object caller)
    {
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
      get { return CadKit.Plugins.Documents.LargeTriangleDocument.Document.TypeName; }
    }


    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Description
    {
      get { return "Document type for working with triangle document that are larger than system RAM"; }
    }


    /// <summary>
    /// See if we can open this document.
    /// </summary>
    bool CadKit.Interfaces.IDocumentOpen.canOpen(string name)
    {
      System.IO.FileInfo info = new System.IO.FileInfo(name);
      string ext = info.Extension.ToLower();
      return (".tdf" == ext || ".stl" == ext || ".r3d" == ext);
    }


    /// <summary>
    /// Return open-filters.
    /// </summary>
    CadKit.Interfaces.Filters CadKit.Interfaces.IFiltersOpen.Filters
    {
      get { return CadKit.Plugins.Documents.LargeTriangleDocument.Document.FiltersOpen; }
    }


    /// <summary>
    /// Return the lock
    /// </summary>
    protected CadKit.Threads.Tools.Lock Lock
    {
      get { return _lock; }
    }
  }
}
