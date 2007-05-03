
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
  public class ExportFileCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Local data members.
    /// </summary>
    private string _persistentName = "CadKit.Helios.Commands.ExportFileCommand.SaveFileDialog";

    public ExportFileCommand(object caller)
      : base()
    {
      _caller = caller;
      _text = "Export File";
    }


    /// <summary>
    /// 
    /// </summary>
    public override void execute()
    {
      CadKit.Interfaces.IFiltersExport filtersExport = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IFiltersExport;

      if (null != filtersExport)
      {
        CadKit.Tools.AskForFilename ask = new CadKit.Tools.AskForFilename("Export File", CadKit.Helios.Application.Instance.Name, this._persistentName, filtersExport.Filters);
        string filename = ask.Filename;

        CadKit.Interfaces.IFileExport fileExport = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IFileExport;
        if (null != fileExport)
        {
          fileExport.export(filename);
        }
      }
    }

    /// <summary>
    /// 
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      CadKit.Interfaces.IDocument document = CadKit.Documents.Manager.Instance.ActiveDocument;
      if (null != document)
      {
        return document is CadKit.Interfaces.IFileExport;
      }

      return false;
    }
  }
}
