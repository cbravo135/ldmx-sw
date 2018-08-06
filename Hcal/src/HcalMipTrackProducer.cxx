/**
 * @file HcalMipTrackProducer.cxx
 * @brief Implementation file for HcalMipTrackProducer class.
 * @author Tom Eichlersmith, University of Minnesota
 */

#include "Hcal/HcalMipTrackProducer.h"

namespace ldmx {

    void HcalMipTrackProducer::configure(const ldmx::ParameterSet& ps) {

        hcalHitCollName_ = ps.getString( "HcalHitCollectionName" );

        hcalHitPassName_ = ps.getString( "HcalHitPassName" );

        TClonesArray *hcalMipTracks_ = new TClonesArray( EventConstants::HCAL_MIP_TRACK.c_str() , 10000 );

        hcalMipTracksCollName_ = ps.getString( "HcalMipTrackCollectionName" );

        trackRadius_ = ps.getDouble( "MipTrackRadius" );

        minPE_ = ps.getDouble( "MinimumPE" );

        maxEnergy_ = ps.getDouble( "MaximumEnergy" );

        minNumClusters_ = ps.getInteger( "MinimumNumClusters" );
        
        //tracks are started with a pair of points, so
        //  the absolute minimum number of clusters in a track is 2
        if ( minNumClusters_ < 2 ) {
            minNumClusters_ = 2;
        } //minNumClusters_ validation check
        
        return;
    }

    void HcalMipTrackProducer::produce(ldmx::Event& event) {
        
        const TClonesArray *rawhits = event.getCollection( hcalHitCollName_ , hcalHitPassName_ );

        //go through raw hits and ignore noise hits
        int nhits = rawhits.GetEntriesFast();
        for ( int iH = 0; iH < nhits; iH++ ) {
            
            HcalHit* chit = dynamic_cast<HcalHit*>((rawhits->At( iH ));

            if ( isNotNoise( chit ) ) {
                int section = chit->getSection();
                int layer = chit->getLayer();
                int strip = chit->getStrip();

                unsigned int key = section*1000*100 + layer*100 + strip;

                hcalHitLog_[ key ] = chit;
            } //if not noise hit

        } //iterate through rawhits (iH)

        clusterHits();
       
                //store best in collection
        //delete best from cluster log

        //repeat track construction until no more pairs of clusters

        return;
    }

    bool HcalMipTrackProducer::isNotNoise( const HcalHit* hit ) const {
        return ( !chit->getNoise() and chit->getPE() > minPE_ );
    }

    bool HcalMipTrackProducer::isMip( const MipCluster* cluster ) const {
        return ( cluster->getEnergy() < maxEnergy_ );
    }

    void HcalMipTrackProducer::clusterHits() {
        
        // cluster hits in same layer
        std::map< unsigned int , HcalHit* >::iterator itH;
        std::map< unsigned int , HCalHit* >::iterator prev_itH = hcalHitLog_.begin();
        MipCluster *current_cluster = new MipCluster();
        for ( itH = hcalHitLog_.begin(); itH != hcalHitLog_.end(); ++itH ) {
            
            //itH and prev_itH will both point to hcalHitLog_.begin() on first loop
            unsigned int keydif = itH->first - prev_itH->first;

            if ( keydif > 1 ) {
                //current hit is in different cluster
                // add previous cluster to log and clear temporary cluster
                current_cluster->setUID( prev_itH->first );
                current_cluster->set(); //calculate real space point
                if ( isMip( current_cluster ) ) {
                    clusterLog_[ current_cluster->getUID() ] = *current_cluster;
                }//check if mip
                delete current_cluster;
                current_cluster = new MipCluster();
            }//check if separate cluster

            current_cluster->addHit( itH->second );

            itH++;
            prev_itH = std::prev( itH );

        }//iterate through sorted hcalHitLog (itH)

        //clean up at end of hit log
        if ( current_cluster->getNumHits() > 0 ) {
            current_cluster->setUID( prev_itH->first );
            current_cluster->set();
            clusterLog_[ current_cluster->getUID() ] = *current_cluster;
        }

        if ( current_cluster ) {
            delete current_cluster;
            current_cluster = nullptr;
        }

        // cluster across layers (if overlap)
     
    }
    
    bool lineHitBox( const std::vector<double> origin , const std::vector<double> dir , 
                     const std::vector<double> minBox , const std::vector<double> maxBox ) const {
        
        bool originInside = true;
        bool originBetween[3];

        //Determine planes that are on the "front" of the box w.r.t. the origin of the ray
        std::vector<double> candidatePlane( 3 , 0.0 );
        for ( unsigned int iC = 0; iC < 3; iC++ ) {
            
            if ( origin.at(iC) < minBox.at(iC) ) {
                originBetween[iC] = false;
                candidatePlane[iC] = minBox.at(iC);
                originInside = false;
            } else if ( origin.at(iC) > maxBox.at(iC) ) {
                originBetween[iC] = false;
                candidatePlane[iC] = maxBox.at(iC) );
                originInside = false;
            } else {
                originBetween[iC] = true;
            } //where origin is w.r.t. box
                
        } //iterate through coordinates (iC)
        
        //Origin Inside Box ==> Ray Intersects Box
        if ( originInside ) {
            return true;
        }

        //Calculate maximum T distances to candidatePlanes
        std::vector<double> maxT( 3 , 0.0 );
        for ( unsigned int iC = 0; iC < 3; iC++ ) {
            
            if ( !originBetween[iC] and dir.at(iC) != 0.0 ) {
                maxT[ iC ] = ( candidatePlane[iC] - origin.at(iC) ) / dir.at(iC);        
            } else {
                maxT[ iC ] = -1.0;
            }

        } //iterate through coordinates (iC)

        //Get largest of maxTs for the final choice of intersection
        unsigned int iMax = 0;
        for ( unsigned int iC = 0; iC < 3; iC++ ) {
            if ( maxT[ iMax ] < maxT[ iC ] ) {
                iMax = iC;
            }
        } //iterate through coordinates (iC)
        
        //Check if final candidate is inside box
        if ( maxT[ iMax ] < 0.0 ) {
            return false;
        }

        for ( unsigned int iC = 0; iC < 3; iC++ ) {
            
            if ( iMax != iC ) {
                double coordinate = origin.at(iC) + maxT[iC]*dir.at(iC);
                if ( coordinate < minBox.at(iC) or coordinate > maxBox.at(iC) ) {
                    //coordinate outside box
                    return false;
                }
            } //if coordinate is not maximum T plane
        } //iterate through coordinates (iC)

        return true;
    }

