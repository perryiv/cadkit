
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Tools
{
  public class ToolWindow
  {
    /// <summary>
    /// Configure the tool window.
    /// </summary>
    public static void configure(System.Windows.Forms.Form toolWindow, System.Windows.Forms.Form parent, string text, bool modifyText )
    {
      lock ("CadKit.Tools.ToolWindow.configure")
      {
        if (null == toolWindow)
          throw new System.ArgumentNullException("Error 3898181007: null tool window given", null as System.Exception);

        // Does not appear in the taskbar or the ALT+TAB window. Need both.
        toolWindow.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
        toolWindow.ShowInTaskbar = false;

        // Look for parent form.
        if (null != parent)
        {
          // Set appropriate text.
          toolWindow.Text = (true == modifyText) ? ( text + ": " + parent.Text ) : text;

          // Set the owner so that this property grid stays on top of the parent form.
          toolWindow.Owner = parent;
        }
        else
        {
          // Set appropriate text.
          toolWindow.Text = text;
        }

        // Set the name.
        toolWindow.Name = toolWindow.Text;
      }
    }
  }
}
