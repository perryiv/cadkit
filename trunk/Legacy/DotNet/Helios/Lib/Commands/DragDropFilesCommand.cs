
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class DragDropFilesCommand : CadKit.Helios.Commands.OpenDocumentCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    class Names : System.Collections.Generic.List<string> { }
    private Names _names = new Names();


    /// <summary>
    /// Constructor.
    /// </summary>
    public DragDropFilesCommand(System.Windows.Forms.DragEventArgs args, object caller)
      : base(caller)
    {
      this._init();
      this._getNames(args);
    }


    /// <summary>
    /// Constructor.
    /// </summary>
    public DragDropFilesCommand(string name, object caller)
      : base(caller)
    {
      this._init();
      if (null != name)
        _names.Add(name);
    }


    /// <summary>
    /// Initialize.
    /// </summary>
    private void _init()
    {
      _text = "Drag-n-drop";
      _menuIcon = null;
      _toolIcon = null;
      _keys = 0;
    }

    /// <summary>
    /// Get the file names.
    /// See http://www.codeproject.com/csharp/dragdrop.asp
    /// </summary>
    private void _getNames(System.Windows.Forms.DragEventArgs args)
    {
      if (null != args && null != args.Data)
      {
        System.Array files = args.Data.GetData(System.Windows.Forms.DataFormats.FileDrop) as System.Array;
        if (null != files)
        {
          foreach (object item in files)
          {
            string file = item as string;
            if (null != file && true == System.IO.File.Exists(file))
            {
              _names.Add(file);
            }
          }
        }
      }
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      // Open the files.
      foreach (string name in _names)
      {
        try
        {
          this._open(name);
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 3611905161: {0}", e.Message);
        }
      }
    }
  }
}
