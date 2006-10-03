
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Viewer
{
  public class Viewer : System.Windows.Forms.Form
  {
    CadKit.Viewer.Panel _panel = new Panel();

    /// <summary>
    /// Constructor.
    /// </summary>
    public Viewer()
    {
      this.Controls.Add(_panel);
      _panel.Dock = System.Windows.Forms.DockStyle.Fill;

      this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(OnFormClosed);
    }


    /// <summary>
    /// The form is closed.  Clean up.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void OnFormClosed(object sender, System.Windows.Forms.FormClosedEventArgs e)
    {
      _panel.clear();
      _panel = null;
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~Viewer()
    {
      _panel = null;
    }


    /// <summary>
    /// Initalize.
    /// </summary>
    public void init()
    {
      _panel.init();

      this.BackColorChanged += new System.EventHandler(_panel.OnBackColorChanged);      
    }
  }
}
