
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Documents.SceneDocument
{
  public class Document : 
    CadKit.Documents.Document,
    CadKit.Interfaces.IRead,
    CadKit.Interfaces.IBuildScene
  {
    /// <summary>
    /// Root for the scene.
    /// </summary>
    private CadKit.OSG.Glue.Node _root = null;

    /// <summary>
    /// Constants
    /// </summary>
    public const string TypeName = "Scene Document";

    /// <summary>
    /// Construct a document.
    /// </summary>
    public Document()
    {
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    protected override string _typeName()
    {
      lock (_mutex) { return CadKit.Plugins.Documents.SceneDocument.Document.TypeName; }
    }

    /// <summary>
    /// Read the file.
    /// </summary>
    void CadKit.Interfaces.IRead.read ( string name, object caller )
    {
      _root = CadKit.OSG.Glue.ReadFile.readNodeFile(name);

      CadKit.Interfaces.IDocument idoc = this as CadKit.Interfaces.IDocument;
      if (null != idoc)
      {
        idoc.Name = name;
      }
    }


    /// <summary>
    /// Get the scene.
    /// </summary>
    object CadKit.Interfaces.IBuildScene.Scene
    {
      get { return _root; }
    }
  }
}
