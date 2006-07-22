
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
        // Cache this thread's id.
        CadKit.Tools.Threading threading = CadKit.Tools.Threading.Instance;

        // Redirect standard output to a file.
        CadKit.Tools.RedirectOutput redirect = CadKit.Tools.RedirectOutput.Instance;

        // Setup code for the forms.
        System.Windows.Forms.Application.EnableVisualStyles();
        System.Windows.Forms.Application.SetCompatibleTextRenderingDefault( false );

        // Declare the application and name it.
        CadKit.Helios.Application.Instance.Name = "Helios";
        System.Console.WriteLine(CadKit.Helios.Application.Instance.Name);

        // Declare main form and pass the persistant name.
        CadKit.Helios.MainForm form = new CadKit.Helios.MainForm( "CadKit.Helios.MainForm" );

        // Set the application's main form.
        CadKit.Helios.Application.Instance.MainForm = form;

        // Run the application.
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
