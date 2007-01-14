
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
    CadKit.Interfaces.IFileOpen,
    CadKit.Interfaces.IBuildScene,
    CadKit.Interfaces.ILayerList
  {
    /// <summary>
    /// Root for the scene.
    /// </summary>
    private CadKit.OSG.Glue.Node _root = null;
    private CadKit.Plugins.Documents.OSSIMPlanetDocument.Glue.OssimPlanet _ossimPlanet = new CadKit.Plugins.Documents.OSSIMPlanetDocument.Glue.OssimPlanet();

    private System.Collections.Generic.List<CadKit.Interfaces.ILayer> _layers = new System.Collections.Generic.List<CadKit.Interfaces.ILayer>();

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
      _root = _ossimPlanet.root();
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
    public void open(string name, object caller)
    {
      using (this.Lock.write())
      {
        _ossimPlanet.addKeyWordList (name);

        // Set document name.
        this.Name = name;

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
    CadKit.Interfaces.ILayer[] CadKit.Interfaces.ILayerList.Layers 
    {
      get
      {
        return _layers.ToArray();
      }
    }

    void CadKit.Interfaces.ILayerList.addLayer(CadKit.Interfaces.ILayer layer, object caller)
    {
      _ossimPlanet.addLayer(layer as CadKit.OSSIMPlanet.Glue.ImageLayer);
      layer.show();
      _layers.Add(layer);
    }

    void CadKit.Interfaces.ILayerList.modifyLayer(CadKit.Interfaces.ILayer layer, object caller)
    {
    }

    void CadKit.Interfaces.ILayerList.hideLayer(CadKit.Interfaces.ILayer layer, object caller)
    {
    }

    void CadKit.Interfaces.ILayerList.showLayer(CadKit.Interfaces.ILayer layer, object caller)
    {
    }

    void CadKit.Interfaces.ILayerList.removeLayer(CadKit.Interfaces.ILayer layer, object caller)
    {
      _ossimPlanet.removeLayer(layer as CadKit.OSSIMPlanet.Glue.ImageLayer);
      _layers.Remove(layer);
    }
  }
}
