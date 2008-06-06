
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.OpenGL
{
  partial class Canvas : System.Windows.Forms.UserControl
  {
    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      try
      {
        // This function is always called, or so it seems. 
        // Therefore, deleting the rendering context here.
        this._deleteRenderingContext();

        // Dispose components.
        if ((true == disposing) && (null != components))
        {
          components.Dispose();
        }

        // Call base class's function.
        base.Dispose(disposing);
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1587966600: {0}\n{1}", e.Message, e.StackTrace);
      }
    }

    #region Component Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      this.SuspendLayout();
      // 
      // Canvas
      // 
      this.Name = "Canvas";
      this.ResumeLayout(false);

    }

    #endregion

    private System.ComponentModel.IContainer components;
  }
}
