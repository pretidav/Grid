/*************************************************************************************

Grid physics library, www.github.com/paboyle/Grid

Source file: ./lib/qcd/action/gauge/WilsonGaugeAction.h

Copyright (C) 2015

Author: Azusa Yamaguchi <ayamaguc@staffmail.ed.ac.uk>
Author: Peter Boyle <paboyle@ph.ed.ac.uk>
Author: neo <cossu@post.kek.jp>
Author: paboyle <paboyle@ph.ed.ac.uk>
Author: Guido Cossu <guido.cossu@ed.ac.uk>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

See the full license in the file "LICENSE" in the top level distribution
directory
*************************************************************************************/
/*  END LEGAL */
#ifndef QCD_WILSON_GAUGE_ACTION_H
#define QCD_WILSON_GAUGE_ACTION_H

namespace Grid {
namespace QCD {

////////////////////////////////////////////////////////////////////////
// Wilson Gauge Action .. should I template the Nc etc..
////////////////////////////////////////////////////////////////////////
template <class Gimpl>
class WilsonGaugeAction : public Action<typename Gimpl::GaugeField> {
 public:  
  INHERIT_GIMPL_TYPES(Gimpl);

  /////////////////////////// constructors
  explicit WilsonGaugeAction(RealD beta_):beta(beta_){};

  virtual std::string action_name() {return "WilsonGaugeAction";}

  virtual std::string LogParameters(){
    std::stringstream sstream;
    sstream << GridLogMessage << "[WilsonGaugeAction] Beta: " << beta << std::endl;
    return sstream.str();
  }

  virtual void refresh(const GaugeField &U,
                       GridParallelRNG &pRNG){};  // noop as no pseudoferms

  virtual RealD S(const GaugeField &U) {
    RealD plaq = WilsonLoops<Gimpl>::avgPlaquette(U);
    RealD vol = U._grid->gSites();
    RealD action = beta * (1.0 - plaq) * (Nd * (Nd - 1.0)) * vol * 0.5;
    return action;
  };

  virtual void deriv(const GaugeField &U, GaugeField &dSdU) {
    // not optimal implementation FIXME
    // extend Ta to include Lorentz indexes

    RealD factor = 0.5 * beta / RealD(Nc);

    //GaugeLinkField Umu(U._grid);
    GaugeLinkField dSdU_mu(U._grid);
    for (int mu = 0; mu < Nd; mu++) {
      //Umu = PeekIndex<LorentzIndex>(U, mu);

      // Staple in direction mu
      //WilsonLoops<Gimpl>::Staple(dSdU_mu, U, mu);
      //dSdU_mu = Ta(Umu * dSdU_mu) * factor;

  
      WilsonLoops<Gimpl>::StapleMult(dSdU_mu, U, mu);
      dSdU_mu = Ta(dSdU_mu) * factor;

      PokeIndex<LorentzIndex>(dSdU, dSdU_mu, mu);
    }
  }
private:
  RealD beta;  
};














// Anisotropic Wilson gauge action
template <class Gimpl>
class WilsonGaugeActionAnisotropic : public Action<typename Gimpl::GaugeField> {
 public:  
  INHERIT_GIMPL_TYPES(Gimpl);

// Fully anisotropic bare coupling (Nd betas for each lattice site).
 private:
  typedef Lattice< iVector<iScalar<iScalar<Real > >, Nd > > LatticeBeta;
  LatticeBeta Lbeta; 

  /////////////////////////// constructors
  explicit WilsonGaugeActionAnisotropic(LatticeBeta Lbeta_):Lbeta(Lbeta_){};

  virtual std::string action_name() {return "WilsonGaugeActionAnisotropic";}

  virtual std::string LogParameters(){
    std::stringstream sstream;

    //explicit loop over the volume: TODO
    for (int i=0;i<Nd;i++){
      sstream << GridLogMessage << "[WilsonGaugeActionAnisotropic] LatticeBeta[" << i <<"]:" << Lbeta << std::endl;
    }
    return sstream.str();
  }

  virtual void refresh(const GaugeField &U,
                       GridParallelRNG &pRNG){};  // noop as no pseudoferms

  virtual RealD S(const GaugeField &U) {

    // FIXME. I have to weight with beta before the average!!!!!
    RealD plaq = WilsonLoops<Gimpl>::avgPlaquette(U);

    //RealD beta_OneMinusPlaq = WilsonLoops<Gimpl>::OneMinusavgPlaquetteAnisotropy(U,Lbeta);    

    //explicit loop over volume: TODO
    RealD vol = U._grid->gSites();
    int X = U._grid->GlobalDimensions()[0];
    int Y = U._grid->GlobalDimensions()[1];
    int Z = U._grid->GlobalDimensions()[2];
    
    if (!isPeriodicGaugeField) vol -= X*Y*Z;  //subtraction of the Wall slice (needed for avoiding periodic). 

    RealD action = Lbeta * (1.0 - plaq) * (Nd * (Nd - 1.0)) * vol * 0.5;
  // RealD action = ( beta_OneMinusPlaq ) * (Nd * (Nd - 1.0)) * vol * 0.5;


    return action;
  };

  virtual void deriv(const GaugeField &U, GaugeField &dSdU) {
    // not optimal implementation FIXME
    // extend Ta to include Lorentz indexes

    //explicit loop over volume: TODO 
    RealD factor = 0.5 * beta / RealD(Nc);

    //GaugeLinkField Umu(U._grid);
    GaugeLinkField dSdU_mu(U._grid);
    for (int mu = 0; mu < Nd; mu++) {
      //Umu = PeekIndex<LorentzIndex>(U, mu);

      // Staple in direction mu
      //WilsonLoops<Gimpl>::Staple(dSdU_mu, U, mu);
      //dSdU_mu = Ta(Umu * dSdU_mu) * factor;

  
      WilsonLoops<Gimpl>::StapleMult(dSdU_mu, U, mu);
      dSdU_mu = Ta(dSdU_mu) * factor;

      PokeIndex<LorentzIndex>(dSdU, dSdU_mu, mu);
    }
  }
private:
  RealD beta;  
};


}
}

#endif
