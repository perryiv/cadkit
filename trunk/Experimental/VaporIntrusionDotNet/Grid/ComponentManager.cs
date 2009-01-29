//*************************************************************************************
//
// Member Variables
//
//*************************************************************************************


using System;
using System.Collections.Generic;
using System.Text;

namespace VaporIntrusionDotNet
{
  class ComponentManager
  {
    //*************************************************************************************
    //
    // Member Variables
    //
    //*************************************************************************************

    private static List<Component> _components;
    private static int _selectedComponent;
    private static Component _empty;

    //*************************************************************************************
    //
    // Initializer
    //
    //*************************************************************************************

    public static void initialize()
    {
      _components = new List<Component>();
      _selectedComponent = 0;

      _empty = new Component();
      _empty.color = System.Drawing.Color.Gray;

      // create and add a default component for testing
      Component defaultComponent = new Component("default", "null");

      // add the default component to the component list
      _components.Add(defaultComponent);
      
    }


    //*************************************************************************************
    //
    // Get material at index <index>
    //
    //*************************************************************************************

    public static Component getComponentAt(int index)
    {
      Component component = new Component();

      if (index < _components.Count)
      {
        component = _components[index];
      }

      return component;
    }


    //*************************************************************************************
    //
    // Get the current componennt
    //
    //*************************************************************************************

    public static Component currentComponent()
    {
      return getComponentAt(_selectedComponent);
    }


    //*************************************************************************************
    //
    // Add a component
    //
    //*************************************************************************************

    public static void add( Component c )
    {
      _components.Add( c );
    }


    //*************************************************************************************
    //
    // Add a component
    //
    //*************************************************************************************

    public static Component getEmptyComponent()
    {
      return _empty;
    }
  }
}
