
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class DisplayListCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public DisplayListCommand(object caller)
      : base()
    {
      _caller = caller;
      _text = "Display Lists";
      _toolTipText = _text;
      _menuIcon = null;
      _toolIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/display_list_command.png");
      CadKit.Documents.Manager.Instance.ActiveViewChanged += this._onActiveViewChanged;
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~DisplayListCommand()
    {
      try
      {
        CadKit.Documents.Manager.Instance.ActiveViewChanged -= this._onActiveViewChanged;
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 4266305360: {0}", e.Message);
      }
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      CadKit.Interfaces.IDisplayListUse use = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IDisplayListUse;
      if (null != use)
      {
        use.UseDisplayLists = !use.UseDisplayLists;
      }
    }


    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      CadKit.Interfaces.IDisplayListUse use = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IDisplayListUse;
      return (null != use);
    }


    /// <summary>
    /// Determine if the button is checked.
    /// </summary>
    protected override bool _isChecked()
    {
      CadKit.Interfaces.IDisplayListUse use = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IDisplayListUse;
      return (null == use) ? false : use.UseDisplayLists;
    }
  }
}
