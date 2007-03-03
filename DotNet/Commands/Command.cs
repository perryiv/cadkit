
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Commands
{
  public abstract class Command : 
    System.IDisposable,
    CadKit.Interfaces.ICommand
  {
    /// <summary>
    /// Local types.
    /// </summary>
    private delegate void VoidReturnZeroArgumentsDelegate();
    public class CommandList : System.Collections.Generic.List<Command> { };


    /// <summary>
    /// Data members.
    /// </summary>
    private static CommandList _commandList = new CommandList();
    private static CadKit.Threads.Tools.Lock _commandLock = new CadKit.Threads.Tools.Lock();
    private CadKit.Threads.Tools.Lock _lock = null;
    protected object _caller = null;
    protected System.Windows.Forms.ToolStripMenuItem _menuButton = null;
    protected System.Windows.Forms.ToolStripButton _toolButton = null;
    protected string _text = "Default Text";
    protected string _toolTipText = "";
    protected System.Drawing.Image _menuIcon = null;
    protected System.Drawing.Image _toolIcon = null;
    protected System.Windows.Forms.Keys _keys = 0;
    protected bool _isToggle = false;


    /// <summary>
    /// Constructor.
    /// </summary>
    protected Command()
    {
      using (_commandLock.write())
      {
        _commandList.Add(this);
      }
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~Command()
    {
      this._localCleanup();
    }


    /// <summary>
    /// Called by the system to clean this instance.
    /// </summary>
    void System.IDisposable.Dispose()
    {
      this._localCleanup();
    }


    /// <summary>
    /// Clean this instance.
    /// </summary>
    private void _localCleanup()
    {
      try
      {
        using (_commandLock.write())
        {
          _commandList.Remove(this);
        }

        _lock = null;
        _caller = null;
        _menuButton = null;
        _toolButton = null;
        _text = null;
        _toolTipText = null;
        _menuIcon = null;
        _toolIcon = null;
        _keys = 0;
        _isToggle = false;
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2214346796: {0}", e.Message);
      }
    }


    /// <summary>
    /// Can the command be undone?
    /// </summary>
    bool CadKit.Interfaces.ICommand.canUndo()
    {
      return this.canUndo();
    }


    /// <summary>
    /// Can the command be undone?
    /// </summary>
    public virtual bool canUndo()
    {
      return false;
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    void CadKit.Interfaces.ICommand.execute()
    {
      this.execute();
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public abstract void execute();


    /// <summary>
    /// Undo the command.
    /// </summary>
    void CadKit.Interfaces.ICommand.undo()
    {
      this.undo();
    }


    /// <summary>
    /// Undo the command.
    /// </summary>
    public void undo()
    {
    }


    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected virtual bool _shouldBeEnabled()
    {
      return true;
    }


    /// <summary>
    /// Determine if the button is checked.
    /// </summary>
    protected virtual bool _isChecked()
    {
      return false;
    }


    /// <summary>
    /// Is there a menu button?
    /// </summary>
    public bool HasMenuButton
    {
      get
      {
        using (this.Lock.read())
        {
          return (null != _menuButton);
        }
      }
    }


    /// <summary>
    /// Is there a tool bar button?
    /// </summary>
    public bool HasToolButton
    {
      get
      {
        using (this.Lock.read())
        {
          return (null != _toolButton);
        }
      }
    }


    /// <summary>
    /// Get the menu button.
    /// </summary>
    public System.Windows.Forms.ToolStripMenuItem MenuButton
    {
      get
      {
        if (false == this.HasMenuButton)
        {
          this._createMenuButton();
        }
        using (this.Lock.read())
        {
          return _menuButton;
        }
      }
      set
      {
        using (this.Lock.write())
        {
          _menuButton = value;
        }
      }
    }


    /// <summary>
    /// Get the toolbar button.
    /// </summary>
    public System.Windows.Forms.ToolStripButton ToolButton
    {
      get
      {
        if (false == this.HasToolButton)
        {
          this._createToolButton();
        }
        using (this.Lock.read())
        {
          return _toolButton;
        }
      }
      set
      {
        using (this.Lock.write())
        {
          _toolButton = value;
        }
      }
    }


    /// <summary>
    /// Create the menu button.
    /// </summary>
    private void _createMenuButton()
    {
      System.Windows.Forms.ToolStripMenuItem menuButton = new System.Windows.Forms.ToolStripMenuItem();
      menuButton.Text = _text;
      menuButton.Click += this._onButtonClicked;
      menuButton.Paint += this._onButtonPaint;
      menuButton.ShortcutKeys = _keys;
      menuButton.Image = _menuIcon;
      menuButton.Enabled = false;
      this.MenuButton = menuButton;
    }


    /// <summary>
    /// Create the tool bar button.
    /// </summary>
    private void _createToolButton()
    {
      System.Windows.Forms.ToolStripButton toolButton = new System.Windows.Forms.ToolStripButton();
      toolButton.Click += this._onButtonClicked;
      toolButton.Paint += this._onButtonPaint;
      toolButton.Image = _toolIcon;
      toolButton.Enabled = false;
      toolButton.ToolTipText = this._toolTipText;
      this.ToolButton = toolButton;
    }


    /// <summary>
    /// Called when the button is pressed.
    /// </summary>
    private void _onButtonClicked(object sender, System.EventArgs args)
    {
      try
      {
        // Execute the command.
        this.execute();

        // Update all commands.
        Command.updateAll();
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1887520459: {0}", e.Message);
      }
    }


    /// <summary>
    /// Called when the button is about to be shown.
    /// </summary>
    private void _onButtonPaint(object sender, System.Windows.Forms.PaintEventArgs args)
    {
      try
      {
        System.Windows.Forms.ToolStripItem item = sender as System.Windows.Forms.ToolStripItem;
        if (null != item)
        {
          item.Enabled = this._shouldBeEnabled();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2046322558: {0}", e.Message);
      }
    }


    /// <summary>
    /// Get the lock.
    /// </summary>
    protected CadKit.Threads.Tools.Lock Lock
    {
      get
      {
        // If this gets called from the finalizer then the lock may have 
        // already been destroyed and set to null.
        if (null == _lock)
        {
          _lock = new CadKit.Threads.Tools.Lock();
        }
        return _lock;
      }
    }


    /// <summary>
    /// Called when the active view changes.
    /// </summary>
    protected void _onActiveViewChanged(CadKit.Interfaces.IDocumentView oldView, CadKit.Interfaces.IDocumentView newView)
    {
      try
      {
        this.update();
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3503365250: {0}", e.Message);
      }
    }


    /// <summary>
    /// Update the tool button.
    /// </summary>
    private void _updateToolButton()
    {
      try
      {
        // Do not call this.ToolButton! We do not want to make one.
        if (true == this.HasToolButton)
        {
          System.Windows.Forms.ToolStrip owner = this.ToolButton.Owner;
          if (null != owner)
          {
            if (true == owner.InvokeRequired)
            {
              owner.BeginInvoke(new VoidReturnZeroArgumentsDelegate(this._updateToolButton));
            }
            else
            {
              bool state = this._isChecked();
              this.ToolButton.CheckState = (state) ? System.Windows.Forms.CheckState.Checked : System.Windows.Forms.CheckState.Unchecked;
              this.ToolButton.Checked = state;
              this.ToolButton.Invalidate();
            }
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3875510231: {0}", e.Message);
      }
    }


    /// <summary>
    /// Update the menu button.
    /// </summary>
    private void _updateMenuButton()
    {
      try
      {
        // Do not call this.ToolButton! We do not want to make one.
        if (true == this.HasMenuButton)
        {
          System.Windows.Forms.ToolStrip owner = this.MenuButton.Owner;
          if (null != owner)
          {
            if (true == owner.InvokeRequired)
            {
              owner.BeginInvoke(new VoidReturnZeroArgumentsDelegate(this._updateMenuButton));
            }
            else
            {
              bool state = this._isChecked();
              this.MenuButton.CheckState = (state) ? System.Windows.Forms.CheckState.Checked : System.Windows.Forms.CheckState.Unchecked;
              this.MenuButton.Checked = state;
              this.MenuButton.Invalidate();
            }
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2674442530: {0}", e.Message);
      }
    }


    /// <summary>
    /// Update the buttons if they exist.
    /// </summary>
    public void update()
    {
      this._updateMenuButton();
      this._updateToolButton();
    }


    /// <summary>
    /// Update all the commands.
    /// </summary>
    public static void updateAll()
    {
      CommandList commandList = new CommandList();
      using (_commandLock.read())
      {
        commandList.AddRange(_commandList);
      }
      foreach (Command command in commandList)
      {
        command.update();
      }
    }


    /// <summary>
    /// Increment the reference count.
    /// </summary>
    void CadKit.Interfaces.IReferenced.reference()
    {
      // TODO
    }


    /// <summary>
    /// Decrement the reference count.
    /// </summary>
    void CadKit.Interfaces.IReferenced.dereference()
    {
      // TODO
    }


    /// <summary>
    /// Decrement the reference count.
    /// </summary>
    void CadKit.Interfaces.IReferenced.dereference(bool allowCleanup)
    {
      // TODO
    }


    /// <summary>
    /// Return the reference count.
    /// </summary>
    uint CadKit.Interfaces.IReferenced.RefCount
    {
      get { return 0; } // TODO
    }
  }
}
