
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.Document
{
  public class Component :
    CadKit.Referenced.Base,
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
    }


    /// <summary>
    /// Called when the plugin is finished
    ///  </summary>
    void CadKit.Interfaces.IPlugin.finish(object caller)
    {
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Name
    {
      get { lock (_mutex) { return DT.Minerva.Plugins.Document.Document.TypeName; } }
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Description
    {
      get { return "Document to control hiding and showing of layers"; }
    }

    /// <summary>
    /// Create new gui-delegate.
    /// </summary>
    object CadKit.Interfaces.IGuiDelegateCreate.create(object caller)
    {
      return new DT.Minerva.Plugins.Document.Delegate();
    }

    /// <summary>
    /// Create a new document.
    /// </summary>
    object CadKit.Interfaces.IDocumentNew.create(object caller)
    {
      // Re-entrant! Do not lock the mutex!
      return new DT.Minerva.Plugins.Document.Document();
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
    /// See if we can open this document.
    /// </summary>
    bool CadKit.Interfaces.IDocumentOpen.canOpen(string name)
    {
      // Re-entrant! Do not lock the mutex!
      System.IO.FileInfo info = new System.IO.FileInfo(name);
      string ext = info.Extension.ToLower();
      return (".kwl" == ext) || (".minerva" == ext);
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
        filters.Add(new CadKit.Interfaces.Filter("Minerva Document (*.minerva)", "*.minerva"));
        return filters;
      }
    }
  }
}
