#ifndef EVENTDISPLAY_DETECTORGEOMETRY_H_
#define EVENTDISPLAY_DETECTORGEOMETRY_H_

#include "TEveElement.h"
#include "EventDisplay/EveShapeDrawer.h"
#include "DetDescr/DetectorGeometry.h"
#include "DetDescr/HcalID.h" //for HcalSection enum

// Updated to v11 geometry on 7/24/2019

// Recoil Tracker Geometry Constants, In mm
static const double stereo_strip_length = 98.0; // 2 mm deadspace
static const double mono_strip_length = 78.0; // 2 mm deadspace
static const double stereo_x_width = 40.34;
static const double stereo_y_width = 100.0;
static const double mono_x_width = 50.0;
static const double mono_y_width = 80.0;
static const double recoil_sensor_thick = 0.52;
static const double stereo_sep = 3.0;
static const double mono_sep = 1.0;

static const std::vector<double> monoSensorXPos = {-2*mono_x_width, -mono_x_width, 0, mono_x_width, 2*mono_x_width};
static const std::vector<double> monoSensorYPos = {-mono_y_width/2, mono_y_width/2};
//layer Z positions relative to target
static const std::vector<double> recoilLayerZPos = {7.5, 22.5, 37.5, 52.5, 90.0, 180.0};

// In radians
static const double stereo_angle = 0.1*180/M_PI; 

namespace ldmx {

    class EveDetectorGeometry {

        public:

            EveDetectorGeometry();

            ~EveDetectorGeometry() {

                delete hcal_;
                delete sidehcal_;
                delete ecal_;
                delete recoilTracker_;
                delete detector_;
                delete shapeDrawer_;
            }

            void drawECAL();

            void drawHCAL();

            void drawRecoilTracker();

            TEveElement* getECAL() { return ecal_; }

            TEveElement* getHCAL() { return hcal_; }

            TEveElement* getRecoilTracker() { return recoilTracker_; }

            TEveElement* getDetector() { return detector_; }

        private:

            TEveElement* hcal_;
            TEveElement* sidehcal_;
            TEveElement* ecal_;
            TEveElement* recoilTracker_;
            TEveElement* detector_;

            EveShapeDrawer* shapeDrawer_;
    };
}

#endif
