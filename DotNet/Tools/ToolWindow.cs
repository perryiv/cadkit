
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
  namespace Tools
  {
    public class ToolWindow
    {
      /// <summary>
      /// Configure the tool window.
      /// </summary>
      public static void configure ( System.Windows.Forms.Form toolWindow, System.Windows.Forms.Form parent, string text )
      {
        if ( null == toolWindow )
          throw new System.ArgumentNullException( "Error 3898181007: null tool window given" );

        // Does not appear in the taskbar or the ALT+TAB window. Need both.
        toolWindow.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
        toolWindow.ShowInTaskbar = false;

        // Look for parent form.
        if ( null != parent )
        {
          // Set appropriate text.
          toolWindow.Text = text + ": " + parent.Text;

          // Set the owner so that this property grid stays on top of the parent form.
          toolWindow.Owner = parent;
        }
        else
        {
          // Set appropriate text.
          toolWindow.Text = text;
        }
      }
    }
  }
}
