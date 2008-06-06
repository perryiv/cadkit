
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class RecentDocumentCommand : CadKit.Helios.Commands.OpenDocumentCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    private string _name = null;


    /// <summary>
    /// Constructor.
    /// </summary>
    public RecentDocumentCommand(string name, uint which, object caller)
      : base(caller)
    {
      if (null == name)
        throw new System.ArgumentException("Error 3445272450: null name given to recent file command");

      _name = name;
      _text = System.String.Format("&{0} {1}", which, _name);
      _menuIcon = null;
      _keys = 0;
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      try
      {
        this._open(this.Name);
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1980234199: {0}", e.Message);
      }
    }


    /// <summary>
    /// Return the name.
    /// </summary>
    public string Name
    {
      get { using (this.Lock.read()) { return _name; } }
    }
  }
}
