
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
      lock ("CadKit.Tools.Menu")
      {
        System.Windows.Forms.ToolStripMenuItem menu = new System.Windows.Forms.ToolStripMenuItem();
        menu.Text = text;
        return menu;
      }
    }
#if no
    /// <summary>
    /// Initialize the menu.
    /// </summary>
    public static System.Windows.Forms.ToolStripMenuItem makeMenu(string text, System.EventHandler handler)
    {
      lock ("CadKit.Tools.Menu")
      {
        System.Windows.Forms.ToolStripMenuItem menu = new System.Windows.Forms.ToolStripMenuItem();
        menu.Text = text;

        if (null != handler)
          menu.DropDownOpening += handler;

        menu.DropDownItems.Add(CadKit.Tools.Menu.makeMenuButton("For keyboard interaction", null, null, 0));
        return menu;
      }
    }

    /// <summary>
    /// Make a new button.
    /// </summary>
    public static System.Windows.Forms.ToolStripMenuItem makeMenuButton(string text, System.EventHandler handler, string icon, System.Windows.Forms.Keys keys)
    {
      lock ("CadKit.Tools.Menu")
      {
        System.Windows.Forms.ToolStripMenuItem button = new System.Windows.Forms.ToolStripMenuItem();
        button.Text = text;

        if (null != handler)
          button.Click += handler;

        if (0 != keys)
          button.ShortcutKeys = keys;

        if (null != icon && icon.Length > 0)
        {
          System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
          button.Image = ((System.Drawing.Image)(resources.GetObject(icon)));
          if (null == button.Image)
            button.Image = CadKit.Images.Image.load(Bm.Gui.Application.instance().iconDir() + "/Icons/" + icon);
        }
        return button;
      }
    }
#endif
    /// <summary>
    /// Append a separator if the menu has any items.
    /// </summary>
    public static void appendSeparatorIfNeeded(System.Windows.Forms.ToolStripMenuItem menu)
    {
      lock ("CadKit.Tools.Menu")
      {
        if (menu.DropDownItems.Count > 0)
        {
          object obj = menu.DropDownItems[menu.DropDownItems.Count - 1];
          if (null != obj)
          {
            if (obj.GetType().ToString() != typeof(System.Windows.Forms.ToolStripSeparator).ToString())
              menu.DropDownItems.Add(new System.Windows.Forms.ToolStripSeparator());
          }
        }
      }
    }
  }
}
