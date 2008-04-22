
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Tools
{
  public static class Progress
  {
    /// <summary>
    /// Report progress if possible.
    /// </summary>
    public static void notify(string text, int value, int total, object caller)
    {
      lock ("CadKit.Tools.Progress.notify")
      {
        CadKit.Interfaces.IProgressBar progress = caller as CadKit.Interfaces.IProgressBar;
        if (null != progress)
        {
          progress.Text = (null != text) ? text : "";
          float v = value;
          float t = total;
          float fraction = v / t;
          float position = fraction * progress.Range;
          progress.Value = (int)position;
        }

        CadKit.Interfaces.IUpdateDisplay update = caller as CadKit.Interfaces.IUpdateDisplay;
        if (null != update)
        {
          update.updateDisplay();
        }
      }
    }
  }
}
