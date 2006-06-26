
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Tools
{
  public static class Parent
  {
    /// <summary>
    /// Find the parent mdi form.
    /// </summary>
    public static System.Windows.Forms.Form findMdiParent(System.Windows.Forms.Control child)
    {
      lock ("CadKit.Tools.Parent.findMdiParent")
      {
        if (null == child)
          return null;

        System.Windows.Forms.Form parent = child.FindForm();
        System.Windows.Forms.Form last = null;
        while (null != parent && last != parent)
        {
          last = parent;
          parent = parent.FindForm();
        }
        if (null != parent.MdiParent)
          return parent.MdiParent;
        else
          return null;
      }
    }
  }
}
