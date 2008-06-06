
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgMessageIds.h: Possible message id values.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_MESSAGE_ID_VALUES_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_MESSAGE_ID_VALUES_H_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlDeclareConst.h"
#endif

namespace CadKit
{
// Misc.
const SlResult SG_NODE_FOUND =                                SL_CONST_UINT64 ( 0x36d0b3e08b2c11d3 );

// 2D Manipulator start.
const SlResult SG_MANIP_2D_TOP_LEFT_TRANSLATE_START =         SL_CONST_UINT64 ( 0x36d0b3e18b2c11d3 );
const SlResult SG_MANIP_2D_TOP_MIDDLE_TRANSLATE_START =       SL_CONST_UINT64 ( 0x36d0b3e28b2c11d3 );
const SlResult SG_MANIP_2D_TOP_RIGHT_TRANSLATE_START =        SL_CONST_UINT64 ( 0x36d0b3e38b2c11d3 );
const SlResult SG_MANIP_2D_MIDDLE_LEFT_TRANSLATE_START =      SL_CONST_UINT64 ( 0x36d0b3e48b2c11d3 );
const SlResult SG_MANIP_2D_MIDDLE_RIGHT_TRANSLATE_START =     SL_CONST_UINT64 ( 0x36d0b3e58b2c11d3 );
const SlResult SG_MANIP_2D_BOTTOM_LEFT_TRANSLATE_START =      SL_CONST_UINT64 ( 0x36d0b3e68b2c11d3 );
const SlResult SG_MANIP_2D_BOTTOM_MIDDLE_TRANSLATE_START =    SL_CONST_UINT64 ( 0x36d0b3e78b2c11d3 );
const SlResult SG_MANIP_2D_BOTTOM_RIGHT_TRANSLATE_START =     SL_CONST_UINT64 ( 0x36d0b3e88b2c11d3 );
const SlResult SG_MANIP_2D_BORDER_TRANSLATE_START =           SL_CONST_UINT64 ( 0x36d0b3e98b2c11d3 );

// 2D Manipulator move.
const SlResult SG_MANIP_2D_TOP_LEFT_TRANSLATE_MOVE =          SL_CONST_UINT64 ( 0x36d0b3ea8b2c11d3 );
const SlResult SG_MANIP_2D_TOP_MIDDLE_TRANSLATE_MOVE =        SL_CONST_UINT64 ( 0x36d0b3eb8b2c11d3 );
const SlResult SG_MANIP_2D_TOP_RIGHT_TRANSLATE_MOVE =         SL_CONST_UINT64 ( 0x36d0b3ec8b2c11d3 );
const SlResult SG_MANIP_2D_MIDDLE_LEFT_TRANSLATE_MOVE =       SL_CONST_UINT64 ( 0x36d0b3ed8b2c11d3 );
const SlResult SG_MANIP_2D_MIDDLE_RIGHT_TRANSLATE_MOVE =      SL_CONST_UINT64 ( 0x36d0b3ee8b2c11d3 );
const SlResult SG_MANIP_2D_BOTTOM_LEFT_TRANSLATE_MOVE =       SL_CONST_UINT64 ( 0x36d0b3ef8b2c11d3 );
const SlResult SG_MANIP_2D_BOTTOM_MIDDLE_TRANSLATE_MOVE =     SL_CONST_UINT64 ( 0x36d0b3f08b2c11d3 );
const SlResult SG_MANIP_2D_BOTTOM_RIGHT_TRANSLATE_MOVE =      SL_CONST_UINT64 ( 0x36d0b3f18b2c11d3 );
const SlResult SG_MANIP_2D_BORDER_TRANSLATE_MOVE =            SL_CONST_UINT64 ( 0x36d0b3f28b2c11d3 );

// 2D Manipulator finish.
const SlResult SG_MANIP_2D_TOP_LEFT_TRANSLATE_FINISH =        SL_CONST_UINT64 ( 0x36d0b3f38b2c11d3 );
const SlResult SG_MANIP_2D_TOP_MIDDLE_TRANSLATE_FINISH =      SL_CONST_UINT64 ( 0x36d0b3f48b2c11d3 );
const SlResult SG_MANIP_2D_TOP_RIGHT_TRANSLATE_FINISH =       SL_CONST_UINT64 ( 0x36d0b3f58b2c11d3 );
const SlResult SG_MANIP_2D_MIDDLE_LEFT_TRANSLATE_FINISH =     SL_CONST_UINT64 ( 0x36d0b3f68b2c11d3 );
const SlResult SG_MANIP_2D_MIDDLE_RIGHT_TRANSLATE_FINISH =    SL_CONST_UINT64 ( 0x36d0b3f78b2c11d3 );
const SlResult SG_MANIP_2D_BOTTOM_LEFT_TRANSLATE_FINISH =     SL_CONST_UINT64 ( 0x36d0b3f88b2c11d3 );
const SlResult SG_MANIP_2D_BOTTOM_MIDDLE_TRANSLATE_FINISH =   SL_CONST_UINT64 ( 0x36d0b3f98b2c11d3 );
const SlResult SG_MANIP_2D_BOTTOM_RIGHT_TRANSLATE_FINISH =    SL_CONST_UINT64 ( 0x36d0b3fa8b2c11d3 );
const SlResult SG_MANIP_2D_BORDER_TRANSLATE_FINISH =          SL_CONST_UINT64 ( 0x36d0b3fb8b2c11d3 );

// Postit parts start.
const SlResult SG_POSTIT_ARROW_DRAG_START =                   SL_CONST_UINT64 ( 0x36d0b3fc8b2c11d3 );
const SlResult SG_POSTIT_ELLIPSE_DRAG_START =                 SL_CONST_UINT64 ( 0x36d0b3fd8b2c11d3 );
const SlResult SG_POSTIT_TEXT_DRAG_START =                    SL_CONST_UINT64 ( 0x36d0b3fe8b2c11d3 );

// Postit parts move.
const SlResult SG_POSTIT_ARROW_DRAG_MOVE =                    SL_CONST_UINT64 ( 0x36d0b3ff8b2c11d3 );
const SlResult SG_POSTIT_ELLIPSE_DRAG_MOVE =                  SL_CONST_UINT64 ( 0x36d0b4008b2c11d3 );
const SlResult SG_POSTIT_TEXT_DRAG_MOVE =                     SL_CONST_UINT64 ( 0x36d0b4018b2c11d3 );

// Postit parts finish.
const SlResult SG_POSTIT_ARROW_DRAG_FINISH =                  SL_CONST_UINT64 ( 0x36d0b4028b2c11d3 );
const SlResult SG_POSTIT_ELLIPSE_DRAG_FINISH =                SL_CONST_UINT64 ( 0x36d0b4038b2c11d3 );
const SlResult SG_POSTIT_TEXT_DRAG_FINISH =                   SL_CONST_UINT64 ( 0x36d0b4048b2c11d3 );

// Axes parts start.
const SlResult SG_AXES_X_AXIS_DRAG_START =                    SL_CONST_UINT64 ( 0x36d0b4058b2c11d3 );
const SlResult SG_AXES_Y_AXIS_DRAG_START =                    SL_CONST_UINT64 ( 0x36d0b4068b2c11d3 );
const SlResult SG_AXES_X_LABEL_DRAG_START =                   SL_CONST_UINT64 ( 0x36d0b4078b2c11d3 );
const SlResult SG_AXES_Y_LABEL_DRAG_START =                   SL_CONST_UINT64 ( 0xa80632008b2c11d3 );
const SlResult SG_AXES_TITLE_DRAG_START =                     SL_CONST_UINT64 ( 0xd1e5425b8b2911d3 );

// Axes parts move.
const SlResult SG_AXES_X_AXIS_DRAG_MOVE =                     SL_CONST_UINT64 ( 0xd1e5425a8b2911d3 );
const SlResult SG_AXES_Y_AXIS_DRAG_MOVE =                     SL_CONST_UINT64 ( 0xd1e542598b2911d3 );
const SlResult SG_AXES_X_LABEL_DRAG_MOVE =                    SL_CONST_UINT64 ( 0xd1e542588b2911d3 );
const SlResult SG_AXES_Y_LABEL_DRAG_MOVE =                    SL_CONST_UINT64 ( 0xd1e542578b2911d3 );
const SlResult SG_AXES_TITLE_DRAG_MOVE =                      SL_CONST_UINT64 ( 0xd1e542568b2911d3 );

// Axes parts finish.
const SlResult SG_AXES_X_AXIS_DRAG_FINISH =                   SL_CONST_UINT64 ( 0xd1e542558b2911d3 );
const SlResult SG_AXES_Y_AXIS_DRAG_FINISH =                   SL_CONST_UINT64 ( 0xd1e542548b2911d3 );
const SlResult SG_AXES_X_LABEL_DRAG_FINISH =                  SL_CONST_UINT64 ( 0xd1e542538b2911d3 );
const SlResult SG_AXES_Y_LABEL_DRAG_FINISH =                  SL_CONST_UINT64 ( 0xd1e542528b2911d3 );
const SlResult SG_AXES_TITLE_DRAG_FINISH =                    SL_CONST_UINT64 ( 0xd1e542518b2911d3 );

// Viewer messages.
const SlResult SG_VIEWER_SPIN_START =                         SL_CONST_UINT64 ( 0xdb411f40077611d5 );
const SlResult SG_VIEWER_SPIN_MOVE =                          SL_CONST_UINT64 ( 0x05a70c10077711d5 );
const SlResult SG_VIEWER_SPIN_FINISH =                        SL_CONST_UINT64 ( 0x05a70c11077711d5 );

const SlResult SG_VIEWER_ROTATING =                           SL_CONST_UINT64 ( 0xa65383d0084711d5 );
const SlResult SG_VIEWER_TRANSLATING =                        SL_CONST_UINT64 ( 0xa65383d1084711d5 );
const SlResult SG_VIEWER_SCALING =                            SL_CONST_UINT64 ( 0xa65383d2084711d5 );

const SlResult SG_VIEWER_VIEWING_MODE =                       SL_CONST_UINT64 ( 0xa65383d3084711d5 );
const SlResult SG_VIEWER_PICKING_MODE =                       SL_CONST_UINT64 ( 0xa65383d4084711d5 );
const SlResult SG_VIEWER_SEEKING_MODE =                       SL_CONST_UINT64 ( 0xa65383d5084711d5 );

const SlResult SG_VIEWER_SEEK_MISS =                          SL_CONST_UINT64 ( 0xa65383d6084711d5 );
const SlResult SG_VIEWER_SEEK_HIT =                           SL_CONST_UINT64 ( 0xa65383d7084711d5 );


}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_MESSAGE_ID_VALUES_H_
