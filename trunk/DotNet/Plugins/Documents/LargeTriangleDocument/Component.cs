
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.LargeTriangleDocument
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
      //System.Threading.Thread.Sleep(new System.TimeSpan(0, 0, 2));
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
    object CadKit.Interfaces.IDocumentNew.createNewDocument(object caller)
    {
      lock (_mutex)
      {
        return new CadKit.LargeTriangleDocument.Document();
      }
    }

    /// <summary>
    /// Open a document.
    /// </summary>
    object CadKit.Interfaces.IDocumentOpen.openDocument(string file, object caller)
    {
      lock (_mutex)
      {
        CadKit.Interfaces.IDocumentNew creator = (CadKit.Interfaces.IDocumentNew)this;
        CadKit.Interfaces.IRead reader = (CadKit.Interfaces.IRead)(creator.createNewDocument(caller));
        reader.read(file, caller);
        return reader;
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
          filters.Add(new CadKit.Interfaces.Filter("All Triangle Files (*.tdf *.stl *.r3d)", "*.tdf;*.stl;*.r3d"));
          filters.Add(new CadKit.Interfaces.Filter("Triangle Document Format (*.tdf)", "*.tdf"));
          filters.Add(new CadKit.Interfaces.Filter("Stereolithography (*.stl)", "*.stl"));
          filters.Add(new CadKit.Interfaces.Filter("RoboMet 3D (*.r3d)", "*.r3d"));
          return filters;
        }
      }
    }
  }
}
