
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.Document
{
  class Document : 
    CadKit.Documents.Document,
    CadKit.Interfaces.ILayerList,
    CadKit.Interfaces.IAnimateTemporal
  {
    enum Mode
    {
      BOTH,
      DLL,
      DISTRIBUTED
    }

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

    private Mode _mode = Mode.DLL;

    /// <summary>
    /// 
    /// </summary>
    public Document()
      : base()
    {
      _distributed.Hostname = "cinema.dt.asu.edu";
      _distributed.Username = "wnv_app";
      _distributed.Password = "wnv";
      _distributed.Database = "wnv_application";
    }


    /// <summary>
    /// 
    /// </summary>
    public void connect()
    {
      _distributed.connect();
    }


    /// <summary>
    /// 
    /// </summary>
    public void disconnect()
    {
      _distributed.disconnect();
    }


    /// <summary>
    /// 
    /// </summary>
    public int connectToSession(string name)
    {
      int sessionID = _distributed.connectToSession(name);

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

      return sessionID;
    }


    public void deleteSession()
    {
      _distributed.deleteSession();
    }

    public string[] Sessions
    {
      get
      {
        return _distributed.getAvailableSessions().ToArray();
      }
    }


    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    protected override string _typeName()
    {
      return DT.Minerva.Plugins.Document.Document.TypeName;
    }


    /// <summary>
    /// 
    /// </summary>
    public CadKit.Viewer.Glue.Viewer Viewer
    {
      set
      {
        _dll.viewer(value);
      }
    }


    /// <summary>
    /// 
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
    /// 
    /// </summary>
    CadKit.Interfaces.ILayer[] CadKit.Interfaces.ILayerList.Layers 
    { 
      get
      {
        return _layers.ToArray();
      }
    }

    /// <summary>
    /// 
    /// </summary>
    void CadKit.Interfaces.ILayerList.addLayer(CadKit.Interfaces.ILayer layer, object caller)
    {
      if (null != layer)
      {
        layer.show();
        this._showLayer(layer as DT.Minerva.Interfaces.ILayer);
        _layers.Add(layer);
      }
    }


    void CadKit.Interfaces.ILayerList.modifyLayer(CadKit.Interfaces.ILayer layer, object caller)
    {
      if (null != layer)
      {
        layer.modify();
        this._modifyLayer(layer as DT.Minerva.Interfaces.ILayer);
      }
    }


    /// <summary>
    /// 
    /// </summary>
    void CadKit.Interfaces.ILayerList.hideLayer(CadKit.Interfaces.ILayer layer, object caller)
    {
      if (null != layer)
      {
        layer.hide();
        _dll.dirtyScene();
      }
    }


    /// <summary>
    /// 
    /// </summary>
    void CadKit.Interfaces.ILayerList.showLayer(CadKit.Interfaces.ILayer layer, object caller)
    {
      if (null != layer)
      {
        layer.show();
        _dll.dirtyScene();
      }
    }


    /// <summary>
    /// 
    /// </summary>
    void CadKit.Interfaces.ILayerList.removeLayer(CadKit.Interfaces.ILayer layer, object caller)
    {
      this._removeLayer(layer as DT.Minerva.Interfaces.ILayer);
      _layers.Remove(layer);
    }


    /// <summary>
    /// 
    /// </summary>
    protected void _removeLayer(DT.Minerva.Interfaces.ILayer layer)
    {
      if (null != layer)
      {
        if (_mode == Mode.BOTH || _mode == Mode.DLL)
        {
          _dll.removeLayer(layer);
        }
        if (_mode == Mode.BOTH || _mode == Mode.DISTRIBUTED)
        {
          _distributed.removeLayer(layer);
        }
      }
    }


    /// <summary>
    /// 
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
    protected void _showLayer(DT.Minerva.Interfaces.ILayer layer)
    {
      if (null != layer)
      {
        if (_mode == Mode.BOTH || _mode == Mode.DLL)
        {
          DT.Minerva.Plugins.Document.AddLayerJob job = new DT.Minerva.Plugins.Document.AddLayerJob(layer, _dll);
          job.Start += job._showLayer;
          job.Finish += this._jobEnd;
          CadKit.Threads.Jobs.Manager.Instance.add(job);
        }
        if (_mode == Mode.BOTH || _mode == Mode.DISTRIBUTED)
        {
          _distributed.showLayer(layer);
        }
      }
    }


    /// <summary>
    /// Modify the layer.
    /// </summary>
    protected void _modifyLayer(DT.Minerva.Interfaces.ILayer layer)
    {
      if (null != layer)
      {
        if (_mode == Mode.BOTH || _mode == Mode.DLL)
        {
          DT.Minerva.Plugins.Document.ModifyLayerJob job = new DT.Minerva.Plugins.Document.ModifyLayerJob(layer, _dll);
          job.Start += job._showLayer;
          job.Finish += this._jobEnd;
          CadKit.Threads.Jobs.Manager.Instance.add(job);
        }
        if (_mode == Mode.BOTH || _mode == Mode.DISTRIBUTED)
        {
          _distributed.showLayer(layer);
        }
      }
    }


    /// <summary>
    /// Start animation.
    /// </summary>
    void CadKit.Interfaces.IAnimateTemporal.startAnimation(float speed, bool accumulate, bool dateTimeStep, bool timeWindow, int numDays)
    {
      try
      {
        this._startAnimation(speed, accumulate, dateTimeStep, timeWindow, numDays);
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
    protected void _startAnimation(float speed, bool accumulate, bool dateTimeStep, bool timeWindow, int numDays)
    {
      if (_mode == Mode.BOTH || _mode == Mode.DLL)
      {
        CadKit.Interfaces.IRenderLoop renderLoop = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IRenderLoop;
        if (null != renderLoop)
        {
          renderLoop.UseRenderLoop = true;
          _dll.startAnimation(speed, accumulate, dateTimeStep, timeWindow, numDays);
        }
      }
      if (_mode == Mode.BOTH || _mode == Mode.DISTRIBUTED)
      {
        _distributed.startAnimation(speed, accumulate, dateTimeStep, timeWindow, numDays);
      }
    }


    /// <summary>
    /// Stop the animation.
    /// </summary>
    protected void _stopAnimation()
    {
      if (_mode == Mode.BOTH || _mode == Mode.DLL)
      {
        CadKit.Interfaces.IRenderLoop renderLoop = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IRenderLoop;
        if (null != renderLoop)
        {
          renderLoop.UseRenderLoop = false;
          _dll.stopAnimation();
        }
      }
      if (_mode == Mode.BOTH || _mode == Mode.DISTRIBUTED)
      {
        _distributed.stopAnimation();
      }
    }
  }
}
