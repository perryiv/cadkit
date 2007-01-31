
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.Document
{
  public class Component :
    CadKit.Interfaces.IPlugin,
    CadKit.Interfaces.IDocumentNew,
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
  }
}
