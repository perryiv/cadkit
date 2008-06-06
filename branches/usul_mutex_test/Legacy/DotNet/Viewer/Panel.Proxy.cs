
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Viewer
{
  public partial class Panel
  {
    public new class PropertyProxy : CadKit.OpenGL.Canvas.PropertyProxy
    {
      /// <summary>
      /// Data members.
      /// </summary>
      private CadKit.Viewer.Panel _panel = null;


      /// <summary>
      /// Constructor
      /// </summary>
      public PropertyProxy(CadKit.Viewer.Panel panel)
        : base(panel)
      {
        if (null == panel)
          throw new System.ArgumentException("Error 1955579892: Null panel given to proxy");
        _panel = panel;
      }


      /// <summary>
      /// Clear color
      /// </summary>
      [
      System.ComponentModel.Category("Graphics"),
      System.ComponentModel.Description("Background color for window"),
      System.ComponentModel.Browsable(true),
      ]
      public override System.Drawing.Color ClearColor
      {
        get { return _panel.ClearColor; }
        set
        {
          _panel.ClearColor = value;
          this._update();
        }
      }


      /// <summary>
      /// Get/set the clear color corner.
      /// </summary>
      [
      System.ComponentModel.Category("Graphics"),
      System.ComponentModel.Description("Background color for window"),
      System.ComponentModel.Browsable(true),
      ]
      public CadKit.Viewer.Glue.Viewer.Corners Corners
      {
        get { return _panel.Corners; }
        set
        {
          _panel.Corners = value;
          _panel.ClearColor = _panel.ClearColor;
        }
      }


      /// <summary>
      /// Number of rendering passes property.
      /// </summary>
      [
      System.ComponentModel.Category("Rendering"),
      System.ComponentModel.Description("Number of rendering passes"),
      System.ComponentModel.Browsable(true),
      ]
      public uint RenderingPasses
      {
        get { return _panel.RenderingPasses; }
        set
        {
          _panel.RenderingPasses = value;
          this._update();
        }
      }


      /// <summary>
      /// Scatter scale.
      /// </summary>
      [
      System.ComponentModel.Category("Rendering"),
      System.ComponentModel.Description("Scale for pixel-scatter pattern when using multiple rendering passes"),
      System.ComponentModel.Browsable(true),
      ]
      public double ScatterScale
      {
        get { return _panel.ScatterScale; }
        set
        {
          _panel.ScatterScale = value;
          this._update();
        }
      }


      /// <summary>
      /// Get/Set the mode.
      /// </summary>
      [
      System.ComponentModel.Category("Rendering"),
      System.ComponentModel.Description("Viewer's mode"),
      System.ComponentModel.Browsable(true),
      ]
      public CadKit.Interfaces.ViewMode Mode
      {
        get { return _panel.Mode; }
        set { _panel.Mode = value; }
      }

      
      /// <summary>
      /// Get/Set the mode.
      /// </summary>
      [
      System.ComponentModel.Category("Rendering"),
      System.ComponentModel.Description("Render in a loop"),
      System.ComponentModel.Browsable(true),
      ]
      public bool RenderLoop
      {
        get { return _panel.RenderTimer; }
        set 
        { 
          if ( value )
            _panel.startRenderTimer(15);
          else
            _panel.endRenderTimer(); 
        }
      }


      /// <summary>
      /// Get/set the directory where screen-dumps are written.
      /// </summary>
      [
      System.ComponentModel.Category("Frame Dump"),
      System.ComponentModel.Description("Directory to dump frames"),
      System.ComponentModel.Browsable(true),
      ]
      public string Directory
      {
        get { return _panel.Directory; }
        set { _panel.Directory = value; }
      }


      /// <summary>
      /// Get/set the filename used when making screen-dumps.
      /// </summary>
      [
      System.ComponentModel.Category("Frame Dump"),
      System.ComponentModel.Description("Base file name for frame dumps"),
      System.ComponentModel.Browsable(true),
      ]
      public string BaseFilename
      {
        get { return _panel.BaseFilename; }
        set { _panel.BaseFilename = value; }
      }


      /// <summary>
      /// Get/set the file extension used when making screen-dumps.
      /// </summary>
      [
      System.ComponentModel.Category("Frame Dump"),
      System.ComponentModel.Description("File name extension for frame dumps"),
      System.ComponentModel.Browsable(true),
      ]
      public string Extension
      {
        get { return _panel.Extension; }
        set { _panel.Extension = value; }
      }


      /// <summary>
      /// Get/set the flag that says whether or not to dump frames.
      /// </summary>
      [
      System.ComponentModel.Category("Frame Dump"),
      System.ComponentModel.Description("Dump frames if true"),
      System.ComponentModel.Browsable(true),
      ]
      public bool DumpFrames
      {
        get { return _panel.DumpFrames; }
        set { _panel.DumpFrames = value; }
      }


      /// <summary>
      /// Get/set the frame dump scale.
      /// </summary>
      [
      System.ComponentModel.Category("Frame Dump"),
      System.ComponentModel.Description("Frame dump image scale"),
      System.ComponentModel.Browsable(true),
      ]
      public float FrameScale
      {
        get { return _panel.FrameScale; }
        set { _panel.FrameScale = value; }
      }


      /// <summary>
      /// Update the panel.
      /// </summary>
      private void _update()
      {
        _panel.Invalidate(true);
        _panel.Update();
      }
    }
  }
}
