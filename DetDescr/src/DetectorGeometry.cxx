/**
 * @file DetectorGeometry.cxx
 * @brief Implementation file for class DetectorGeometry
 */

#include "DetDescr/DetectorGeometry.h"

namespace ldmx {
    
    DetectorGeometry::DetectorGeometry() {
        
        ///////////////////////////////////////////////////////////////////////////////////
        // HCAL

        hcalParityVertical_ = 1;
        hcalThicknessScint_ = 15.0; 

        hcalWidthScint_ = 100.0;

        hcalThicknessLayer_ = 25. + hcalThicknessScint_ + 2*2.; //absorber + scint +2*air

        hcalNLayers_[ HcalSection::BACK   ] = 100;
        hcalNLayers_[ HcalSection::TOP    ] = 28;
        hcalNLayers_[ HcalSection::BOTTOM ] = 28;
        hcalNLayers_[ HcalSection::LEFT   ] = 28;
        hcalNLayers_[ HcalSection::RIGHT  ] = 28;
        
        hcalNStrips_[ HcalSection::BACK   ] = 30;
        hcalNStrips_[ HcalSection::TOP    ] = 3;
        hcalNStrips_[ HcalSection::BOTTOM ] = 3;
        hcalNStrips_[ HcalSection::LEFT   ] = 3;
        hcalNStrips_[ HcalSection::RIGHT  ] = 3;
         
        double ecal_z  = 290.;
        double ecal_xy = 525.;
        double ecal_front = 200.;

        hcalLengthScint_[ HcalSection::BACK   ] = 3000.;
        hcalLengthScint_[ HcalSection::TOP    ] = (3000.+ecal_xy)/2.;
        hcalLengthScint_[ HcalSection::BOTTOM ] = (3000.+ecal_xy)/2.;
        hcalLengthScint_[ HcalSection::LEFT   ] = (3000.+ecal_xy)/2.;
        hcalLengthScint_[ HcalSection::RIGHT  ] = (3000.+ecal_xy)/2.;
         
        hcalZeroLayer_[ HcalSection::BACK   ] = ecal_front + hcalNStrips_[ HcalSection::TOP ] * hcalWidthScint_;
        hcalZeroLayer_[ HcalSection::TOP    ] = ecal_xy/2.;
        hcalZeroLayer_[ HcalSection::BOTTOM ] = ecal_xy/2.;
        hcalZeroLayer_[ HcalSection::LEFT   ] = ecal_xy/2.;
        hcalZeroLayer_[ HcalSection::RIGHT  ] = ecal_xy/2.;
         
        hcalZeroStrip_[ HcalSection::BACK   ] = 3000./2.; 
        hcalZeroStrip_[ HcalSection::TOP    ] = 200.;
        hcalZeroStrip_[ HcalSection::BOTTOM ] = 200.;
        hcalZeroStrip_[ HcalSection::LEFT   ] = 200.;
        hcalZeroStrip_[ HcalSection::RIGHT  ] = 200.;

        ///////////////////////////////////////////////////////////////////////////////////
        // ECAL

        ecalHexRadius_ = 85.;

        ecalHexGap_ = 0.0;

        ecalZeroLayer_ = 200.0;

        ecalNCellsWide_ = 23;

        ecalSiThickness_ = 1.5;

        ecalDepth_ = 290.0;

        //TODO Recalculate these planes automatically
        ecalSiPlanes_ = {2.8, 5.7, 12.05, 16.45, 24.3, 30.2, 39.3, 45.7, 54.8, 61.2, 70.3, 76.7, 85.8, 92.2, 101.3, 107.7, 116.8, 123.2, 132.3, 138.7, 147.8, 154.2, 163.3, 169.7, 182.3, 192.2, 204.8, 214.7, 227.3, 237.2, 249.8, 259.7, 272.3, 282.2}; // With respect to the front face of the ECAL

        ecalHexReader_ = std::make_unique<EcalHexReadout>( ecalHexRadius_ , ecalHexGap_, ecalNCellsWide_ );

        ecalXYTower_.emplace_back( 0.0 , 0.0 );
        for ( int towerIndex = 0; towerIndex < 6; towerIndex++ ) {
            ecalXYTower_.emplace_back( 
                        sin( M_PI/3 * towerIndex)*( 2*ecalHexRadius_ + ecalHexGap_ ),
                        cos( M_PI/3 * towerIndex)*( 2*ecalHexRadius_ + ecalHexGap_ )
                    );
        }

        ///////////////////////////////////////////////////////////////////////////////////
        // RECOIL TRACKER




    }

