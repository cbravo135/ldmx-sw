#include "SimApplication/SimParticleBuilder.h"

// LDMX
#include "Event/Event.h"
#include "Event/EventConstants.h"
#include "SimApplication/G4CalorimeterHit.h"
#include "SimApplication/G4TrackerHit.h"
#include "SimApplication/UserTrackingAction.h"

// Geant4
#include "G4SystemOfUnits.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4VTrajectoryPoint.hh"

namespace ldmx {

    SimParticleBuilder::SimParticleBuilder() :
            currentEvent_(nullptr) {
        trackMap_ = UserTrackingAction::getUserTrackingAction()->getTrackMap();
        outputParticleColl_ = new TClonesArray(EventConstants::SIM_PARTICLE.c_str(), 50);
    }

    SimParticleBuilder::~SimParticleBuilder() {
        delete outputParticleColl_;
    }

    void SimParticleBuilder::buildSimParticles(ldmx::Event* outputEvent) {

        // Clear the output particle collection.
        outputParticleColl_->Clear("C");

        // Get the trajectory container for the event.
        TrajectoryContainer* trajectories = (TrajectoryContainer*) (const_cast<G4Event*>(currentEvent_))->GetTrajectoryContainer();

        // Create empty SimParticle objects and create the map of track ID to particles.
        buildParticleMap(trajectories, outputParticleColl_);

        // Fill information into the particles.
        for (auto trajectory : *trajectories->GetVector()) {
            buildSimParticle(static_cast<Trajectory*>(trajectory));
        }

        // Add the collection data to the output event.
        outputEvent->add("SimParticles", outputParticleColl_);
    }

    void SimParticleBuilder::buildSimParticle(Trajectory* traj) {

        SimParticle* simParticle = particleMap_[traj->GetTrackID()];

        if (!simParticle) {
            std::cerr << "[ SimParticleBuilder ] : SimParticle not found for Trajectory with track ID " << traj->GetTrackID() << std::endl;
            G4Exception("SimParticleBuilder::buildSimParticle", "", FatalException, "SimParticle not found for Trajectory.");
        }

        simParticle->setGenStatus(traj->getGenStatus());
        simParticle->setTrackID(traj->GetTrackID());
        simParticle->setPdgID(traj->GetPDGEncoding());
        simParticle->setCharge(traj->GetCharge());
        simParticle->setMass(traj->getMass());
        simParticle->setEnergy(traj->getEnergy());
        simParticle->setTime(traj->getGlobalTime());
        simParticle->setProcessType(traj->getProcessType());

        const G4ThreeVector& vertex = traj->getVertexPosition();
        simParticle->setVertex(vertex[0], vertex[1], vertex[2]);

        const G4ThreeVector& momentum = traj->GetInitialMomentum();
        simParticle->setMomentum(momentum[0], momentum[1], momentum[2]);

        const G4ThreeVector& endpMomentum = traj->getEndPointMomentum();
        simParticle->setEndPointMomentum(endpMomentum[0], endpMomentum[1], endpMomentum[2]);

        G4ThreeVector endpoint = traj->getEndPoint();
        simParticle->setEndPoint(endpoint[0], endpoint[1], endpoint[2]);

        if (traj->GetParentID() > 0) {
            SimParticle* parent = findSimParticle(traj->GetParentID());
            if (parent != nullptr) {
                simParticle->addParent(parent);
                parent->addDaughter(simParticle);
            } else {
                // If the parent particle can not be found by its track ID, this is a fatal error!
                std::cerr << "[ SimParticleBuilder ] : ERROR - SimParticle with parent ID " << traj->GetParentID() << " not found for track ID " << traj->GetTrackID() << std::endl;
                G4Exception("SimParticleBuilder::buildSimParticle", "", FatalException, "SimParticle not found from parent track ID.");
            }
        }
    }

    void SimParticleBuilder::buildParticleMap(TrajectoryContainer* trajectories, TClonesArray* simParticleColl) {
        particleMap_.clear();
        for (auto trajectory : *trajectories->GetVector()) {
            particleMap_[trajectory->GetTrackID()] = (SimParticle*) simParticleColl->ConstructedAt(simParticleColl->GetEntries());
        }
    }

    SimParticle* SimParticleBuilder::findSimParticle(G4int trackID) {
        G4VTrajectory* traj = trackMap_->findTrajectory(trackID);
        if (traj != nullptr) {
            return particleMap_[traj->GetTrackID()];
        } else {
            return nullptr;
        }
    }

}
