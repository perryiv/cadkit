
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
    CadKit.Interfaces.IFileExport,
    CadKit.Interfaces.IFiltersSave,
    CadKit.Interfaces.IFiltersExport,
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
    private DT.Minerva.Plugins.Document.Glue.DllGlue _navtiveDocument = new DT.Minerva.Plugins.Document.Glue.DllGlue();

    /// <summary>
    ///  Mirror layers on managed side.  It's a bit easier to work with.
    /// </summary>
    private System.Collections.Generic.List<CadKit.Interfaces.ILayer> _layers = new System.Collections.Generic.List<CadKit.Interfaces.ILayer>();


    /// <summary>
    /// Constructor.
    /// </summary>
    public Document()
      : base()
    {
      CadKit.Documents.Manager.Instance.ActiveDocumentChanged += new CadKit.Documents.Manager.ActiveDocumentChangedDelegate(_activeDocumentChanged);
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~Document()
    {
      CadKit.Documents.Manager.Instance.ActiveDocumentChanged -= _activeDocumentChanged;
      //this._cleanup();
    }


    /// <summary>
    /// Dispose.
    /// </summary>
    void System.IDisposable.Dispose()
    {
      //this._cleanup();
    }


    /// <summary>
    /// Clean up.
    /// </summary>
    protected override void _cleanup()
    {
      try
      {
        _navtiveDocument = null;

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

        _navtiveDocument.setActive(true);

        // Read the file.
        _navtiveDocument.open(name);

        // Set document name.
        this.Name = name;
        this.HasDefaultName = false;

        // Add file to recent-file list.
        CadKit.Interfaces.IRecentFileList recent = caller as CadKit.Interfaces.IRecentFileList;
        if (null != recent)
        {
          recent.add(name);
        }

        _layers.InsertRange(0, _navtiveDocument.Layers);
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
      _navtiveDocument.save();
      this.Modified = false;
    }


    /// <summary>
    /// Save the document as the new filename.
    /// </summary>
    public void saveAs(string filename, object caller)
    {
      this.Name = filename;
      this.HasDefaultName = false;

      _navtiveDocument.saveAs(filename);
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
    /// Return the filters.
    /// </summary>
    CadKit.Interfaces.Filters CadKit.Interfaces.IFiltersExport.Filters
    {
      get
      {
        CadKit.Interfaces.Filters filters = new CadKit.Interfaces.Filters();
        filters.Add(new CadKit.Interfaces.Filter("Minerva Document (*.minerva)", "*.minerva"));
        filters.Add(new CadKit.Interfaces.Filter("Google Earth (*.kml)", "*.kml"));
        filters.Add(new CadKit.Interfaces.Filter("Google Earth Archive (*.kmz)", "*.kmz"));
        return filters;
      }
    }


    /// <summary>
    /// Export the document to the given filename.
    /// </summary>
    public void export(string filename)
    {
      _navtiveDocument.exportFile(filename);
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
        _navtiveDocument.viewer(value);
      }
    }


    /// <summary>
    /// resize the window.
    /// </summary>
    public void resize(object sender, System.EventArgs e)
    {
      System.Windows.Forms.Form form = sender as System.Windows.Forms.Form;
      if (null != form && null != _navtiveDocument)
      {
        _navtiveDocument.resize(form.Size.Width, form.Size.Height);
      }
    }


    /// <summary>
    /// Get the layer list.
    /// </summary>
    CadKit.Interfaces.ILayer[] CadKit.Interfaces.ILayerList.Layers
    {
      get
      {
        return this._layers.ToArray();
      }
    }


    /// <summary>
    /// Add a layer
    /// </summary>
    void CadKit.Interfaces.ILayerList.addLayer(CadKit.Interfaces.ILayer layer, object caller)
    {
      if (null != layer)
      {
        _navtiveDocument.setActive(true);
        if (layer is CadKit.Interfaces.IOssimLayer)
          _navtiveDocument.addLayer(layer, null);
        else
          this._createAddLayerJob(layer);

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
        this._createModifyLayerJob(layer);
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
        _navtiveDocument.setActive(true);
        _navtiveDocument.hideLayer(layer);
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
        _navtiveDocument.setActive(true);
        _navtiveDocument.showLayer(layer);
        this.Modified = true;
      }
    }


    /// <summary>
    /// Remove the layer.
    /// </summary>
    void CadKit.Interfaces.ILayerList.removeLayer(CadKit.Interfaces.ILayer layer, object caller)
    {
      if (null != layer)
      {
        _navtiveDocument.setActive(true);
        _layers.Remove(layer);
        _navtiveDocument.removeLayer(layer);
        this.Modified = true;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    void CadKit.Interfaces.ILayerList.viewLayerExtents(CadKit.Interfaces.ILayer layer)
    {
      _navtiveDocument.viewLayerExtents(layer);
    }


    void CadKit.Interfaces.ILayerList.addToFavorites(CadKit.Interfaces.ILayer layer)
    {
      _navtiveDocument.addToFavorites(layer);
      this.Modified = true;
    }

    CadKit.Interfaces.ILayer CadKit.Interfaces.ILayerList.createFavorite(string name)
    {
      return _navtiveDocument.createFavorite(name);
    }


    string[] CadKit.Interfaces.ILayerList.Favorites
    {
      get { return _navtiveDocument.Favorites; }
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
    protected void _createAddLayerJob(CadKit.Interfaces.ILayer layer)
    {
      if (null != layer)
      {
        _navtiveDocument.setActive(true);
        DT.Minerva.Plugins.Document.AddLayerJob job = new DT.Minerva.Plugins.Document.AddLayerJob(layer, _navtiveDocument);
        job.Name = "Adding " + layer.Name;
        job.Start += job._showLayer;
        job.Finish += this._jobEnd;
        CadKit.Threads.Jobs.Manager.Instance.add(job);
      }
    }


    /// <summary>
    /// Modify the layer.
    /// </summary>
    protected void _createModifyLayerJob(CadKit.Interfaces.ILayer layer)
    {
      if (null != layer)
      {
        _navtiveDocument.setActive(true);
        DT.Minerva.Plugins.Document.ModifyLayerJob job = new DT.Minerva.Plugins.Document.ModifyLayerJob(layer, _navtiveDocument);
        job.Name = "Modifying " + layer.Name;
        job.Start += job._showLayer;
        job.Finish += this._jobEnd;
        CadKit.Threads.Jobs.Manager.Instance.add(job);
      }
    }


    CadKit.Interfaces.AnimateTimestep CadKit.Interfaces.IAnimateTemporal.TimestepType
    {
      get { return _navtiveDocument.timestepType(); }
      set { _navtiveDocument.timestepType(value); }
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

    void CadKit.Interfaces.IAnimateTemporal.pauseAnimation()
    {
      _navtiveDocument.pauseAnimation();
    }

    double CadKit.Interfaces.IAnimateTemporal.AnimationSpeed
    {
      get
      {
        return 0.0;
      }
      set
      {
        _navtiveDocument.animationSpeed(value);
      }
    }

    bool CadKit.Interfaces.IAnimateTemporal.ShowPastEvents
    {
      get
      {
        return false;
      }
      set
      {
      }
    }

    bool CadKit.Interfaces.IAnimateTemporal.TimeWindow
    {
      get
      {
        return false;
      }
      set
      {
      }
    }

    bool CadKit.Interfaces.IAnimateTemporal.TimeWindowLength
    {
      get
      {
        return false;
      }
      set
      {
      }
    }


    /// <summary>
    /// Start the animation.
    /// </summary>
    protected void _startAnimation(float speed, bool accumulate, bool timeWindow, int numDays)
    {
      CadKit.Interfaces.IRenderLoop renderLoop = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IRenderLoop;
      if (null != renderLoop)
      {
        renderLoop.UseRenderLoop = true;
        _navtiveDocument.startAnimation(speed, accumulate, timeWindow, numDays);
      }
    }


    /// <summary>
    /// Stop the animation.
    /// </summary>
    protected void _stopAnimation()
    {
      CadKit.Interfaces.IRenderLoop renderLoop = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IRenderLoop;
      if (null != renderLoop)
      {
        renderLoop.UseRenderLoop = false;
        _navtiveDocument.stopAnimation();
      }
    }


    /// <summary>
    /// 
    /// </summary>
    bool CadKit.Interfaces.IOssimPlanetSettings.ElevationEnabled
    {
      get { return _navtiveDocument.elevationEnabled(); }
      set { _navtiveDocument.elevationEnabled(value); }
    }

    float CadKit.Interfaces.IOssimPlanetSettings.HeightExageration
    {
      get { return _navtiveDocument.elevationExag(); }
      set { _navtiveDocument.elevationExag(value); }
    }


    float CadKit.Interfaces.IOssimPlanetSettings.ElevationPatchSize
    {
      get { return _navtiveDocument.elevationPatchSize(); }
      set { _navtiveDocument.elevationPatchSize(value); }
    }

    float CadKit.Interfaces.IOssimPlanetSettings.MaxLevelDetail
    {
      get { return _navtiveDocument.levelDetail(); }
      set { _navtiveDocument.levelDetail(value); }
    }

    string CadKit.Interfaces.IOssimPlanetSettings.ElevationCacheDir
    {
      get { return _navtiveDocument.elevationCacheDir(); }
      set { _navtiveDocument.elevationCacheDir(value); }
    }

    bool CadKit.Interfaces.IOssimPlanetSettings.EphemerisEnabled
    {
      get { return _navtiveDocument.ephemerisFlag(); }
      set { _navtiveDocument.ephemerisFlag(value); }
    }

    bool CadKit.Interfaces.IOssimPlanetSettings.HudEnabled
    {
      get { return _navtiveDocument.hudEnabled(); }
      set { _navtiveDocument.hudEnabled(value); }
    }

    bool CadKit.Interfaces.IOssimPlanetSettings.LatLongGrid
    {
      get { return _navtiveDocument.latLongGrid(); }
      set { _navtiveDocument.latLongGrid(value); }
    }

    bool CadKit.Interfaces.ILegend.ShowLegend
    {
      get { return _navtiveDocument.showLegend(); }
      set { _navtiveDocument.showLegend(value); }
    }

    float CadKit.Interfaces.ILegend.PercentScreenWidth
    {
      get { return _navtiveDocument.percentScreenWidth(); }
      set { _navtiveDocument.percentScreenWidth(value); }
    }

    void CadKit.Interfaces.ILayerOperation.setLayerOperation(string opType, int val, CadKit.Interfaces.ILayer layer)
    {
      _navtiveDocument.setLayerOperation(opType, val, layer);
    }

    void CadKit.Interfaces.IMovieMode.setMovieMode( bool b )
    {
      if (b)
      {
        CadKit.Viewer.Viewer viewer = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Viewer.Viewer;
        _navtiveDocument.setMovieMode(b, viewer.HeliosViewer);
      }
      else
      {
        CadKit.Viewer.Viewer viewer = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Viewer.Viewer;
        _navtiveDocument.setMovieMode(b, viewer.HeliosViewer);
      }
    }

    void CadKit.Interfaces.IMovieMode.play()
    {
      CadKit.Viewer.Viewer viewer = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Viewer.Viewer;
      _navtiveDocument.play(viewer.HeliosViewer);
    }

    void CadKit.Interfaces.IMovieMode.pause()
    {
      CadKit.Viewer.Viewer viewer = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Viewer.Viewer;
      _navtiveDocument.pause(viewer.HeliosViewer);
    }

    void CadKit.Interfaces.IMovieMode.restart()
    {
      CadKit.Viewer.Viewer viewer = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Viewer.Viewer;
      _navtiveDocument.restart(viewer.HeliosViewer);
    }

    bool CadKit.Interfaces.IMovieMode.isPlaying()
    {
      return _navtiveDocument.isPlaying();
    }

    bool CadKit.Interfaces.IMovieMode.isPaused()
    {
      return _navtiveDocument.isPaused();
    }

    void _activeDocumentChanged(CadKit.Interfaces.IDocument oldDoc, CadKit.Interfaces.IDocument newDoc)
    {
      //bool active = ( this == newDoc );
      //_navtiveDocument.setActive ( active );
    }
  }  
}

