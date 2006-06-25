
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Tools
{
  public class Menu
  {
    /// <summary>
    /// Make a menu.
    /// </summary>
    public static System.Windows.Forms.ToolStripMenuItem makeMenu(string text)
    {
      System.Windows.Forms.ToolStripMenuItem menu = new System.Windows.Forms.ToolStripMenuItem();
      menu.Text = text;
      return menu;
    }
  }
}
