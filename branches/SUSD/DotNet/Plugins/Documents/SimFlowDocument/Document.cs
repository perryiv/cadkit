
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Aashish Chaudhary
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Documents.SimFlowDocument
{
  public class Document :
    CadKit.Documents.Document,
    CadKit.Interfaces.IFileOpen,
    CadKit.Interfaces.IBuildScene,
    CadKit.Interfaces.IUpdateScene
  {
    /// <summary>
    /// Root for the scene.
    /// </summary>
    private CadKit.OSG.Glue.Node _root = null;
    SimFlowGlue _sfGl = null;

    /// <summary>
    /// Constants
    /// </summary>
    public const string TypeName = "SimFlow Document";

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
      return CadKit.Plugins.Documents.SimFlowDocument.Document.TypeName;
    }

    /// <summary>
    /// Read the file.
    /// </summary>
    void CadKit.Interfaces.IFileOpen.open(string name, object caller)
    {
      using (this.Lock.write())
      {
        // read file and create scene here.
        _sfGl = new SimFlowGlue(name, caller);
        _root = _sfGl.root();
      }
    }


    /// <summary>
    /// Update the scene.
    /// </summary>
    public void updateScene()
    {
      _sfGl.update();
    }


    /// <summary>
    /// Get the scene.
    /// </summary>
    object CadKit.Interfaces.IBuildScene.Scene
    {
      get { using (this.Lock.read()) { return _root; } }
    }
  }
}
