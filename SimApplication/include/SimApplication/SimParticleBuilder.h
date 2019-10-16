/**
 * @file SimParticleBuilder.h
 * @brief Class for building output SimParticle collection from trajectories
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef SIMAPPLICATION_SIMPARTICLEBUILDER_H_
#define SIMAPPLICATION_SIMPARTICLEBUILDER_H_

// LDMX
#include "Event/EventConstants.h"
#include "Event/Event.h"
#include "Event/SimParticle.h"
#include "SimApplication/TrackMap.h"
#include "SimApplication/Trajectory.h"
#include "SimApplication/TrajectoryContainer.h"

// LCIO
#include "EVENT/LCIO.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/MCParticleImpl.h"

// Geant4
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"

// STL
#include <map>

namespace ldmx {

    /**
     * @class SimParticleBuilder
     * @brief Builds output SimParticle collection from Trajectory container
     */
    class SimParticleBuilder {

        public:

            /**
             * Map of track ID to SimParticles.
             */
            typedef std::map<G4int, SimParticle*> SimParticleMap;

            /**
             * Class constructor.
             */
            SimParticleBuilder();

            /**
             * Class destructor.
             */
            virtual ~SimParticleBuilder();

            /**
             * Set the current Geant4 event.
             * @param anEvent The Geant4 event.
             */
            void setCurrentEvent(const G4Event* anEvent) {
                this->currentEvent_ = const_cast<G4Event*>(anEvent);
            }

            /**
             * Build SimParticle collection into an output event.
             * @param outputEvent The output event.
             */
            void buildSimParticles(Event* outputEvent);

            /**
             * Build SimParticle collection into an output lcio event.
             * @param outputEvent The output lcio event.
             */
            void buildSimParticles(IMPL::LCEventImpl* lcioEvent);

            /**
             * Find a SimParticle by track ID.
             * @param trackID The trackID of the particle.
             */
            SimParticle* findSimParticle(G4int trackID);

            /**
             * Find a SimParticle by track ID.
             * @param trackID The trackID of the particle.
             */
            IMPL::MCParticleImpl* findLcioParticle(G4int trackID);

            /**
             * Find a trajectory by its track ID.
             * If this track ID does not have a trajectory, then the
             * first trajectory found in its parentage is returned.
             * @param anEvent The Geant4 event.
             * @param trackkID The track ID of the trajectory to find.
             */
             G4VTrajectory* findTrajectory(G4int trackID){return trackMap_->findTrajectory(trackID);};
        private:

            /**
             * Build a SimParticle from trajectory information.
             * @param info The trajectory information.
             */
            void buildSimParticle(Trajectory* info);

            /**
             * Build a LcioParticle from trajectory information.
             * @param info The trajectory information.
             */
            void buildLcioParticle(Trajectory* info);

            /**
             * Build the SimParticle map from the trajectory container.
             * This will create SimParticles without their information filled.
             * @param trajectories The input trajectory container.
             * @param simParticleColl The output SimParticle collection.
             */
            void buildParticleMap(TrajectoryContainer* trajectories, TClonesArray* simParticleColl);

            /**
             * Build the SimParticle map from the trajectory container.
             * This will create SimParticles without their information filled.
             * @param trajectories The input trajectory container.
             * @param simParticleColl The output lcio collection vector.
             */
            void buildParticleMap(TrajectoryContainer* trajectories, IMPL::LCCollectionVec* collVec);

        private:

            /** The map of track IDs to SimParticles. */
            SimParticleMap particleMap_;

            /** The map of track IDs to LCIO IMPL::MCParticleImpl*. */
            std::map<G4int, IMPL::MCParticleImpl*> lcioParticleMap_;

            /** The map of tracks to their parent IDs and Trajectory objects. */
            TrackMap* trackMap_;

            /** The current Geant4 event. */
            G4Event* currentEvent_;

            /** The output SimParticle collection. */
            TClonesArray* outputParticleColl_{new TClonesArray(EventConstants::SIM_PARTICLE.c_str(), 50)};
    };

}

#endif
