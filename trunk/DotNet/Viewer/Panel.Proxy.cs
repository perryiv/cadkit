
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
    public class PropertyProxy : CadKit.OpenGL.Canvas.PropertyProxy
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
      /// Number of rendering passes property.
      /// </summary>
      [
      System.ComponentModel.Category("Rendering"),
      System.ComponentModel.Description("Rendering Passes"),
      System.ComponentModel.Browsable(true),
      ]
      public uint RenderingPasses
      {
        get { return _panel.RenderingPasses; }
        set
        {
          _panel.RenderingPasses = value;
          _panel.render();
        }
      }


      /// <summary>
      /// Scatter scale.
      /// </summary>
      [
      System.ComponentModel.Category("Rendering"),
      System.ComponentModel.Description("Scatter Scale"),
      System.ComponentModel.Browsable(true),
      ]
      public double ScatterScale
      {
        get { return _panel.ScatterScale; }
        set
        {
          _panel.ScatterScale = value;
          _panel.render();
        }
      }


      /// <summary>
      /// Get/Set the mode.
      /// </summary>
      [
      System.ComponentModel.Category("Rendering"),
      System.ComponentModel.Description("Mode"),
      System.ComponentModel.Browsable(true),
      ]
      public CadKit.Interfaces.ViewMode Mode
      {
        get { return _panel.Mode; }
        set { _panel.Mode = value; }
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
      public string Filename
      {
        get { return _panel.Filename; }
        set { _panel.Filename = value; }
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
      /// Get/set the frame dump size.
      /// </summary>
      [
      System.ComponentModel.Category("Frame Dump"),
      System.ComponentModel.Description("Frame dump image size"),
      System.ComponentModel.Browsable(true),
      ]
      public System.Drawing.Size FrameSize
      {
        get { return _panel.FrameSize; }
        set { _panel.FrameSize = value; }
      }


      /// <summary>
      /// Get/set the flag for using the frame size.
      /// </summary>
      [
      System.ComponentModel.Category("Frame Dump"),
      System.ComponentModel.Description("Use window size when dumping frames"),
      System.ComponentModel.Browsable(true),
      ]
      public bool UseFrameSize
      {
        get { return _panel.UseFrameSize; }
        set { _panel.UseFrameSize = value; }
      }
    }
  }
}
