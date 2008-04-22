
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
  public class SaveDocumentCommand : CadKit.Commands.Command
  {
    public SaveDocumentCommand(object caller) : base()
    {
      _caller = caller;
      _text = "&Save...";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/save_document.gif");
      _toolIcon = _menuIcon;
      _keys = System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S;
    }


    /// <summary>
    /// 
    /// </summary>
    public override void execute()
    {
      try
      {
        CadKit.Interfaces.IDocument document = CadKit.Documents.Manager.Instance.ActiveDocument;
        if (null != document)
        {
          // Only save if the document is modified.
          // This function should never get called if this is false, but it can't hurt to have the check.
          if (document.Modified)
          {
            // If the document has a name.
            if (false == document.HasDefaultName)
            {
              CadKit.Interfaces.IFileSave save = document as CadKit.Interfaces.IFileSave;
              if (null != save)
                save.save(_caller);
            }

            // We need to have the user choose a name.
            else
            {
              CadKit.Helios.Commands.SaveAsDocumentCommand saveAs = new SaveAsDocumentCommand(_caller);
              saveAs.execute();
            }
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1903003532: {0}", e.Message);
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
        return document.Modified && document is CadKit.Interfaces.IFileSave;
      }

      return false;
    }
  }
}
