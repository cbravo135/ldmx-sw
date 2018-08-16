/**
 * @file HcalMipTrack.cxx
 * @brief Class implementation of a track through the Hcal
 * @author Tom Eichlersmith, University of Minnesota
 */

#include "Event/HcalMipTrack.h"

ClassImp( ldmx::HcalMipTrack );

namespace ldmx {
    
    HcalMipTrack::HcalMipTrack() 
        : TObject() , start_( 3 , 0.0 ), end_( 3 , 0.0 )
          { } 

    HcalMipTrack::~HcalMipTrack() {
        Clear();
        
        hcalHits_.Delete();
        
    }
    
    HcalMipTrack& HcalMipTrack::operator= ( const HcalMipTrack &track ) {
        
        if ( this != &track ) { //self-assignment guard
            
            this->hcalHits_ = track.hcalHits_;
            this->zxGr_ = track.zxGr_;
            this->zyGr_ = track.zyGr_;
        }

        return *this;
    }
             
    void HcalMipTrack::Clear( Option_t *opt ) {

        TObject::Clear(opt);
        
        hcalHits_.Clear(opt);

        zxGr_.Set( 0 );
        zyGr_.Set( 0 );

        for ( int i = 0; i < 3; i++ ) {
            start_[i] = 0.0;
            end_[i] = 0.0;
        }

        return;
    }
 
    void HcalMipTrack::addHit( HcalHit* hit ) {
        hcalHits_.Add( hit );
        return;
    }

    void HcalMipTrack::addPoint( const std::vector<double> &min , const std::vector<double> &origin ,
                                 const std::vector<double> &max ) {
        
        zxGr_.SetPoint( zxGr_.GetN() , origin[2] , origin[0] );
        zxGr_.SetPointError( zxGr_.GetN()-1 , min[2]-origin[2] , max[2]-origin[2] ,
                                              min[0]-origin[0] , max[0]-origin[0] );

        zyGr_.SetPoint( zyGr_.GetN() , origin[2] , origin[1] );
        zyGr_.SetPointError( zyGr_.GetN()-1 , min[2]-origin[2] , max[2]-origin[2] ,
                                              min[1]-origin[1] , max[1]-origin[1] );

        return;       
    }
       
    int HcalMipTrack::getNHits() const {
        return hcalHits_.GetEntriesFast();
    }

    HcalHit* HcalMipTrack::getHit( int i ) const {
        return ( dynamic_cast<HcalHit*>(hcalHits_.At(i)) );
    }
    
    void HcalMipTrack::setFit() {
        
        zxGr_.Sort();
        zyGr_.Sort();

        zxGr_.GetPoint( 0 , start_[2] , start_[0] );
        zyGr_.GetPoint( 0 , start_[2] , start_[1] );

        zxGr_.GetPoint( zxGr_.GetN()-1 , end_[2] , end_[0] );
        zyGr_.GetPoint( zyGr_.GetN()-1 , end_[2] , end_[1] );

        zxGr_.Fit( "pol1" , "Q" );
        zyGr_.Fit( "pol1" , "Q" );
        
        return;
    }

    void HcalMipTrack::evalFit( const double z , double &x , double &y ) const {
        
        TF1 *fitresult;
        fitresult = zxGr_.GetFunction( "pol1" );
        x = fitresult->Eval( z );
        fitresult = zyGr_.GetFunction( "pol1" );
        y = fitresult->Eval( z );
        
        return;
    }
    
    int HcalMipTrack::getNClusters() const {
        return ( zxGr_.GetN() );
    }

    void HcalMipTrack::merge( HcalMipTrack *track ) {
        
        //add all hits to this track
        for ( int i = 0; i < track->getNHits(); i++ ) {
            this->addHit( track->getHit( i ) );
        }

        TRefArray *zxGrColl = new TRefArray();
        zxGrColl->Add( (TObject *)(&track->zxGr_) );
        this->zxGr_.Merge( zxGrColl );

        TRefArray *zyGrColl = new TRefArray();
        zyGrColl->Add( (TObject *)(&track->zyGr_) );
        this->zyGr_.Merge( zyGrColl );

        zxGrColl->Delete();
        zyGrColl->Delete();

        this->setFit();
    }
    
    bool HcalMipTrack::isEmpty() const {
        return ( hcalHits_.IsEmpty() );
    }

    bool HcalMipTrack::isBroken() const {
        for ( int iH = 0; iH < getNHits(); iH++ ) {
            if ( getHit( iH ) == nullptr )
                return true;
        } //iterate through hits checking for nullptr (iH)

        return false;
    }
}