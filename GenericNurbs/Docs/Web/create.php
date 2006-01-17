<html>
<?php
  $page_title = "G-NURBS -- Create";
  $content_title = "G-NURBS -- Create";
  require ( "start_page.php" );
?>
<blockquote>
  <p>
    Using G-NURBS, there are many ways to create a valid curve, surface, 
    or general spline with <i>n</i>-number of independent variables.
    
    All of the creation methods are template functions in 
    <?php write_cpp ( "namespace GN::Create" ); ?>.
  </p>
  <p>
    <?php require "example_common.php" ?>
  </p>
</blockquote>
  <br /><?php require "horizontal_rule.php" ?><br />
<blockquote>
  <h3>Line</h3>
  <p>
    To make a 2D line:
    <pre>
      Curve c;
      Curve::Vec2 start ( 1, 2 ), end ( 2, 3 );
      GN::Create::line ( c, start, end );
    </pre>
    To make a 3D line:
    <pre>
      Curve c;
      Curve::Vec3 start ( 1, 2, 3 ), end ( 2, 3, 4 );
      GN::Create::line ( c, start, end );
    </pre>
  </p>
</blockquote>
  <br /><?php require "horizontal_rule.php" ?><br />
<blockquote>
  <h3>Circle</h3>
  <p>
    To make a 2D circle:
    <pre>
      Curve c;
      Curve::Vec2 center ( 1, 2 );
      Curve::ControlPointType radius = 10;
      GN::Create::circle ( c, center, radius );
    </pre>
    To make a 3D circle:
    <pre>
      Curve c;
      Curve::Vec3 center ( 1, 2, 3 );
      Curve::ControlPointType radius = 10;
      GN::Create::circle ( c, center, radius );
    </pre>
    To make an <i>n</i>-dimensional circle at the origin with a unit radius:
    <pre>
      Curve c;
      Curve::UIntType dimension = n;
      GN::Create::circle ( c, dimension );
    </pre>
  </p>
</blockquote>
  <br /><?php require "horizontal_rule.php" ?><br />
<blockquote>
  <h3>Sphere</h3>
  <p>
    To make a 3D sphere:
    <pre>
      Surface s;
      Surface::Vec2 center ( 1, 2, 3 );
      Surface::ControlPointType radius = 10;
      GN::Create::sphere ( s, center, radius );
    </pre>
    To make an <i>n</i>-dimensional sphere at the origin with a unit radius:
    <pre>
      Surface s;
      Surface::UIntType dimension = n;
      GN::Create::sphere ( s, dimension );
    </pre>
  </p>
</blockquote>
  <br /><?php require "horizontal_rule.php" ?><br />
<blockquote>
  <h3>Custom</h3>
  <p>
    You can directly set a spline's data. Here is how to construct a 3D curve:
    <pre>
      // Declare the spline and define its size.
      Spline s;
      Spline::UIntType numIndepVars = 1;
      Spline::UIntType dimension    = 3;

      // Even though we are making a curve, we have to define the 
      // number of control points and order in a general way.
      Spline::UIntContainer numCtrPts, order;
      numCtrPts.resize ( numIndepVars );
      order.resize     ( numIndepVars );
      numCtrPts[0]     = 9;
      order[0]         = 3;

      // We want rational curve.
      bool rational = true;

      // This will make sure the spline has enough room.
      s.resize ( dimension, order, numCtrPts, rational );

      // Set the knot vector. Pass 0 for "which independent variable".
      // With 9 control points and an order or 3, there will be 12 knots.
      s.knot(0,0) = ... 
      ...
      s.knot(0,11) = ...

      // Note: when setting the control points, the first zero 
      // specifies which dependent variable (e.g., x, y, or z).

      // Set all the wx-control points.
      s.controlPoint(0,0) = ...
      ...
      s.controlPoint(0,8) = ...

      // Set all the wy-control points.
      s.controlPoint(1,0) = ...
      ...
      s.controlPoint(1,8) = ...

      // Set all the wz-control points.
      s.controlPoint(2,0) = ...
      ...
      s.controlPoint(2,8) = ...

      // Set all the weights.
      s.weight(0) = ...
      ...
      s.weight(8) = ...
    </pre>
    If you use class
    <?php write_cpp ( "Curve" ); ?>
    instead of class
    <?php write_cpp ( "Spline" ); ?>
    then the function
    <?php write_cpp ( "resize()" ); ?>
    is a little more intuitive:
    <pre>
      // Declare the spline and define its size.
      Curve c;
      Curve::UIntType dimension    = 3;
      Curve::UIntType numCtrPts    = 9
      Curve::UIntType order        = 3;
      bool rational                = true;
      c.resize ( dimension, order, numCtrPts, rational );
    </pre>
    Also, with a 
    <?php write_cpp ( "Curve" ); ?>
    you do not need to specify which independent variable when setting the knots (there can be only one).
    Re-writing the above example, we get:
    <pre>
      s.knot(0) = ... 
      ...
      s.knot(11) = ...
    </pre>
  </p>
<?php require ( "end_page.php" ); ?>
</html>
