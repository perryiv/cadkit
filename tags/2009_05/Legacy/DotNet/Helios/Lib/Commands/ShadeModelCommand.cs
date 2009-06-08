
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class ShadeModelCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Data members.
    /// </summary>
    protected CadKit.Interfaces.ShadeModel.Model _model = CadKit.Interfaces.ShadeModel.Model.SMOOTH;


    /// <summary>
    /// Constructor.
    /// </summary>
    protected ShadeModelCommand(object caller)
      : base()
    {
      _caller = caller;
      CadKit.Documents.Manager.Instance.ActiveViewChanged += this._onActiveViewChanged;
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~ShadeModelCommand()
    {
      try
      {
        CadKit.Documents.Manager.Instance.ActiveViewChanged -= this._onActiveViewChanged;
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 8088226900: {0}", e.Message);
      }
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      CadKit.Interfaces.IShadeModel model = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IShadeModel;
      if (null != model)
      {
        model.Model = (this.Model == model.Model) ? CadKit.Interfaces.ShadeModel.Model.NONE : this.Model;
      }
    }


    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      CadKit.Interfaces.IShadeModel model = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IShadeModel;
      return (null != model);
    }


    /// <summary>
    /// Determine if the button is checked.
    /// </summary>
    protected override bool _isChecked()
    {
      CadKit.Interfaces.IShadeModel model = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IShadeModel;
      return ((null != model) && (model.Model == this.Model));
    }


    /// <summary>
    /// Return shading model.
    /// </summary>
    private CadKit.Interfaces.ShadeModel.Model Model
    {
      get { using (this.Lock.read()) { return _model; } }
    }
  }
}
