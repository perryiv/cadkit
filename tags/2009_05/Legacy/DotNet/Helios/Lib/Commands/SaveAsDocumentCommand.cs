
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class SaveAsDocumentCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Local data members.
    /// </summary>
    private string _persistentName = "CadKit.Helios.Commands.SaveAsDocumentCommand.SaveFileDialog";

    /// <summary>
    /// Constructor.
    /// </summary>
    public SaveAsDocumentCommand(object caller)
      : base()
    {
      _caller = caller;
      _text = "Save As...";
    }


    /// <summary>
    /// 
    /// </summary>
    public override void execute()
    {
      try
      {
        CadKit.Interfaces.IFiltersSave filtersSave = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IFiltersSave;

        if (null != filtersSave)
        {
          CadKit.Tools.AskForFilename ask = new CadKit.Tools.AskForFilename("Save As", CadKit.Helios.Application.Instance.Name, this.PersistentName, filtersSave.Filters);
          string filename = ask.Filename;

          CadKit.Interfaces.IFileSave fileSave = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IFileSave;
          if (null != fileSave)
          {
            fileSave.saveAs(filename, _caller);
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 933316032: {0}", e.Message);
      }
    }


    /// <summary>
    /// 
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      return CadKit.Documents.Manager.Instance.ActiveDocument is CadKit.Interfaces.IFileSave;
    }


    /// <summary>
    /// Set/get the persistent name.
    /// </summary>
    protected string PersistentName
    {
      get { return _persistentName; }
    }
  }
}
