
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Tools
{
  public class Size
  {
    /// <summary>
    /// Get the appropriate size for the MDI child form.
    /// </summary>
    public static void mdiChild ( System.Drawing.Size size, System.Windows.Forms.Form child )
    {
      lock ("CadKit.Tools.Size.size")
      {
        if (null == child)
          return;

        float percent = 0.7f;
        float fh = percent * size.Height;
        float fw = percent * size.Width;
        int ih = (int)fh;
        int iw = (int)fw;
        child.Size = new System.Drawing.Size(iw, ih);
      }
    }
  }
}
