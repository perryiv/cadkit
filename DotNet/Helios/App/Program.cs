
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [System.STAThread]
    static void Main ( string[] args )
    {
      try
      {
        CadKit.Tools.Threading threading = CadKit.Tools.Threading.Instance;
        CadKit.Tools.RedirectOutput redirect = CadKit.Tools.RedirectOutput.Instance;
        System.Windows.Forms.Application.EnableVisualStyles();
        System.Windows.Forms.Application.SetCompatibleTextRenderingDefault( false );
        CadKit.Helios.Application.Instance.Name = "Helios";
        CadKit.Helios.MainForm form = new CadKit.Helios.MainForm( "CadKit.Helios.MainForm" );
        CadKit.Helios.Application.Instance.MainForm = form;
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
