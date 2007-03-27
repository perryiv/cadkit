
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Tools
{
  public class ToolStripPanel
  {
    /// <summary>
    /// Make a tool-strip panel.
    /// </summary>
    public static System.Windows.Forms.ToolStripPanel make(System.Windows.Forms.DockStyle style, System.Windows.Forms.ToolStrip strip)
    {
      lock ("CadKit.Tools.ToolStripPanel.make.1")
      {
        System.Windows.Forms.ToolStripPanel panel = CadKit.Tools.ToolStripPanel.make(style);
        if (null != panel && null != strip)
          panel.Join(strip);
        return panel;
      }
    }

    /// <summary>
    /// Make a tool-strip panel.
    /// </summary>
    public static System.Windows.Forms.ToolStripPanel make(System.Windows.Forms.DockStyle style)
    {
      lock ("CadKit.Tools.ToolStripPanel.make.2")
      {
        System.Windows.Forms.ToolStripPanel panel = new System.Windows.Forms.ToolStripPanel();
        panel.Dock = style;
        return panel;
      }
    }
  }
}
