
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Persistence
{
  public class FormConfig
  {
    /// <summary>
    /// Read the rectangle.
    /// </summary>
    public static void readRect(System.Windows.Forms.Form form, string persistentName)
    {
      lock ("CadKit.Persistence.FormConfig.readRect")
      {
        try
        {
          if (null == form || null == persistentName)
            return;
          form.SuspendLayout();
          _readRect(form, persistentName);
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error {0}: {1}", 1517174314, e.Message);
        }
        finally
        {
          form.ResumeLayout();
        }
      }
    }

    /// <summary>
    /// Read the rectangle.
    /// </summary>
    protected static void _readRect(System.Windows.Forms.Form form, string persistentName)
    {
      lock ("CadKit.Persistence.FormConfig._readRect")
      {
        if (null == form || null == persistentName)
          return;

        // Get dimensions of screen.
        System.Drawing.Rectangle rect = System.Windows.Forms.Screen.PrimaryScreen.WorkingArea;

        int sw = rect.Width;
        int sh = rect.Height;

        // Default sizes. (They are correct!)
        int x = (int)(0.1 * (float)sw);
        int y = (int)(0.1 * (float)sh);
        int w = (int)(0.8 * (float)sw);
        int h = (int)(0.8 * (float)sh);

        // Set default.
        form.DesktopBounds = new System.Drawing.Rectangle
          (new System.Drawing.Point(x, y), new System.Drawing.Size(w, h));

        // Manual sizing.
        form.StartPosition = System.Windows.Forms.FormStartPosition.Manual;

        // Get dimensions of main window.
        x = CadKit.Persistence.Registry.Instance.getInt(persistentName, "x", x);
        y = CadKit.Persistence.Registry.Instance.getInt(persistentName, "y", y);
        w = CadKit.Persistence.Registry.Instance.getInt(persistentName, "width", w);
        h = CadKit.Persistence.Registry.Instance.getInt(persistentName, "height", h);

        // Loop through each screen and see if the values fit.
        foreach (System.Windows.Forms.Screen screen in System.Windows.Forms.Screen.AllScreens)
        {
          // Make sure it fits.
          if (_fitsScreen(screen, x, y, w, h))
          {
            form.DesktopBounds = new System.Drawing.Rectangle
              (new System.Drawing.Point(x, y), new System.Drawing.Size(w, h));
            break;
          }
        }

        // Is it maximized?
        if (CadKit.Persistence.Registry.Instance.getBool(persistentName, "maximized", false))
          form.WindowState = System.Windows.Forms.FormWindowState.Maximized;
      }
    }


    /// <summary>
    /// Checks to see if the values fit on the given screen.
    /// </summary>
    protected static bool _fitsScreen(System.Windows.Forms.Screen screen, int x, int y, int w, int h)
    {
      System.Drawing.Rectangle rect = screen.WorkingArea;

      // Get the screen width and height.
      int sx = rect.X;
      int sy = rect.Y;
      int sw = rect.Width;
      int sh = rect.Height;

      // Make sure it fits.
      bool xFits = ((x > sx) && (x < ((sw + sx) - w)));
      bool yFits = ((y > 10) && (y < (sh - h)));
      bool wFits = (w < sw);
      bool hFits = (h < sh);
      bool fits = (xFits && yFits && wFits && hFits);

      return fits;
    }


    /// <summary>
    /// Write the rectangle.
    /// </summary>
    public static void writeRect(System.Windows.Forms.Form form, string persistentName)
    {
      lock ("CadKit.Persistence.FormConfig.writeRect")
      {
        try
        {
          if (null == form || null == persistentName)
            return;
          _writeRect(form, persistentName);
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error {0}: {1}", 3213183551, e.Message);
        }
      }
    }

    /// <summary>
    /// Read the rectangle.
    /// </summary>
    protected static void _writeRect(System.Windows.Forms.Form form, string persistentName)
    {
      lock ("CadKit.Persistence.FormConfig._writeRect")
      {
        if (null == form || null == persistentName)
          return;
        bool maximized = (form.WindowState == System.Windows.Forms.FormWindowState.Maximized);
        CadKit.Persistence.Registry.Instance.setBool(persistentName, "maximized", maximized);
        if (false == maximized)
        {
          CadKit.Persistence.Registry.Instance.setInt(persistentName, "x", form.DesktopBounds.X);
          CadKit.Persistence.Registry.Instance.setInt(persistentName, "y", form.DesktopBounds.Y);
          CadKit.Persistence.Registry.Instance.setInt(persistentName, "width", form.DesktopBounds.Width);
          CadKit.Persistence.Registry.Instance.setInt(persistentName, "height", form.DesktopBounds.Height);
        }
      }
    }
  }
}
