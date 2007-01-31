
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
  namespace Persistence
  {
    public class FormConfig
    {
      /// <summary>
      /// Read the rectangle.
      /// </summary>
      public static void readRect( System.Windows.Forms.Form form, string persistentName )
      {
        if ( null == form || null == persistentName )
          return;
        try
        {
          form.SuspendLayout ();
          _readRect ( form, persistentName );
        }
        catch ( System.Exception e )
        {
          System.Console.WriteLine ( "Error {0}: {1}", 1517174314, e.Message );
        }
        finally
        {
          form.ResumeLayout ();
        }
      }

      /// <summary>
      /// Read the rectangle.
      /// </summary>
      protected static void _readRect( System.Windows.Forms.Form form, string persistentName )
      {
        if ( null == form || null == persistentName )
          return;
        // Get dimensions of screen.
        System.Drawing.Rectangle rect = System.Windows.Forms.Screen.PrimaryScreen.WorkingArea;
        int sw = rect.Width;
        int sh = rect.Height;

        // Default sizes. (They are correct!)
        int x = ( int ) ( 0.1 * ( float ) sw );
        int y = ( int ) ( 0.1 * ( float ) sh );
        int w = ( int ) ( 0.8 * ( float ) sw );
        int h = ( int ) ( 0.8 * ( float ) sh );

        // Set default.
        form.DesktopBounds = new System.Drawing.Rectangle
          ( new System.Drawing.Point ( x, y ), new System.Drawing.Size ( w, h ) );

        // Manual sizing.
        form.StartPosition = System.Windows.Forms.FormStartPosition.Manual;

        // Get dimensions of main window.
        x = CadKit.Persistence.Registry.instance().getInt( persistentName, "x", x );
        y = CadKit.Persistence.Registry.instance().getInt( persistentName, "y", y );
        w = CadKit.Persistence.Registry.instance().getInt( persistentName, "width", w );
        h = CadKit.Persistence.Registry.instance().getInt( persistentName, "height", h );

        // Make sure it fits.
        bool xFits = ( ( x > 0 ) && ( x < ( sw - w ) ) );
        bool yFits = ( ( y > 10 ) && ( y < ( sh - h ) ) );
        bool wFits = ( w < sw );
        bool hFits = ( h < sh );
        bool fits = ( xFits && yFits && wFits && hFits );

        // Set origin and size if it fits.
        if ( fits )
        {
          form.DesktopBounds = new System.Drawing.Rectangle
            ( new System.Drawing.Point ( x, y ), new System.Drawing.Size ( w, h ) );
        }

        // Is it maximized?
        if ( CadKit.Persistence.Registry.instance ().getBool ( persistentName, "maximized", false ) )
          form.WindowState = System.Windows.Forms.FormWindowState.Maximized;
      }

      /// <summary>
      /// Write the rectangle.
      /// </summary>
      public static void writeRect( System.Windows.Forms.Form form, string persistentName )
      {
        if ( null == form || null == persistentName )
          return;
        try
        {
          _writeRect ( form, persistentName );
        }
        catch ( System.Exception e )
        {
          System.Console.WriteLine ( "Error {0}: {1}", 3213183551, e.Message );
        }
      }

      /// <summary>
      /// Read the rectangle.
      /// </summary>
      protected static void _writeRect( System.Windows.Forms.Form form, string persistentName )
      {
        if ( null == form || null == persistentName )
          return;
        bool maximized = ( form.WindowState == System.Windows.Forms.FormWindowState.Maximized );
        CadKit.Persistence.Registry.instance().setBool( persistentName, "maximized", maximized );
        if ( false == maximized )
        {
          CadKit.Persistence.Registry.instance().setInt( persistentName, "x", form.DesktopBounds.X );
          CadKit.Persistence.Registry.instance().setInt( persistentName, "y", form.DesktopBounds.Y );
          CadKit.Persistence.Registry.instance().setInt( persistentName, "width", form.DesktopBounds.Width );
          CadKit.Persistence.Registry.instance().setInt( persistentName, "height", form.DesktopBounds.Height );
        }
      }
    }
  }
}
