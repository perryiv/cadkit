
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Documents.OSSIMPlanetDocument
{
  public class Document :
    CadKit.Documents.Document,
    CadKit.Interfaces.IRead,
    CadKit.Interfaces.IBuildScene,
    CadKit.Interfaces.IImageLayer
  {
    /// <summary>
    /// Root for the scene.
    /// </summary>
    private CadKit.OSG.Glue.Node _root = null;
    private CadKit.Plugins.Documents.OSSIMPlanetDocument.Glue.OssimPlanet _ossimPlanet = null;

    public CadKit.Plugins.Documents.OSSIMPlanetDocument.Glue.OssimPlanet OssimPlanet
    {
      get { return _ossimPlanet; }
      set { _ossimPlanet = value; }
    }

    /// <summary>
    /// Constants
    /// </summary>
    public const string TypeName = "OSSIMPlanet Document";

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
      return CadKit.Plugins.Documents.OSSIMPlanetDocument.Document.TypeName;
    }

    /// <summary>
    /// Read the file.
    /// </summary>
    void CadKit.Interfaces.IRead.read(string name, object caller)
    {
      using (this.Lock.write())
      {
        _ossimPlanet = new CadKit.Plugins.Documents.OSSIMPlanetDocument.Glue.OssimPlanet(name);
        _root = _ossimPlanet.root();

        // Add file to recent-file list.
        CadKit.Interfaces.IRecentFileList recent = caller as CadKit.Interfaces.IRecentFileList;
        if (null != recent)
        {
          recent.add(name);
        }
      }
    }


    /// <summary>
    /// Get the scene.
    /// </summary>
    object CadKit.Interfaces.IBuildScene.Scene
    {
      get { using (this.Lock.read()) { return _root; } }
    }


    /// <summary>
    /// 
    /// </summary>
    void CadKit.Interfaces.IImageLayer.addImageLayer(string filename)
    {
      _ossimPlanet.addImageLayer(filename);
    }


    /// <summary>
    /// 
    /// </summary>
    void CadKit.Interfaces.IImageLayer.hideImageLayer(string filename)
    {
      _ossimPlanet.hideImageLayer(filename);
    }


    /// <summary>
    /// 
    /// </summary>
    void CadKit.Interfaces.IImageLayer.showImageLayer(string filename)
    {
      _ossimPlanet.showImageLayer(filename);
    }


    /// <summary>
    /// 
    /// </summary>
    void CadKit.Interfaces.IImageLayer.removeImageLayer(string filename)
    {
    }
  }
}
