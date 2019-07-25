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

//// ECAL Geometry Constants, In mm
////      Hex_radius, ECal_dx, ECal_dy, ECal_dz
////      preshower_Thickness, layer_X_Thickness, ecal_X_layers
//static const double ecal_z_length = 290.0;
//static const double ecal_front_z = 200.0; // Distance from target to ECAL front face
//static const double ecal_xy  = 525.;
//
//static const std::vector<double> towerXPos = {0, 0, 0, 170*sqrt(3)/2, -170*sqrt(3)/2, -170*sqrt(3)/2, 170*sqrt(3)/2};
//static const std::vector<double> towerYPos = {0, 170, -170, 85, 85, -85, -85};
//// TODO recalculate layerZpos automatically
//static const double layerZPos[] = {2.8, 5.7, 12.05, 16.45, 24.3, 30.2, 39.3, 45.7, 54.8, 61.2, 70.3, 76.7, 85.8, 92.2, 101.3, 107.7, 116.8, 123.2, 132.3, 138.7, 147.8, 154.2, 163.3, 169.7, 182.3, 192.2, 204.8, 214.7, 227.3, 237.2, 249.8, 259.7, 272.3, 282.2}; // With respect to the front face of the ECAL

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