    bool HcalMipTrackProducer::compMipTracks( const HcalMipTrack &track1 , const HcalMipTrack &track2 ) const {
        
        bool better = false;
        if ( track1.isEmpty() ) {
            better = true;
        } else {
            //TEMPORARY
            better = ( track1.getEnergy() < track2.getEnergy() );
        } //if track1 is empty

        return better;
    }

    bool HcalMipTrackProducer::buildTrack( std::vector< unsigned int > &track_mipids ) {
        //for clusters:
        //  no suffix means that it isn't an endpoint
        //  suffice {1,2} means that it is one of the endpoints
        track_mipids.clear();
        //iterate through all pairs of points
        HcalMipTrack best_track;
        std::map< unsigned int , MipCluster >::iterator itC1, itC2, itC; //iterators for map
        for ( itC1 = clusterLog_.begin(); itC1 != clusterLog.end(); ++itC1 ) {
            for ( itC2 = itC1+1; itC2 != clusterLog_.end(); ++itC2 ) {
                //construct track in cylinder
                
                std::vector< double > point1, point2 , errors1 , errors2;
                (itC1->second).getPoint( point1 , errors1 );
                (itC2->second).getPoint( point2 , errors2 );

                std::vector< double > origin( 3 , 0.0 ), direction( 3 , 0.0 ), linesmudge( errors1 );
                for ( unsigned int iC = 0; iC < 3; iC++ ) {
                    direction[ iC ] = point2[iC] - point1[iC];
                }

                //project origin back to plane of maximum direction change
                // e.g. if direction has largest value in z then the origin will
                //  be put on xy-plane (z = 0)
                unsigned int iMax = 0;
                for ( unsigned int iC = 1; iC < 3; iC++ ) {
                    if ( std::abs(direction[iMax]) < std::abs(direction[iC]) ) {
                        iMax = iC;
                    }
                }

                //calculate origin and recalculate direction
                double originT = ( -point1[iMax] ) / direction[iMax];
                for ( unsigned int iC = 0; iC < 3; iC++ ) {
                    //calculate origin
                    origin[ iC ] = point1[ iC ] + originT*direction[ iC ];
                    //re-calculate direction
                    direction[ iC ] = point2[ iC ] - origin[ iC ];
                    //calculate smudge of line
                    if ( errors2[iC] < errors1[iC] ) {
                        linesmudge[iC] = errors2[iC];
                    }
                }
                
                std::vector< unsigned int > ctrack_mipids; //ids of mip clusters in track
                //iterate through all clusters to see if they are in track
                for ( itC = clusterLog_.begin(); itC != clusterLog_.end(); ++itC ) {
                    
                    std::vector< double> point, errors;
                    (itC->second).getPoint( point , errors );
                    
                    //construct hit box
                    // could add a fudge factor controlled by user
                    std::vector< double > maxBox( 3 , 0.0 ), minBox( 3 , 0.0 );
                    for ( unsigned int iC = 0; iC < 3; iC++ ) {
                        maxBox[iC] = point[iC] + errors[iC] + linesmudge[iC];
                        minBox[iC] = point[iC] - errors[iC] - linesmudge[iC];
                    }
                    
                    //see if ray hits box
                    if ( lineHitBox( origin , direction , minBox , maxBox ) ) {
                        ctrack_mipids.push_back( itC->first );
                    }

                } //iterate through all clusters to see if they are in track (itC)
                
                //check if plausible track
                if ( ctrack_mipids.size() > minNumClusters_ ) {
                    //create fit for ctrack
                    HcalMipTrack ctrack;
                    for ( std::vector< unsigned int >::iterator it = ctrack_mipids.begin();
                        it != ctrack_mipids.end(); ++it ) {
                        
                        MipCluster* cmip = &clusterLog_[ *it ];
                        for ( int i = 0; i < cmip->getNumHits(); i++ ) {
                            ctrack->addHit( cmip->getHit( iH ) );
                        }//iterate through hits in cluster

                    } //add clusters with mipids to ctrack
                    
                    if ( compMipTracks( best_track , ctrack ) ) {
                        best_track = ctrack;
                        track_mipids = ctrack_mipids;
                    }//ctrack is better than best_track

                }//ctrack is a plausible track

            } //go through remaining hits as second end point (itC2)
        } //go through all hits as first end point (itC1)
        
        return (!track_mipids.empty());
    }
}

DECLARE_PRODUCER_NS(ldmx, HcalMipTrackProducer);