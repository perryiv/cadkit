using System;
using System.Collections.Generic;
using System.Text;

namespace VaporIntrusionDotNet
{
  class GridSpace
  {

    //*************************************************************************************
    //
    // Member Variables
    //
    //*************************************************************************************

    private static List<Component> _gridSpace;
    private static Helper.Vector3 _dimensions;
    private static Helper.Vector3 _currentPositions;

    //*************************************************************************************
    //
    // Initializer
    //
    //*************************************************************************************
    
    public static void initialize()
    {
      // set the initial dimensions
      _dimensions.x = 10;
      _dimensions.y = 10;
      _dimensions.z = 10;

      // set the initial positions
      _currentPositions.x = 0;
      _currentPositions.y = 0;
      _currentPositions.z = 0;

      // resize the grid space
      _gridSpace = new List<Component>(_dimensions.x * _dimensions.y * _dimensions.z);     

    }


    //*************************************************************************************
    //
    // Get/Set the dimensions of the grid space
    //
    //*************************************************************************************

    public Helper.Vector3 dimensions
    {
      get
      {
        return _dimensions;
      }
      set
      {
        // Set the dimensions
        _dimensions = value;

        // erase and set the max number of elements for the grid space
        _gridSpace = new List<Component>(_dimensions.x * _dimensions.y * dimensions.z);

      }
    }


    //*************************************************************************************
    //
    // Get/Set the current positions of the grid space
    //
    //*************************************************************************************

    public Helper.Vector3 positions
    {
      get
      {
        return _currentPositions;
      }
      set
      {
        // Set the dimensions
        _currentPositions = value;
      }
    }

  }
}
