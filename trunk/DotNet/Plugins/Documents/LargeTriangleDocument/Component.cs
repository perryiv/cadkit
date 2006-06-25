
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
    CadKit.Interfaces.IDocumentOpen
  {
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
    }

    /// <summary>
    /// Create a new document.
    /// </summary>
    object CadKit.Interfaces.IDocumentNew.createNewDocument(object caller)
    {
      return new CadKit.LargeTriangleDocument.Document();
    }

    /// <summary>
    /// Open a document.
    /// </summary>
    object CadKit.Interfaces.IDocumentOpen.openDocument(string file, object caller)
    {
      CadKit.Interfaces.IDocumentNew creator = (CadKit.Interfaces.IDocumentNew) this;
      CadKit.Interfaces.IRead reader = (CadKit.Interfaces.IRead) (creator.createNewDocument(caller));
      reader.read(file, caller);
      return reader;
    }
  }
}
