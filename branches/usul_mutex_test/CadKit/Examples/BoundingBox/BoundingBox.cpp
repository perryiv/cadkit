
#include "Standard/SlBoundingBox.h"
using namespace CadKit;


void testBoundingBox()
{
  SlVec3d min ( 0, 0, 0 );
  SlVec3d max ( 2, 2, 2 );
  SlBoundingBoxd bd1 ( min, max );

  SlVec3d linePt ( 1, 2, 3 );
  SlVec3d lineVec ( 0, 0, -1 );
  lineVec.normalize();
  SlVec3d coord;
  SL_VERIFY ( true == bd1.intersect ( linePt, lineVec, coord ) );

  lineVec.setValue ( 1, 1, 1 );
  SL_VERIFY ( false == bd1.intersect ( linePt, lineVec, coord ) );

  SlVec3d center = bd1.getCenter();

  double w = bd1.getWidth();
  double h = bd1.getHeight();
  double d = bd1.getDepth();

  min.setValue ( 1, 1, 1 );
  max.setValue ( 4, 4, 4 );
  SlBoundingBoxd bd3 ( min, max );
  bd1.grow ( bd3 );

  SlMatrix44d M ( true );
  SlBoundingBoxd bd4 = M * bd1;

  bd3 += bd1;
  bd4 = bd3 + bd1;
  bd4.setValue ( bd1 );

  bd1.transform ( M );

  SlBoundingBoxf bf1;
  bf1.init();
}


int main ( int argc, char **argv )
{
  testBoundingBox();
  return 1;
}
