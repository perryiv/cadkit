
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.App
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [System.STAThread]
    static void Main()
    {
      try
      {
        System.Windows.Forms.Application.EnableVisualStyles();
        System.Windows.Forms.Application.SetCompatibleTextRenderingDefault( false );
        CadKit.Helios.Lib.Application.instance().Name = "Helios";
        CadKit.Helios.Lib.MainForm form = new CadKit.Helios.Lib.MainForm( "CadKit.Helios.Lib.MainForm" );
        CadKit.Helios.Lib.Application.instance().MainForm = form;
        System.Windows.Forms.Application.Run( form );
      }
      catch ( System.Exception e )
      {
        if ( null != e.Message && e.Message.Length > 0 )
        {
          System.Windows.Forms.MessageBox.Show( e.Message );
          System.Console.WriteLine( e.Message );
        }
      }
    }
  }
}
