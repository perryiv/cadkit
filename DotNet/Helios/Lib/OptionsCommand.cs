
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class OptionsCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public OptionsCommand ( object caller )
    {
      _caller = caller;
      _text = "&Options...";
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
          CadKit.Helios.OptionsForm form = new CadKit.Helios.OptionsForm("CadKit.Helios.OptionsCommand._execute");
          form.Owner = CadKit.Helios.Application.Instance.MainForm;
          form.Icon = CadKit.Helios.Application.Instance.MainForm.Icon;
          form.Text = CadKit.Helios.Application.Instance.Name + " - Options";
          this._addPages(form);
          form.ShowDialog();
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 1648821079: Failed to show options dialog: {0}", e.Message);
        }
      }
    }

    /// <summary>
    /// All all the pages.
    /// </summary>
    private void _addPages(CadKit.Helios.OptionsForm form)
    {
      lock (_mutex)
      {
        try
        {
          CadKit.Interfaces.IOptionsPageAdd[] pages = CadKit.Plugins.Manager.Instance.getAll<CadKit.Interfaces.IOptionsPageAdd>();
          foreach (CadKit.Interfaces.IOptionsPageAdd page in pages)
          {
            try
            {
              this._addPage(form, page);
            }
            catch (System.Exception e)
            {
              System.Console.WriteLine("Error 4696038720: Failed to load option page: {0}", e.Message);
            }
          }
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 3413639661: Failed to load all option pages: {0}", e.Message);
        }
      }
    }

    /// <summary>
    /// All the page.
    /// </summary>
    private void _addPage(CadKit.Helios.OptionsForm form, CadKit.Interfaces.IOptionsPageAdd adder)
    {
      lock (_mutex)
      {
        if (null == form || null == adder)
          throw new System.ArgumentNullException("Error 3336043850: invalid arguments given");

        string name = adder.Name;
        if (null == name)
          throw new System.Exception("Error 2195698280: null name for the page");

        System.Drawing.Image image = adder.Image as System.Drawing.Image;
        string file = adder.Image as string;
        if (null == file && null == image)
          throw new System.Exception("Error 9877185980: no image for page");

        CadKit.Helios.OptionsPage p = null;
        if (null != file)
        {
          p = form.newPage(adder.Name, file);
        }
        if (null == p)
        {
          p = form.newPage(adder.Name, image);
        }
        if (null != p)
        {
          System.Windows.Forms.Control content = adder.contents(p) as System.Windows.Forms.Control;
          if (null == content)
            throw new System.Exception("Error 7244448840: null control for the page content");

          CadKit.Interfaces.IOptionsPage ipage = p as CadKit.Interfaces.IOptionsPage;
          if (null != ipage)
          {
            System.Windows.Forms.Control contents = ipage.Contents as System.Windows.Forms.Control;
            if ( null != contents )
            {
              form.add(ipage);
              contents.Controls.Add(content);
              content.Dock = System.Windows.Forms.DockStyle.Fill;
            }
          }
        }
      }
    }

    /// <summary>
    /// Determine of the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      return true;
    }
  }
}
