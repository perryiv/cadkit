
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for detecting and using an InterSense tracker.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _INTERSENSE_TRACKER_LIBRARY_EXCEPTIONS_H_
#define _INTERSENSE_TRACKER_LIBRARY_EXCEPTIONS_H_


namespace InterSense {


struct TrackerException{};
struct FailedToConnect             : public TrackerException{};
struct FailedToUpdateStation       : public TrackerException{};
struct UnsupportedTrackerModel     : public TrackerException{};
struct FailedToDetectTrackerConfig : public TrackerException{};
struct FailedToDetectStationConfig : public TrackerException{};
struct FailedToConfigureStation    : public TrackerException{};
struct FailedToGetTrackerData      : public TrackerException{};
struct UnknownUnits                : public TrackerException{};
struct InvalidButtonID             : public TrackerException{};


}; // namespace InterSense


#endif // _INTERSENSE_TRACKER_LIBRARY_EXCEPTIONS_H_
