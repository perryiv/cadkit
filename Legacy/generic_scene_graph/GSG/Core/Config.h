
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Configuration file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_CONFIG_H_
#define _GENERIC_SCENE_GRAPH_CORE_CONFIG_H_

#if _MFC_VER <= 1200
# pragma warning ( disable : 4290 ) // Exception Specification ignored.
#endif

#include "GSG/Core/CoreDll.h"
#include "GSG/Core/Pointer.h"
#include "GSG/Core/Matrix.h"
#include "GSG/Core/Vector2.h"
#include "GSG/Core/Vector3.h"
#include "GSG/Core/Vector4.h"
#include "GSG/Core/Viewport.h"
#include "GSG/Core/BoundingBox.h"
#include "GSG/Core/BoundingSphere.h"
#include "GSG/Core/Predicates.h"
#include "GSG/Core/Threads.h"
#include "GSG/Core/ErrorPolicy.h"

#include <utility>
#include <string>
#include <vector>
#include <map>
#include <list>


namespace GSG {


///////////////////////////////////////////////////////////////////////////////
//
//  These cannot be typedefs.
//
///////////////////////////////////////////////////////////////////////////////

#define GSG_BIDIRECTIONAL_CONTAINER std::list
#define GSG_RANDOM_ACCESS_CONTAINER std::vector


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs in namespace GSG.
//
///////////////////////////////////////////////////////////////////////////////

typedef Detail::SingleThreaded Lock;
typedef double Real;
typedef int SignedInteger;
typedef unsigned int UnsignedInteger;
typedef UnsignedInteger Index;
typedef UnsignedInteger ReferenceCount;
typedef UnsignedInteger MessageId;
typedef Detail::AssertBoundsChecker BoundsChecker;
typedef Detail::Vector2 < Real, Index, BoundsChecker > Vec2;
typedef Detail::Vector3 < Real, Index, BoundsChecker > Vec3;
typedef Detail::Vector4 < Real, Index, BoundsChecker > Vec4;
typedef GSG_RANDOM_ACCESS_CONTAINER < Index > Indices;
typedef GSG_RANDOM_ACCESS_CONTAINER < Vec4 > SeqVec4;
typedef GSG_RANDOM_ACCESS_CONTAINER < Vec3 > SeqVec3;
typedef GSG_RANDOM_ACCESS_CONTAINER < Vec2 > SeqVec2;
typedef std::map < Vec4, Indices::size_type, Detail::LessVec4 > MapVec4;
typedef std::map < Vec3, Indices::size_type, Detail::LessVec3 > MapVec3;
typedef std::map < Vec2, Indices::size_type, Detail::LessVec2 > MapVec2;
typedef Detail::Matrix < Real, Index, BoundsChecker > Matrix;
typedef Detail::Viewport < SignedInteger, UnsignedInteger > Viewport;
typedef Detail::BoundingBox < Vec3 > BoundingBox;
typedef Detail::BoundingSphere < Vec3 > BoundingSphere;
typedef Detail::AssertErrorChecker ErrorChecker;
typedef Detail::AssertNullChecker NullCheckPolicy;
typedef Detail::NoNullChecker NullOkPolicy;
typedef Detail::NoReferenceCounting NoRefCountPolicy;
typedef Detail::ReferenceCounting RefCountPolicy;
typedef std::pair < std::string, void * > RootClass;
typedef Detail::Vector2 < SignedInteger, Index, BoundsChecker > MousePoint;


}; // namespace GSG


///////////////////////////////////////////////////////////////////////////////
//
//  Keep these headers down here.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/Core/Define.h"
#include "GSG/Core/Traits.h"


#endif // _GENERIC_SCENE_GRAPH_CORE_CONFIG_H_