    BoundingBox DetectorGeometry::getBoundingBox( HcalHit* hit ) const {
        
        //pairs that will go into BoundingBox
        std::pair<double,double> X(0,0), Y(0,0), Z(0,0);

        HcalSection section = (HcalSection)( hit->getSection() );
        int layer = hit->getLayer();
        int strip = hit->getStrip();

        //calculate center of layer,strip with respect to detector section
        double layercenter = layer*hcalThicknessLayer_ + 0.5*hcalThicknessScint_;
        double stripcenter = (strip + 0.5)*hcalWidthScint_;

        //calculate error in layer,strip position
        double elayer = 0.5*hcalThicknessScint_;
        double estrip = 0.5*hcalWidthScint_;
        
        double x,y,z;
        if ( section == HcalSection::BACK ) {
            
            z = hcalZeroLayer_.at( section ) + layercenter;
            Z.first  = z-elayer;
            Z.second = z+elayer;
            
            //only horizontal layers implemented currently
            if ( false ) { //( (layer ^ hcalParityVertical_) & 1) == 0 ) { //checks for same parity
                //Vertical Layers
                
                x = hcalZeroStrip_.at( section ) + stripcenter;
                X.first  = x - estrip;
                X.second = x + estrip;
                
                y = hit->getY();
                Y.first  = y - hcalUncertaintyTimingPos_;
                Y.second = y + hcalUncertaintyTimingPos_;

            } else {
                //Horizontal Layers
                
                x = hit->getX();
                X.first  = x - hcalUncertaintyTimingPos_;
                X.second = x + hcalUncertaintyTimingPos_;

                y = hcalZeroStrip_.at( section ) + stripcenter;
                Y.first  = y - estrip;
                Y.second = y + estrip;

            } //calculate depending on layer

        } else {
            
            z = hcalZeroStrip_.at( section ) + stripcenter;
            Z.first  = z - estrip;
            Z.second = z + estrip;

            if ( section == HcalSection::TOP or section == HcalSection::BOTTOM ) {
                
                x = hit->getX();
                X.first  = x - hcalUncertaintyTimingPos_;
                X.second = x + hcalUncertaintyTimingPos_;
                
                if ( section == HcalSection::TOP ) {
                    y = hcalZeroLayer_.at( section ) + layercenter;
                } else {
                    y = hcalZeroLayer_.at( section ) - layercenter;
                } //top or bottom hcal

                Y.first  = y - elayer;
                Y.second = y + elayer;
                
            } else if ( section == HcalSection::LEFT or section == HcalSection::RIGHT ) {
                
                y = hit->getY();
                Y.first  = y - hcalUncertaintyTimingPos_;
                Y.second = y + hcalUncertaintyTimingPos_;

                if ( section == HcalSection::LEFT ) {
                    x = hcalZeroLayer_.at( section ) + layercenter;
                } else {
                    x = hcalZeroLayer_.at( section ) - layercenter;
                } //left or right hcal

                X.first  = x - elayer;
                X.second = x + elayer;
    
            } else {
                std::cerr << "[ DetectorGeometry::getBoundingBox ] : Unknown Hcal Section!" << std::endl;
                std::cerr << "    Returning a valid BoundingBox but with values that are all zero." << std::endl;
            } //side hcal
        
        } //calculate depending on section

        BoundingBox hbox;
        hbox.push_back( X );
        hbox.push_back( Y );
        hbox.push_back( Z );
        return hbox;
    }
    
    BoundingBox DetectorGeometry::getBoundingBox( const std::vector<HcalHit*>  &hitVec ) const {
        
        std::vector<double> pointSum ( 3 , 0.0 ); //sums of weighted coordinates
        std::vector<double> weightSum( 3 , 0.0 ); //sums of weights for each coordinate
        
        //calculate real space point for each hit
        for ( HcalHit* hit : hitVec ) {
            
            BoundingBox box = getBoundingBox( hit );
            
            //Add weighted values to sums
            double weight;
            for ( unsigned int iC = 0; iC < 3; iC++ ) {
                
                double cer = abs(box[iC].second - box[iC].first)/2.0;

                weight = 1.0 / ( cer*cer );
                weightSum[ iC ] += weight;
                pointSum[ iC ] += weight*( ( box[iC].second + box[iC].first )/2.0 );
            }
        } //go through hitVec
        
        //Construct final BoundingBox
        BoundingBox hbox;
        for ( int iC = 0; iC < 3; iC++ ) {
            double c = pointSum[ iC ] / weightSum[ iC ];
            double ec = 1.0 / sqrt( weightSum[ iC ] );
            hbox.emplace_back( c - ec , c + ec );
        }

        return hbox;
    }

