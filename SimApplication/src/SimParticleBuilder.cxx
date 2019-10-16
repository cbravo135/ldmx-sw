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

    void SimParticleBuilder::buildSimParticles(IMPL::LCEventImpl* lcioEvent) {

        // Make new particle collection.
        auto collVec = new IMPL::LCCollectionVec(EVENT::LCIO::MCPARTICLE);

        // Get the trajectory container for the event.
        TrajectoryContainer* trajectories = (TrajectoryContainer*) (const_cast<G4Event*>(currentEvent_))->GetTrajectoryContainer();

        // Create the map of track ID to particles.
        buildParticleMap(trajectories, collVec);

        // Fill information into the particles.
        for (auto trajectory : *trajectories->GetVector()) {
            buildLcioParticle(static_cast<Trajectory*>(trajectory));
        }

        // Add the collection data to the output event.
        lcioEvent->addCollection(collVec, EVENT::LCIO::MCPARTICLE);
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

    void SimParticleBuilder::buildLcioParticle(Trajectory* traj) {

        IMPL::MCParticleImpl* lcioParticle = lcioParticleMap_[traj->GetTrackID()];

        if (!lcioParticle) {
            std::cerr << "[ SimParticleBuilder ] : SimParticle not found for Trajectory with track ID " << traj->GetTrackID() << std::endl;
            G4Exception("SimParticleBuilder::buildSimParticle", "", FatalException, "SimParticle not found for Trajectory.");
        }

        lcioParticle->setGeneratorStatus(traj->getGenStatus());
        //lcioParticle->setTrackID(traj->GetTrackID());
        lcioParticle->setPDG(traj->GetPDGEncoding());
        lcioParticle->setCharge(traj->GetCharge());
        lcioParticle->setMass(traj->getMass());
        //lcioParticle->setEnergy(traj->getEnergy());
        lcioParticle->setTime(traj->getGlobalTime());
        //lcioParticle->setProcessType(traj->getProcessType());

        double vertexArr[] ={traj->getVertexPosition()[0], traj->getVertexPosition()[1], traj->getVertexPosition()[2]};
        lcioParticle->setVertex(vertexArr);

        double momentum[] = { traj->GetInitialMomentum()[0] / GeV, traj->GetInitialMomentum()[1] / GeV,
            traj->GetInitialMomentum()[2] / GeV };
        lcioParticle->setMomentum(momentum);

        //const G4ThreeVector& endpMomentum = traj->getEndPointMomentum();
        //lcioParticle->setEndPointMomentum(endpMomentum[0], endpMomentum[1], endpMomentum[2]);

        double endp[] = { traj->getEndPoint()[0], traj->getEndPoint()[1], traj->getEndPoint()[2] };
        lcioParticle->setEndpoint(endp);

        if (traj->GetParentID() > 0) {
            IMPL::MCParticleImpl* parent = findLcioParticle(traj->GetParentID());
            if (parent != nullptr) {
                lcioParticle->addParent(parent);
                //parent->addDaughter(lcioParticle);
            } else {
                // If the parent particle can not be found by its track ID, this is a fatal error!
                std::cerr << "[ SimParticleBuilder ] : ERROR - SimParticle with parent ID " << traj->GetParentID() << " not found for track ID " << traj->GetTrackID() << std::endl;
                G4Exception("SimParticleBuilder::buildSimParticle", "", FatalException, "SimParticle not found from parent track ID.");
            }
        }
        // Set sim status to indicate particle was created in simulation.
        if (!traj->getGenStatus()) {
            std::bitset<32> simStatus;
            simStatus[EVENT::MCParticle::BITCreatedInSimulation] = 1;
            lcioParticle->setSimulatorStatus(simStatus.to_ulong());
        }
    }

    void SimParticleBuilder::buildParticleMap(TrajectoryContainer* trajectories, TClonesArray* simParticleColl) {
        particleMap_.clear();
        for (auto trajectory : *trajectories->GetVector()) {
            particleMap_[trajectory->GetTrackID()] = (SimParticle*) simParticleColl->ConstructedAt(simParticleColl->GetEntries());
        }
    }

    void SimParticleBuilder::buildParticleMap(TrajectoryContainer* trajectories, IMPL::LCCollectionVec* collVec) {
        lcioParticleMap_.clear();
        for (auto trajectory : *trajectories->GetVector()) {
            auto particle = new IMPL::MCParticleImpl();
            collVec->addElement(particle);
            lcioParticleMap_[trajectory->GetTrackID()] = particle;
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

    IMPL::MCParticleImpl* SimParticleBuilder::findLcioParticle(G4int trackID) {
        G4VTrajectory* traj = trackMap_->findTrajectory(trackID);
        if (traj != nullptr) {
            return lcioParticleMap_[traj->GetTrackID()];
        } else {
            return nullptr;
        }
    }
}
