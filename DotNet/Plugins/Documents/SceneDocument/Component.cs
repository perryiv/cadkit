
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
    void CadKit.Interfaces.IPlugin.startupNotify(object caller)
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
      lock (_mutex)
      {
        return new CadKit.Plugins.Documents.SceneDocument.Document();
      }
    }

    /// <summary>
    /// Open a document.
    /// </summary>
    object CadKit.Interfaces.IDocumentOpen.open(string file, object caller)
    {
      lock (_mutex)
      {
        CadKit.Interfaces.IDocumentNew creator = (CadKit.Interfaces.IDocumentNew)this;
        CadKit.Interfaces.IRead reader = (CadKit.Interfaces.IRead)(creator.create(caller));
        reader.read(file, caller);
        return reader;
      }
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Name
    {
      get { lock (_mutex) { return CadKit.Plugins.Documents.SceneDocument.Document.TypeName; } }
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Description
    {
      get { lock (_mutex) { return "Document type for working with a scene"; } }
    }

    /// <summary>
    /// See if we can open this document.
    /// </summary>
    bool CadKit.Interfaces.IDocumentOpen.canOpen(string name)
    {
      lock (_mutex)
      {
        System.IO.FileInfo info = new System.IO.FileInfo(name);
        string ext = info.Extension.ToLower();
        return (".osg" == ext || ".ive" == ext || ".flt" == ext);
      }
    }

    /// <summary>
    /// Return the filters.
    /// </summary>
    CadKit.Interfaces.Filters CadKit.Interfaces.IFiltersOpen.Filters
    {
      get
      {
        lock (_mutex)
        {
          CadKit.Interfaces.Filters filters = new CadKit.Interfaces.Filters();
          filters.Add(new CadKit.Interfaces.Filter("All Scene Files (*.osg *.ive *.flt)", "*.osg;*.ive;*.flt"));
          filters.Add(new CadKit.Interfaces.Filter("OpenSceneGraph ASCII (*.osg)", "*.osg"));
          filters.Add(new CadKit.Interfaces.Filter("OpenSceneGraph Binary (*.ive)", "*.ive"));
          filters.Add(new CadKit.Interfaces.Filter("OpenFlight (*.flt)", "*.flt"));
          return filters;
        }
      }
    }
  }
}
