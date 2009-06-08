
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins
{
  public partial class Manager
  {
#if no
    class DummyPlugin : CadKit.Interfaces.IPlugin
    {
      string _name = "";
      string _description = "";

      public DummyPlugin(string name, string description)
      {
      }

      public string Name
      {
        get { return _name; }
      }

      public string Description
      {
        get { return _description; }
      }

      public void startupNotify(object caller)
      {
        // Do nothing.
      }
    }

    public CadKit.Interfaces.IPlugin pluginInterface(string file)
    {
      System.Reflection.Assembly assembly = null;

      // Check to see if we have the plugin.
      _assemblies.TryGetValue(file, out assembly);

      if (assembly == null)
      {
        // See: http://msdn2.microsoft.com/en-US/library/7hcs6az6.aspx
        System.AppDomain domain = System.AppDomain.CreateDomain("CadKit.Plugins.Factory");
        object obj = domain.CreateInstanceFrom(file, "CadKit.Plugins.Factory");
        //domain.AssemblyResolve += new System.ResolveEventHandler(domain_AssemblyResolve);

        //assembly = domain.Load(file);
        //object obj = domain.CreateInstance(file, "CadKit.Plugins.Factory");

        //CadKit.Interfaces.IPlugin plugin = _getInterface(file, assembly);

        //DummyPlugin dummy = new DummyPlugin(plugin.Name, plugin.Description);

        System.AppDomain.Unload(domain);

        //return dummy;
        return new DummyPlugin("", "");
      }

      return _getInterface(file, assembly);
    }

    byte[] loadFile(string file)
    {
      System.IO.FileStream fs = new System.IO.FileStream(file, System.IO.FileMode.Open);
      byte[] buffer = new byte[(int)fs.Length];
      fs.Read(buffer, 0, buffer.Length);
      fs.Close();

      return buffer;
    }

    System.Reflection.Assembly domain_AssemblyResolve(object sender, System.ResolveEventArgs args)
    {
      System.AppDomain domain = (System.AppDomain)sender;

      byte[] rawAssembly = loadFile(args.Name);
      //byte[] rawSymbolStore = loadFile("temp.pdb");
      //Assembly assembly = domain.Load(rawAssembly, rawSymbolStore);
      return domain.Load(rawAssembly);
    }

    private static CadKit.Interfaces.IPlugin _getInterface(string file, System.Reflection.Assembly assembly)
    {
      CadKit.Interfaces.IClassFactory factory = (CadKit.Interfaces.IClassFactory)assembly.CreateInstance("CadKit.Plugins.Factory");
      if (null == factory)
      {
        throw new System.Exception("Error 1407956654: Assembly file " + file + " does not have a class factory");
      }

      CadKit.Interfaces.IPlugin plugin = (CadKit.Interfaces.IPlugin)factory.createInstance("CadKit.Interfaces.IPlugin");
      if (null == plugin)
      {
        throw new System.Exception("Error 1689831654: Failed to create plugin instance in assembly file: " + file);
      }
      return plugin;
    }
#endif
  }
}
