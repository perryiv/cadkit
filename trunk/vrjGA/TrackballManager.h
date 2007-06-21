
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Managing class for a Trackball's input
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRJGA_TRACKBALL_MANAGER_H_
#define _VRJGA_TRACKBALL_MANAGER_H_

#include "vrjGA/Referenced.h"
#include "gadget/Type/Digital.h"
#include "gmtl/Matrix.h"
#include "gmtl/Ray.h"
#include "gmtl/Vec.h"

namespace vrjGA
{
  class ButtonDevice;
  class TrackerDevice;

	template <class TBType, class TrackerType, class ButtonType>
	class TrackballManager : public Referenced
	{
	public:
    typedef Referenced BaseClass;
    USUL_DECLARE_REF_POINTERS ( TrackballManager );

    TrackballManager(): BaseClass(),
      _tb(0x0), _wand(0x0), _btn(0x0) {}

    TrackballManager(TBType* tb, TrackerType* w, ButtonType* b): BaseClass(),
      _tb(tb), _wand(w), _btn(b) {}

    TrackballManager(const TrackballManager& tbm): BaseClass(),
      _tb(tbm._tb), _wand(tbm.wand), _btn(tbm._btn) {}

    TrackballManager& operator = (const TrackballManager& tbm)
    {
      _tb = tbm._tb;
      _wand = tbm._wand;
      _btn = tbm._btn;
    }

    void setActionButton(ButtonType* b) { _btn = b; }
    const ButtonType* getActionButton() const { return _btn; }

    void setTracker(TrackerType* w) { _wand = w; }
    const TrackerType* getTracker() const { return _wand; }

    void setTrackball(TBType* tb) { _tb = _tb; }
    const TBType* getTrackball() const { return _tb; }

    // called every frame
    inline void update()
    {
      _tb->setTime( _wand->time() );

      if( _btn->state() == gadget::Digital::ON )
      {
        gmtl::Matrix44f wand_matrix = _wand->matrix();
        gmtl::Vec3f RayOrigin = gmtl::makeTrans<gmtl::Vec3f, gmtl::Matrix44f>( wand_matrix );
        gmtl::Vec3f RayDirection = wand_matrix * gmtl::Vec3f(0.0, 0.0, -1.0);

        gmtl::Rayf TBray(RayOrigin, RayDirection);
        _tb->input(TBray);
      }

      else
      {
        _tb->update();
      }
    }

  protected:
    ~TrackballManager() {}

  private:
    TBType* _tb;
    TrackerType* _wand;
    ButtonType* _btn;
  };
};

#endif