    BoundingBox DetectorGeometry::getBoundingBox( HcalSection section ) const {

        std::pair< double, double > X(0,0), Y(0,0), Z(0,0);

        double total_strip_width = hcalNStrips_.at( section ) * hcalWidthScint_;
        double total_thickness = hcalNLayers_.at( section ) * hcalThicknessLayer_;
        if ( section == HcalSection::BACK ) {
           
            X.first  = -hcalZeroStrip_.at( HcalSection::BACK );
            X.second = X.first + total_strip_width;

            Y.first  = -hcalLengthScint_.at( HcalSection::BACK )/2.0;
            Y.second =  hcalLengthScint_.at( HcalSection::BACK )/2.0;

            Z.first  = hcalZeroLayer_.at( HcalSection::BACK );
            Z.second = Z.first + total_thickness;

        } else {

            Z.first  = hcalZeroStrip_.at( section );
            Z.second = Z.first + total_strip_width;

            if ( section == HcalSection::LEFT ) {
                
                X.first  = hcalZeroLayer_.at( HcalSection::LEFT );
                X.second = X.first + total_thickness;

                Y.second = hcalZeroLayer_.at( HcalSection::TOP );
                Y.first  = Y.second - hcalLengthScint_.at( HcalSection::LEFT );

            } else if ( section == HcalSection::RIGHT ) {

                X.second = -hcalZeroLayer_.at( HcalSection::RIGHT );
                X.first  = X.second - total_thickness;

                Y.first  = -hcalZeroLayer_.at( HcalSection::BOTTOM );
                Y.second = Y.first + hcalLengthScint_.at( HcalSection::RIGHT );

            } else if ( section == HcalSection::TOP ) {

                Y.first  = hcalZeroLayer_.at( HcalSection::TOP );
                Y.second = Y.first + total_thickness;

                X.first  = -hcalZeroLayer_.at( HcalSection::RIGHT );
                X.second = X.first + hcalLengthScint_.at( HcalSection::TOP );

            } else if ( section == HcalSection::BOTTOM ) {

                Y.second = -hcalZeroLayer_.at( HcalSection::BOTTOM );
                Y.first  = Y.second - total_thickness;

                X.second = hcalZeroLayer_.at( HcalSection::LEFT );
                X.first  = X.second - hcalLengthScint_.at( HcalSection::BOTTOM );

            } else {
                std::cerr << "[ Warning ] : Unrecognized HcalSection in DetectorGeometry::getBoundingBox." << std::endl;
                std::cerr << "    Will return an incorrect geometry description!" << std::endl;
            }
        }

        BoundingBox boundingbox;
        boundingbox.push_back( X );
        boundingbox.push_back( Y );
        boundingbox.push_back( Z );

        return boundingbox;
    }

    HexPrism DetectorGeometry::getHexPrism( unsigned int cellID , unsigned int moduleID , int layer ) const {

        unsigned int combinedID = ecalHexReader_->combineID( cellID , moduleID );

        XYCoords xy = ecalHexReader_->getCellCenterAbsolute( combinedID );

        HexPrism hexpris;
        hexpris.x = xy.first;
        hexpris.y = xy.second;
        hexpris.z = ecalSiPlanes_.at( layer );
        hexpris.height = ecalSiThickness_;
        hexpris.radius = ecalHexRadius_ / ecalNCellsWide_;

        return hexpris;
    }

    HexPrism DetectorGeometry::getHexPrism( EcalHit* hit ) const {

        unsigned int hitID = hit->getID();
        unsigned int cellID = hitID >> 15;
        unsigned int moduleID = (hitID << 17) >> 29;
        int layer = hit->getLayer();

        return this->getHexPrism( cellID , moduleID , layer );
    }

    HexPrism DetectorGeometry::getHexPrism( int towerIndex ) const {

        HexPrism hexpris;

        if ( towerIndex < 0 or towerIndex > 6 ) {
            std::cerr << "[ Warning ] : towerIndex " << towerIndex << " out of bounds!" << std::endl;
            std::cerr << "    Will return a malformed HexPrism." << std::endl;
            return hexpris;
        }

        hexpris.x = ecalXYTower_.at( towerIndex ).first;
        hexpris.y = ecalXYTower_.at( towerIndex ).second;
        hexpris.z = ecalZeroLayer_ + ecalDepth_/2;
        hexpris.height = ecalDepth_;
        hexpris.radius = ecalHexRadius_ * 2 / sqrt(3); //need radius to corner, not to side

        return hexpris;
    }
}
