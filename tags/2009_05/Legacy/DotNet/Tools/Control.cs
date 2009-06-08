
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Tools
{
  public static class Control
  {
    /// <summary>
    /// Make the given control be the last child of the forms's control list.
    /// </summary>
    public static void last(System.Windows.Forms.Form form, System.Windows.Forms.Control control)
    {
      lock ("CadKit.Tools.Control.last")
      {
        if (null != form && null != form.Controls && null != control)
        {
          form.Controls.Remove(control);
          form.Controls.Add(control);
        }
      }
    }
  }
}
