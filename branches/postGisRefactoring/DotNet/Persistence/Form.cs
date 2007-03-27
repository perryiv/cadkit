
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Persistence
{
  public class Form : System.Windows.Forms.Form
  {
    public Form()
    {
      this.Load += _formLoaded;
      this.FormClosing += _formClosing;
    }

    private void _formClosing( object sender, System.Windows.Forms.FormClosingEventArgs e )
    {
      lock (_mutex)
      {
        if (false == _hasWritten)
        {
          CadKit.Persistence.FormConfig.writeRect(this, this.PersistentName);
          _hasWritten = true;
        }
      }
    }

    private void _formLoaded( object sender, System.EventArgs e )
    {
      lock (_mutex)
      {
        CadKit.Persistence.FormConfig.readRect(this, this.PersistentName);

        // Catch when owner windows are closed.
        System.Windows.Forms.Form owner = this.Owner;
        while (null != owner)
        {
          owner.FormClosing += _formClosing;
          owner = owner.Owner;
        }
      }
    }

    public string PersistentName
    {
      get { lock (_mutex) { return _persistentName; } }
      set { lock (_mutex) { _persistentName = value; } }
    }

    private string _persistentName = typeof( Form ).ToString();
    private bool _hasWritten = false;
    protected object _mutex = new object();
  }
}
