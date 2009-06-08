The examples on this page assume
<?php write_cpp ( "Curve" ); ?> and
<?php write_cpp ( "Surface" ); ?> are
<?php write_cpp ( "typedef" ); ?>'d similar to this:
<pre>
  #include "GN/Config/UsulConfig.h"
  #include "GN/Splines/Curve.h"
  #include "GN/Splines/Surface.h"

  typedef GN::Config::Usul::Vector&lt;double&gt Config;
  typedef GN::Splines::Spline&lt;Config&gt      Spline;
  typedef GN::Splines::Curve&lt;Config&gt       Curve;
  typedef GN::Splines::Surface&lt;Config&gt     Surface;
</pre>
