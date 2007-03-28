
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
  class Document :
    CadKit.Documents.Document,
    System.IDisposable,
    CadKit.Interfaces.IFileOpen,
    CadKit.Interfaces.IFileSave,
    CadKit.Interfaces.IFiltersSave,
    CadKit.Interfaces.ILayerList,
    CadKit.Interfaces.IAnimateTemporal,
    CadKit.Interfaces.IOssimPlanetSettings,
    CadKit.Interfaces.ILegend,
    CadKit.Interfaces.ILayerOperation, 
    CadKit.Interfaces.IMovieMode
  {
    /// <summary>
    /// Constants
    /// </summary>
    public const string TypeName = "Minerva Document";


    /// <summary>
    ///  Data Members
    /// </summary>
    private System.Collections.Generic.List<CadKit.Interfaces.ILayer> _layers = new System.Collections.Generic.List<CadKit.Interfaces.ILayer>();

    private DT.Minerva.Plugins.Document.Glue.DllGlue _dll = new DT.Minerva.Plugins.Document.Glue.DllGlue();
    private DT.Minerva.Plugins.Document.Glue.DistributedGlue _distributed = new DT.Minerva.Plugins.Document.Glue.DistributedGlue();

    private bool _useDll = true;
    private bool _useDistributed = false;


    /// <summary>
    /// Constructor.
    /// </summary>
    public Document()
      : base()
    {
      this._parseConfigFile();
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~Document()
    {
      this._cleanup();
    }


    /// <summary>
    /// Dispose.
    /// </summary>
    void System.IDisposable.Dispose()
    {
      this._cleanup();
    }


    /// <summary>
    /// Clean up.
    /// </summary>
    protected override void _cleanup()
    {
      try
      {
        if (this.Distributed && null != _distributed)
        {
          _distributed.deleteSession();
          _distributed.disconnect();
        }

        _dll = null;
        _distributed = null;
        base._cleanup();
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3258987650: {0}", e.Message);
      }
    }


    /// <summary>
    /// Read the file.
    /// </summary>
    public void open(string name, object caller)
    {
      string cwd = null;
      try
      {
        // Make the path the current working directory. 
        // This will increase the chances of textures loading.
        cwd = System.IO.Directory.GetCurrentDirectory();
        System.IO.FileInfo info = new System.IO.FileInfo(name);
        System.IO.Directory.SetCurrentDirectory(info.DirectoryName);

        // Read the file.
        _dll.open(name);

        // Set document name.
        this.Name = name;
        this.HasDefaultName = false;

        // Add file to recent-file list.
        CadKit.Interfaces.IRecentFileList recent = caller as CadKit.Interfaces.IRecentFileList;
        if (null != recent)
        {
          recent.add(name);
        }
      }
      finally
      {
        // Always reset the current working directory.
        if (null != cwd && System.IO.Directory.Exists(cwd))
        {
          System.IO.Directory.SetCurrentDirectory(cwd);
        }
      }
    }


    /// <summary>
    /// Save the document.
    /// </summary>
    public void save(object caller)
    {
      _dll.save();
      this.Modified = false;
    }


    /// <summary>
    /// Save the document as the new filename.
    /// </summary>
    public void saveAs(string filename, object caller)
    {
      this.Name = filename;
      this.HasDefaultName = false;

      _dll.saveAs(filename);
      this.Modified = false;
    }


    /// <summary>
    /// Return the filters.
    /// </summary>
    CadKit.Interfaces.Filters CadKit.Interfaces.IFiltersSave.Filters
    {
      get
      {
        CadKit.Interfaces.Filters filters = new CadKit.Interfaces.Filters();
        filters.Add(new CadKit.Interfaces.Filter("Minerva Document (*.minerva)", "*.minerva"));
        return filters;
      }
    }


    /// <summary>
    /// Connect to the session.
    /// </summary>
    public void connectToSession(string name)
    {
      _distributed.connectToSession(name);

#if FALSE
      string message = "Do you want to launch a instance of the viz client locally?";
      string caption = "Launch viz client?";
      System.Windows.Forms.MessageBoxButtons buttons = System.Windows.Forms.MessageBoxButtons.YesNo;
      System.Windows.Forms.MessageBoxIcon icon = System.Windows.Forms.MessageBoxIcon.Question;

      // Displays the MessageBox.
      System.Windows.Forms.DialogResult result = System.Windows.Forms.MessageBox.Show(message, caption, buttons, icon);

      if (result == System.Windows.Forms.DialogResult.Yes)
      {
#if DEBUG
        System.Diagnostics.Process.Start("wnv_viz_d.exe", name);
#else
        System.Diagnostics.Process.Start("wnv_viz.exe", name);
#endif
      }
#endif
    }


    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    protected override string _typeName()
    {
      return DT.Minerva.Plugins.Document.Document.TypeName;
    }


    /// <summary>
    /// Set the viewer
    /// </summary>
    public CadKit.Viewer.Glue.Viewer Viewer
    {
      set
      {
        _dll.viewer(value);
      }
    }


    /// <summary>
    /// resize the window.
    /// </summary>
    public void resize(object sender, System.EventArgs e)
    {
      System.Windows.Forms.Form form = sender as System.Windows.Forms.Form;
      if (null != form)
      {
        _dll.resize(form.Size.Width, form.Size.Height);
      }
    }


    /// <summary>
    /// Get the layer list.
    /// </summary>
    CadKit.Interfaces.ILayer[] CadKit.Interfaces.ILayerList.Layers
    {
      get
      {
        return _layers.ToArray();
      }
    }


    /// <summary>
    /// Add a layer
    /// </summary>
    void CadKit.Interfaces.ILayerList.addLayer(CadKit.Interfaces.ILayer layer, object caller)
    {
      if (null != layer)
      {
        layer.show();
        if (layer is CadKit.Interfaces.IOssimLayer)
          _dll.addLayer((CadKit.Interfaces.IOssimLayer)layer);
        else
          this._showLayer(layer);
        _layers.Add(layer);

        this.Modified = true;
      }
    }


    /// <summary>
    /// Modify the layer.
    /// </summary>
    void CadKit.Interfaces.ILayerList.modifyLayer(CadKit.Interfaces.ILayer layer, object caller)
    {
      if (null != layer)
      {
        layer.modify();
        this._modifyLayer(layer);

        this.Modified = true;
      }
    }


    /// <summary>
    /// Hide the layer.
    /// </summary>
    void CadKit.Interfaces.ILayerList.hideLayer(CadKit.Interfaces.ILayer layer, object caller)
    {
      if (null != layer)
      {
        _dll.hideLayer(layer);
        this.Modified = true;
      }
    }


    /// <summary>
    /// Show the layer.
    /// </summary>
    void CadKit.Interfaces.ILayerList.showLayer(CadKit.Interfaces.ILayer layer, object caller)
    {
      if (null != layer)
      {
        _dll.showLayer(layer);
        this.Modified = true;
      }
    }


    /// <summary>
    /// Remove the layer.
    /// </summary>
    void CadKit.Interfaces.ILayerList.removeLayer(CadKit.Interfaces.ILayer layer, object caller)
    {
      if (layer is CadKit.Interfaces.IOssimLayer)
        _dll.removeLayer((CadKit.Interfaces.IOssimLayer)layer);
      else
        this._removeLayer(layer);
      _layers.Remove(layer);
      this.Modified = true;
    }


    /// <summary>
    /// 
    /// </summary>
    void CadKit.Interfaces.ILayerList.viewLayerExtents(CadKit.Interfaces.ILayer layer)
    {
      _dll.viewLayerExtents(layer);
    }


    void CadKit.Interfaces.ILayerList.addToFavorites(CadKit.Interfaces.ILayer layer)
    {
      _dll.addToFavorites(layer);
      this.Modified = true;
    }

    CadKit.Interfaces.ILayer CadKit.Interfaces.ILayerList.createFavorite(string name)
    {
      return _dll.createFavorite(name);
    }


    string[] CadKit.Interfaces.ILayerList.Favorites
    {
      get { return _dll.Favorites; }
    }


    /// <summary>
    /// Remove the layer.
    /// </summary>
    protected void _removeLayer(CadKit.Interfaces.ILayer layer)
    {
      if (null != layer)
      {
        if (this.Dll)
        {
          _dll.removeLayer(layer);
        }
        if (this.Distributed)
        {
          _distributed.removeLayer(layer);
        }
      }
    }


    /// <summary>
    /// The job has ended.
    /// </summary>
    protected void _jobEnd(CadKit.Threads.Jobs.Job job)
    {
      if (job.Canceled)
      {
        // If it's a add layer job...
        DT.Minerva.Plugins.Document.AddLayerJob addLayerJob = job as DT.Minerva.Plugins.Document.AddLayerJob;
        if (null != addLayerJob)
          addLayerJob._removeLayer(job);
      }
    }


    /// <summary>
    /// Show the layer.
    /// </summary>
    protected void _showLayer(CadKit.Interfaces.ILayer layer)
    {
      if (null != layer)
      {
        if (this.Dll)
        {
          DT.Minerva.Plugins.Document.AddLayerJob job = new DT.Minerva.Plugins.Document.AddLayerJob(layer, _dll);
          job.Name = "Adding " + layer.Name;
          job.Start += job._showLayer;
          job.Finish += this._jobEnd;
          CadKit.Threads.Jobs.Manager.Instance.add(job);
        }
        if (this.Distributed)
        {
          _distributed.showLayer(layer);
        }
      }
    }


    /// <summary>
    /// Modify the layer.
    /// </summary>
    protected void _modifyLayer(CadKit.Interfaces.ILayer layer)
    {
      if (null != layer)
      {
        if (this.Dll)
        {
          DT.Minerva.Plugins.Document.ModifyLayerJob job = new DT.Minerva.Plugins.Document.ModifyLayerJob(layer, _dll);
          job.Name = "Modifying " + layer.Name;
          job.Start += job._showLayer;
          job.Finish += this._jobEnd;
          CadKit.Threads.Jobs.Manager.Instance.add(job);
        }
        if (this.Distributed)
        {
          _distributed.showLayer(layer);
        }
      }
    }


    CadKit.Interfaces.AnimateTimestep CadKit.Interfaces.IAnimateTemporal.TimestepType
    {
      get { return _dll.timestepType(); }
      set { _dll.timestepType(value); }
    }


    /// <summary>
    /// Start animation.
    /// </summary>
    void CadKit.Interfaces.IAnimateTemporal.startAnimation(float speed, bool accumulate, bool timeWindow, int numDays)
    {
      try
      {
        this._startAnimation(speed, accumulate, timeWindow, numDays);
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2899159884: {0}", e.Message);
      }
    }


    /// <summary>
    /// Stop animation.
    /// </summary>
    void CadKit.Interfaces.IAnimateTemporal.stopAnimation()
    {
      try
      {
        this._stopAnimation();
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3428066134: {0}", e.Message);
      }
    }


    /// <summary>
    /// Start the animation.
    /// </summary>
    protected void _startAnimation(float speed, bool accumulate, bool timeWindow, int numDays)
    {
      if (this.Dll)
      {
        CadKit.Interfaces.IRenderLoop renderLoop = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IRenderLoop;
        if (null != renderLoop)
        {
          renderLoop.UseRenderLoop = true;
          _dll.startAnimation(speed, accumulate, timeWindow, numDays);
        }
      }
      if (this.Distributed)
      {
        _distributed.startAnimation(speed, accumulate, timeWindow, numDays);
      }
    }


    /// <summary>
    /// Stop the animation.
    /// </summary>
    protected void _stopAnimation()
    {
      if (this.Dll)
      {
        CadKit.Interfaces.IRenderLoop renderLoop = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IRenderLoop;
        if (null != renderLoop)
        {
          renderLoop.UseRenderLoop = false;
          _dll.stopAnimation();
        }
      }
      if (this.Distributed)
      {
        _distributed.stopAnimation();
      }
    }


    /// <summary>
    /// Config filename.
    /// </summary>
    protected string ConfigFilename
    {
      get
      {
        return System.Windows.Forms.Application.UserAppDataPath + "\\minerva_document_config.xml";
      }
    }


    /// <summary>
    /// See if there is a config file.
    /// </summary>
    protected bool HasConfigFile
    {
      get
      {
        if (System.IO.File.Exists(this.ConfigFilename))
          return true;
        return false;
      }
    }


    /// <summary>
    /// Parse the config file.
    /// </summary>
    protected void _parseConfigFile()
    {
      try
      {
        if (this.HasConfigFile)
        {
          // Read the file.
          System.Xml.XmlDocument doc = new System.Xml.XmlDocument();
          doc.Load(this.ConfigFilename);

          string session = "default";

          // Get top-level node.
          System.Xml.XmlNode root = doc.FirstChild;

          // While there is a top-level node...
          while (null != root)
          {
            if ("minerva" == root.Name)
            {
              System.Xml.XmlNodeList kids = root.ChildNodes;
              for (int i = 0; i < kids.Count; ++i)
              {
                System.Xml.XmlNode node = kids[i];
                if ("distributed" == node.Name)
                {
                  if ("true" == node.InnerText)
                  {
                    _useDistributed = true;
                  }
                }
                if ("session" == node.Name)
                {
                  session = node.InnerText;
                }
                if ("host" == node.Name)
                {
                  _distributed.Hostname = node.InnerText;
                }
                if ("database" == node.Name)
                {
                  _distributed.Database = node.InnerText;
                }
                if ("username" == node.Name)
                {
                  _distributed.Username = node.InnerText;
                }
                if ("password" == node.Name)
                {
                  _distributed.Password = node.InnerText;
                }
              }
            }

            // Go to next top-level node.
            root = root.NextSibling;
          }

          if (this.Distributed)
          {
            _distributed.connect();
            this.connectToSession(session);
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1284367684: {0}", e.Message);
      }
    }


    /// <summary>
    /// Are we in dll mode?
    /// </summary>
    public bool Dll
    {
      get
      {
        return _useDll;
      }
    }


    /// <summary>
    /// Are we in distributed mode?
    /// </summary>
    public bool Distributed
    {
      get
      {
        return _useDistributed;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    bool CadKit.Interfaces.IOssimPlanetSettings.ElevationEnabled
    {
      get { return _dll.elevationEnabled(); }
      set { _dll.elevationEnabled(value); }
    }

    float CadKit.Interfaces.IOssimPlanetSettings.HeightExageration
    {
      get { return _dll.elevationExag(); }
      set { _dll.elevationExag(value); }
    }


    float CadKit.Interfaces.IOssimPlanetSettings.ElevationPatchSize
    {
      get { return _dll.elevationPatchSize(); }
      set { _dll.elevationPatchSize(value); }
    }

    float CadKit.Interfaces.IOssimPlanetSettings.MaxLevelDetail
    {
      get { return _dll.levelDetail(); }
      set { _dll.levelDetail(value); }
    }

    string CadKit.Interfaces.IOssimPlanetSettings.ElevationCacheDir
    {
      get { return _dll.elevationCacheDir(); }
      set { _dll.elevationCacheDir(value); }
    }

    bool CadKit.Interfaces.IOssimPlanetSettings.EphemerisEnabled
    {
      get { return _dll.ephemerisFlag(); }
      set { _dll.ephemerisFlag(value); }
    }

    bool CadKit.Interfaces.IOssimPlanetSettings.HudEnabled
    {
      get { return _dll.hudEnabled(); }
      set { _dll.hudEnabled(value); }
    }

    bool CadKit.Interfaces.IOssimPlanetSettings.LatLongGrid
    {
      get { return _dll.latLongGrid(); }
      set { _dll.latLongGrid(value); }
    }

    bool CadKit.Interfaces.ILegend.ShowLegend
    {
      get { return _dll.showLegend(); }
      set { _dll.showLegend(value); }
    }

    void CadKit.Interfaces.ILayerOperation.setLayerOperation(string opType, int val, int[] layers)
    {
      if (this.Dll)
      {
        for (int i = 0; i < layers.Length; ++i)
        {
          ((CadKit.Interfaces.ILayerOperation)this).setLayerOperation(opType, val, i);
        }
      }
    }

    void CadKit.Interfaces.ILayerOperation.setLayerOperation(string opType, int val, int layer)
    {
      if (this.Dll)
      {
         _dll.setLayerOperation(opType, val, _layers[layer]);
      }
    }

    void CadKit.Interfaces.IMovieMode.setMovieMode( bool b )
    {
      if (b)
      {
        CadKit.Viewer.Viewer viewer = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Viewer.Viewer;
        if (this.Dll)
        {
          _dll.setMovieMode(b, viewer.HeliosViewer);
        }
      }
      else
      {
        CadKit.Viewer.Viewer viewer = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Viewer.Viewer;
        if (this.Dll)
        {
          _dll.setMovieMode(b, viewer.HeliosViewer);
        }
      }
    }

    void CadKit.Interfaces.IMovieMode.play()
    {
      CadKit.Viewer.Viewer viewer = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Viewer.Viewer;
      if (this.Dll)
      {
        _dll.play(viewer.HeliosViewer);
      }
    }

    void CadKit.Interfaces.IMovieMode.pause()
    {
      CadKit.Viewer.Viewer viewer = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Viewer.Viewer;
      if (this.Dll)
      {
        _dll.pause(viewer.HeliosViewer);
      }
    }

    void CadKit.Interfaces.IMovieMode.restart()
    {
      CadKit.Viewer.Viewer viewer = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Viewer.Viewer;
      if (this.Dll)
      {
        _dll.restart(viewer.HeliosViewer);
      }
    }

    bool CadKit.Interfaces.IMovieMode.isPlaying()
    {
      if (this.Dll)
      {
        return _dll.isPlaying();
      }
      else
      {
        return false;
      }
    }

    bool CadKit.Interfaces.IMovieMode.isPaused()
    {
      if (this.Dll)
      {
        return _dll.isPaused();
      }
      else
      {
        return false;
      }
    }
  }  
}

