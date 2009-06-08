///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Text;

namespace VaporIntrusionDotNet
{
  class GridSpace
  {

    //*************************************************************************************
    //
    // Structs
    //
    //*************************************************************************************

    public class GridComponent
    {
      public List<Component> component;
      public GridComponent()
      {
        component = new List<Component>();
      }
    };

    //*************************************************************************************
    //
    // Member Variables
    //
    //*************************************************************************************

    private static List<GridComponent> _gridSpace;
    private static Helper.Vector3 _dimensions;

    //*************************************************************************************
    //
    // Initializer
    //
    //*************************************************************************************
    
    public static void initialize()
    {
      // set the initial dimensions
      _dimensions.x = 100;
      _dimensions.y = 100;
      _dimensions.z = 100;

      // initialize the grid space
      _gridSpace = new List<GridComponent>();     
      
      // resize the grid space
      int size = _dimensions.x * _dimensions.y * _dimensions.z;

      // add the default <empty> component to the grid space
      for (int i = 0; i < size; ++i)
      {
        GridComponent empty = new GridComponent();
        _gridSpace.Add(empty);
      }
      

    }


    //*************************************************************************************
    //
    // Get/Set the dimensions of the grid space
    //
    //*************************************************************************************

    public static Helper.Vector3 dimensions
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
        _gridSpace = new List<GridComponent>(_dimensions.x * _dimensions.y * dimensions.z);

      }
    }


    //*************************************************************************************
    //
    // Set the grid component at i,j,k to component <c>
    //
    //*************************************************************************************

    public static void setComponentAt( int i, int j, int k, Component c )
    {
      // index ( j + ( i * d[1] ) + ( k * d[0] * d[1] ) );
      int index =  i + ( j * _dimensions.x ) + ( k * _dimensions.x * _dimensions.y );

      if (_gridSpace.Count > index)
      {
        _gridSpace[index].component.Add( c );
      }
      else
      {
        // throw here
      }
    }

  }
}
