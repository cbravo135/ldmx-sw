/**
 * @file MipCluster.h
 * @author Tom Eichlersmith, University of Minnesota
 * @brief Header file for class MipCluster
 */

#ifndef HCAL_MIPCLUSTER_H
#define HCAL_MIPCLUSTER_H

//Standard Libraries
#include <vector> //for storage vector of HcalHit pointers

//LDMX Framework
#include "DetDescr/HcalID.h" //for the HcalSection enum
#include "Tools/HcalDetectorGeometry.h" //to calculate real space point
#include "Tools/HitBox.h" //to represent real space point

namespace ldmx {
    
    /**
     * @class MipCluster
     * @brief Stores pointers to HcalHits that are considered a single MIP hit (usually due to proximity).
     */
    class MipCluster {
        public:
            /**
             * Default Constructor
             */
            MipCluster();

            /**
             * Add an HcalHit to the MipCluster
             */
            void addHit( HcalHit* hit );
            
            /**
             * Merges the input cluster into this cluster
             */
            void mergeCluster( const MipCluster &cluster );

            /**
             * Re-calculate member variables that depend on the hits.
             * This should be called whenever the hcalHits vector changes.
             */
            void set();
            
            /**
             * Get the total energy of the MipCluster
             */
            float getEnergy() const { return totalEnergy_; }

            /**
             * Get the number of HcalHits in this MipCluster
             */
            int getNumHits() const { return hcalHits_.size(); }

            /**
             * Get a hit in this MipCluster
             */
            HcalHit* getHcalHit( const int i ) const { return hcalHits_.at( i ); }

            /**
             * Get the real space point and errors in each coordinate.
             */
            HitBox getBox() const;

            /**
             * Set the uniqe event id.
             */
            void setUID( const unsigned int id ) { uID_ = id; return; }

            /**
             * Get the unique event id.
             */
            unsigned int getUID() const { return uID_; }

            /**
             * Set whether this cluster has been used as a seed
             */
            void wasBadSeed( const bool wasBadSeed = true ) { wasBadSeed_ = wasBadSeed; return; }

            /**
             * Get whether this cluster is a good seed
             */
            bool isGoodSeed() const { return !wasBadSeed_; }

        private:
            
            /**
             * Calculate the total energy of the MipCluster.
             */
            void setTotalEnergy();

            /** The total energy of the MipCluster */
            float totalEnergy_;

            /** ID that is unique in a single event */
            unsigned int uID_;

            /** Flag to say it has been checked as a seed */
            bool wasBadSeed_;

            /** Class instance to calculate real space pont 
             *
             * Anyone know how to make a self-defined class static?
             * That would be helpful here so the O(100) different MipClusters
             * in each event can share one calculator.
             */
            HcalDetectorGeometry hdg_;

            /** Storage vector of pointers to HcalHits */
            std::vector< HcalHit* > hcalHits_;

            /** Real Space point representing cluster in the form of axis-aligned box */
            HitBox box_;
    };

}

#endif /* HCAL_MIPCLUSTER_H */
