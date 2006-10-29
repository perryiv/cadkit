
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
        throw new System.ArgumentException("Error 3445272450: null nme given to recent file command");

      _name = name;
      _text = System.String.Format("&{0} {1}", which, _name);
      _menuIcon = null;
      _keys = 0;
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    protected override void _execute()
    {
      lock (_mutex)
      {
        try
        {
          this._open(_name);
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 1980234199: {0}", e.Message);
        }
      }
    }
  }
}
