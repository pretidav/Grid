/*************************************************************************************

Grid physics library, www.github.com/paboyle/Grid 

Source file: extras/Hadrons/Modules/MContraction/Baryon.hpp

Copyright (C) 2015
Copyright (C) 2016

Author: David Preti <david.preti@to.infn.it>

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

See the full license in the file "LICENSE" in the top level distribution directory
*************************************************************************************/
/*  END LEGAL */

#ifndef Hadrons_MContraction_Baryon_hpp_
#define Hadrons_MContraction_Baryon_hpp_

#include <Grid/Hadrons/Global.hpp>
#include <Grid/Hadrons/Module.hpp>
#include <Grid/Hadrons/ModuleFactory.hpp>

BEGIN_HADRONS_NAMESPACE

/******************************************************************************
 *                               Baryon                                       *
 ******************************************************************************/
BEGIN_MODULE_NAMESPACE(MContraction)

inline RealD Sign(int a, int b){ 
    return (a==b) ? 0:(b-a)/std::abs(b-a);
}

class BaryonPar: Serializable
{
public:
    GRID_SERIALIZABLE_CLASS_MEMBERS(BaryonPar,
                                    std::string, q1,
                                    std::string, q2,
                                    std::string, q3,
                                    std::string, output);
};

template <typename FImpl1, typename FImpl2, typename FImpl3>
class TBaryon: public Module<BaryonPar>
{
public:
    FERM_TYPE_ALIASES(FImpl1, 1);
    FERM_TYPE_ALIASES(FImpl2, 2);
    FERM_TYPE_ALIASES(FImpl3, 3);
    class Result: Serializable
    {
    public:
        GRID_SERIALIZABLE_CLASS_MEMBERS(Result,
                                       std::vector<Complex>, corr);
                                       //std::vector<std::vector<std::vector<Complex>>>, corr);
    };
public:
    // constructor
    TBaryon(const std::string name);
    // destructor
    virtual ~TBaryon(void) = default;
    // dependency relation
    virtual std::vector<std::string> getInput(void);
    virtual std::vector<std::string> getOutput(void);
protected:
    // setup
    virtual void setup(void);
    // execution
    virtual void execute(void);
};

MODULE_REGISTER_NS(Baryon, ARG(TBaryon<FIMPL, FIMPL, FIMPL>), MContraction);

/******************************************************************************
 *                         TBaryon implementation                             *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
template <typename FImpl1, typename FImpl2, typename FImpl3>
TBaryon<FImpl1, FImpl2, FImpl3>::TBaryon(const std::string name)
: Module<BaryonPar>(name)
{}

// dependencies/products ///////////////////////////////////////////////////////
template <typename FImpl1, typename FImpl2, typename FImpl3>
std::vector<std::string> TBaryon<FImpl1, FImpl2, FImpl3>::getInput(void)
{
    std::vector<std::string> input = {par().q1, par().q2, par().q3};
    
    return input;
}

template <typename FImpl1, typename FImpl2, typename FImpl3>
std::vector<std::string> TBaryon<FImpl1, FImpl2, FImpl3>::getOutput(void)
{
    std::vector<std::string> output = {};
    
    return output;
}

// setup ///////////////////////////////////////////////////////////////////////
template <typename FImpl1, typename FImpl2, typename FImpl3>
void TBaryon<FImpl1, FImpl2, FImpl3>::setup(void)
{
    envTmpLat(LatticeComplex, "c");
    envTmpLat(LatticeComplex, "NCtrtr");
    envTmpLat(LatticeComplex, "NCtr");
    envTmpLat(LatticeComplex, "LCtrtr");
    envTmpLat(LatticeComplex, "LCtr");
}


// execution ///////////////////////////////////////////////////////////////////
template <typename FImpl1, typename FImpl2, typename FImpl3>
void TBaryon<FImpl1, FImpl2, FImpl3>::execute(void)
{
    LOG(Message) << "Computing baryon contractions '" << getName() << "' using"
                 << " quarks '" << par().q1 << "', '" << par().q2 << "', and '"
                 << par().q3 << "'" << std::endl;
    
    //std::string output_name = par().output + "." + std::to_string(env().getTrajectory());
    
    //CorrWriter             writer(output_name);
    ResultWriter           writer(RESULT_FILE_NAME(par().output));
    //PropagatorField1      &q1 = *env().template getObject<PropagatorField1>(par().q1);
    //PropagatorField2      &q2 = *env().template getObject<PropagatorField2>(par().q2);
    //PropagatorField3      &q3 = *env().template getObject<PropagatorField3>(par().q3);
    
    auto &q1 = envGet(PropagatorField1, par().q1);
    auto &q2 = envGet(PropagatorField2, par().q2);
    auto &q3 = envGet(PropagatorField2, par().q3);
    //LatticeComplex        c(env().getGrid()),  NCtrtr(env().getGrid()), NCtr(env().getGrid()), LCtrtr(env().getGrid()), LCtr(env().getGrid());
    envGetTmp(LatticeComplex, c);
    envGetTmp(LatticeComplex, NCtrtr);
    envGetTmp(LatticeComplex, NCtr);
    envGetTmp(LatticeComplex, LCtrtr);
    envGetTmp(LatticeComplex, LCtr);

    int                   nt = env().getDim(Tp);
    std::vector<TComplex> bufN,bufL;
    std::vector<Result>   Nresult, Lresult; 
    
    

   //-------- FIXME with optimized gammas ---------
   // Prepare Spin Matrices
   SpinMatrix gT, Id, P[2]; 

   Id= zero;
   RealD unit = 1.0;
   for (int i=0;i<Ns;i++){
     Id()(i,i)()=1;  
   }  
   
   gT= zero;
   gT()(0,2)()=  1;
   gT()(1,3)()=  1;
   gT()(2,0)()=  1;
   gT()(3,1)()=  1;
   

  //Prepare useful factors 
    RealD four = 4.;
    RealD two = four/2.;
    RealD norm = 1./6.;

  //Prepare antisymmetric tensor epsilon 
  int ep[3][Nc];
  int em[3][Nc];
  int contp=0,contm=0;
  int S1,S2,S3; //,S4,S5,S6;  more if SU(N>3)

  for (int i=0;i<Nc;i++){
    for (int j=0;j<Nc;j++){
      for (int k=0;k<Nc;k++){
        S1=Sign(i,j);
        S2=Sign(j,k);
        S3=Sign(i,k);
        if ((S1*S2*S3)>0){
          ep[contp][0]=i;
          ep[contp][1]=j;
          ep[contp][2]=k;
          contp++;
        } else if ((S1*S2*S3)<0){
          em[contm][0]=i;
          em[contm][1]=j;
          em[contm][2]=k;
          contm++;
        }
      }
    }
  }

//I am considering:  Cconjugation=GammaTGammaY 

Gamma::Algebra Ga [] ={
    Gamma::Algebra::Identity,
    Gamma::Algebra::Gamma5,
    Gamma::Algebra::Identity,
    Gamma::Algebra::Identity,
    Gamma::Algebra::Identity,
    Gamma::Algebra::Identity
};

Gamma::Algebra Gb [] ={
    Gamma::Algebra::MinusSigmaXZ,
    Gamma::Algebra::MinusSigmaYT,
    Gamma::Algebra::GammaYGamma5, // missing  imag i  (corrected in the contractions)
    Gamma::Algebra::MinusGammaZGamma5,
    Gamma::Algebra::GammaT,
    Gamma::Algebra::GammaXGamma5
};


// Parity Projection 
P[0] = 0.5 * ( Id + gT );  //Positive Parity Projector
P[1] = 0.5 * ( Id - gT );  //Negative Parity Projector

// Contractions
LatticeSpinMatrix q1_c(env().getGrid()); q1_c = zero;
LatticeSpinMatrix q2_c(env().getGrid()); q2_c = zero;
LatticeSpinMatrix q3_c(env().getGrid()); q3_c = zero;

Nresult.resize(12);
Lresult.resize(12);

int count=-1;

  for (unsigned int iParity=0;iParity<2;iParity++){
    for (unsigned int iSrc=0;iSrc<6;iSrc++){
        
      count++;
      NCtrtr=zero;
      NCtr=zero; 
      LCtrtr=zero;
      LCtr=zero;
 //loop over non-zero permutations     
      for (unsigned int i=0;i<3;i++){
        for (unsigned int j=0;j<3;j++){
    //positive colours permutations                                                                                                                                           
          q3_c = peekColour(q3,ep[i][1],ep[j][1]);
          q1_c = peekColour(q1,ep[i][0],ep[j][0]);
          q2_c = peekColour(q2,ep[i][2],ep[j][2]);
          
          NCtrtr += trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c ) * trace( transpose(Gamma(Gb[iSrc])*q3_c*transpose(Gamma(Gb[iSrc]))) * q2_c );

          NCtr += trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c * transpose(Gamma(Gb[iSrc])*q3_c*transpose(Gamma(Gb[iSrc]))) * q2_c );

          LCtrtr += four * trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q3_c ) * trace( transpose(Gamma(Gb[iSrc])*q2_c*transpose(Gamma(Gb[iSrc]))) * q1_c );
          LCtrtr += trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q2_c ) * trace( transpose(Gamma(Gb[iSrc])*q3_c*transpose(Gamma(Gb[iSrc]))) * q1_c );
          LCtrtr += trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c ) * trace( transpose(Gamma(Gb[iSrc])*q3_c*transpose(Gamma(Gb[iSrc]))) * q2_c );
          LCtr -= two*trace( q3_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c * Gamma(Gb[iSrc])*transpose(q2_c)*Gamma(Gb[iSrc]) );
          LCtr += two*trace( q2_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q3_c * transpose(Gamma(Gb[iSrc]))*transpose(q1_c)*Gamma(Gb[iSrc]) );
          LCtr -= two*trace( q1_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q3_c * transpose(Gamma(Gb[iSrc]))*transpose(q2_c)*transpose(Gamma(Gb[iSrc])) );
          LCtr += two*trace( q3_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q2_c * transpose(Gamma(Gb[iSrc]))*transpose(q1_c)*Gamma(Gb[iSrc]) );
          LCtr -= trace( q1_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q2_c * Gamma(Gb[iSrc])*transpose(q3_c)*transpose(Gamma(Gb[iSrc])) );
          LCtr -= trace( q2_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c * Gamma(Gb[iSrc])*transpose(q3_c)*transpose(Gamma(Gb[iSrc])) );
          q3_c = zero; q1_c = zero; q2_c = zero;
          
          q3_c = peekColour(q3,em[i][1],em[j][1]);
          q1_c = peekColour(q1,em[i][0],em[j][0]);
          q2_c = peekColour(q2,em[i][2],em[j][2]);

          NCtrtr += trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c ) * trace( transpose(Gamma(Gb[iSrc])*q3_c*transpose(Gamma(Gb[iSrc]))) * q2_c );
          NCtr += trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c * transpose(Gamma(Gb[iSrc])*q3_c*transpose(Gamma(Gb[iSrc]))) * q2_c );
          
          LCtrtr += four * trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q3_c ) * trace( transpose(Gamma(Gb[iSrc])*q2_c*transpose(Gamma(Gb[iSrc]))) * q1_c );
          LCtrtr += trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q2_c ) * trace( transpose(Gamma(Gb[iSrc])*q3_c*transpose(Gamma(Gb[iSrc]))) * q1_c );
          LCtrtr += trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c ) * trace( transpose(Gamma(Gb[iSrc])*q3_c*transpose(Gamma(Gb[iSrc]))) * q2_c );
          LCtr -= two*trace( q3_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c * Gamma(Gb[iSrc])*transpose(q2_c)*Gamma(Gb[iSrc]) );
          LCtr += two*trace( q2_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q3_c * transpose(Gamma(Gb[iSrc]))*transpose(q1_c)*Gamma(Gb[iSrc]) );
          LCtr -= two*trace( q1_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q3_c * transpose(Gamma(Gb[iSrc]))*transpose(q2_c)*transpose(Gamma(Gb[iSrc])) );
          LCtr += two*trace( q3_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q2_c * transpose(Gamma(Gb[iSrc]))*transpose(q1_c)*Gamma(Gb[iSrc]) );
          LCtr -= trace( q1_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q2_c * Gamma(Gb[iSrc])*transpose(q3_c)*transpose(Gamma(Gb[iSrc])) );
          LCtr -= trace( q2_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c * Gamma(Gb[iSrc])*transpose(q3_c)*transpose(Gamma(Gb[iSrc])) );
          q3_c = zero; q1_c = zero; q2_c = zero;

    //negative colours permutations                                                                                                                                           
          q3_c = peekColour(q3,ep[i][1],em[j][1]);
          q1_c = peekColour(q1,ep[i][0],em[j][0]);
          q2_c = peekColour(q2,ep[i][2],em[j][2]);
          
          NCtrtr -= trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c ) * trace( transpose(Gamma(Gb[iSrc])*q3_c*transpose(Gamma(Gb[iSrc]))) * q2_c );
          NCtr -= trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c * transpose(Gamma(Gb[iSrc])*q3_c*transpose(Gamma(Gb[iSrc]))) * q2_c );
          
          LCtrtr -= four * trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q3_c ) * trace( transpose(Gamma(Gb[iSrc])*q2_c*transpose(Gamma(Gb[iSrc]))) * q1_c );
          LCtrtr -= trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q2_c ) * trace( transpose(Gamma(Gb[iSrc])*q3_c*transpose(Gamma(Gb[iSrc]))) * q1_c );
          LCtrtr -= trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c ) * trace( transpose(Gamma(Gb[iSrc])*q3_c*transpose(Gamma(Gb[iSrc]))) * q2_c );
          LCtr += two*trace( q3_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c * Gamma(Gb[iSrc])*transpose(q2_c)*Gamma(Gb[iSrc]) );
          LCtr -= two*trace( q2_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q3_c * transpose(Gamma(Gb[iSrc]))*transpose(q1_c)*Gamma(Gb[iSrc]) );
          LCtr += two*trace( q1_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q3_c * transpose(Gamma(Gb[iSrc]))*transpose(q2_c)*transpose(Gamma(Gb[iSrc])) );
          LCtr -= two*trace( q3_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q2_c * transpose(Gamma(Gb[iSrc]))*transpose(q1_c)*Gamma(Gb[iSrc]) );
          LCtr += trace( q1_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q2_c * Gamma(Gb[iSrc])*transpose(q3_c)*transpose(Gamma(Gb[iSrc])) );
          LCtr += trace( q2_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c * Gamma(Gb[iSrc])*transpose(q3_c)*transpose(Gamma(Gb[iSrc])) );
          q3_c = zero; q1_c = zero; q2_c = zero;

          q3_c = peekColour(q3,em[i][1],ep[j][1]);
          q1_c = peekColour(q1,em[i][0],ep[j][0]);
          q2_c = peekColour(q2,em[i][2],ep[j][2]);

          NCtrtr -= trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c ) * trace( transpose(Gamma(Gb[iSrc])*q3_c*transpose(Gamma(Gb[iSrc]))) * q2_c );
          NCtr -= trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c * transpose(Gamma(Gb[iSrc])*q3_c*transpose(Gamma(Gb[iSrc]))) * q2_c );
        
          LCtrtr -= four * trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q3_c ) * trace( transpose(Gamma(Gb[iSrc])*q2_c*transpose(Gamma(Gb[iSrc]))) * q1_c );
          LCtrtr -= trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q2_c ) * trace( transpose(Gamma(Gb[iSrc])*q3_c*transpose(Gamma(Gb[iSrc]))) * q1_c );
          LCtrtr -= trace( (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c ) * trace( transpose(Gamma(Gb[iSrc])*q3_c*transpose(Gamma(Gb[iSrc]))) * q2_c );       
          LCtr += two*trace( q3_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c * Gamma(Gb[iSrc])*transpose(q2_c)*Gamma(Gb[iSrc]) );
          LCtr -= two*trace( q2_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q3_c * transpose(Gamma(Gb[iSrc]))*transpose(q1_c)*Gamma(Gb[iSrc]) );
          LCtr += two*trace( q1_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q3_c * transpose(Gamma(Gb[iSrc]))*transpose(q2_c)*transpose(Gamma(Gb[iSrc])) );
          LCtr -= two*trace( q3_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q2_c * transpose(Gamma(Gb[iSrc]))*transpose(q1_c)*Gamma(Gb[iSrc]) );        
          LCtr += trace( q1_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q2_c * Gamma(Gb[iSrc])*transpose(q3_c)*transpose(Gamma(Gb[iSrc])) );
          LCtr += trace( q2_c * (Gamma(Ga[iSrc])*P[iParity]*Gamma(Ga[iSrc])) * q1_c * Gamma(Gb[iSrc])*transpose(q3_c)*transpose(Gamma(Gb[iSrc])) );
          q3_c = zero; q1_c = zero; q2_c = zero;
        }
      }
      c = (NCtrtr + NCtr);
      sliceSum(c, bufN, Tp);
      Nresult[count].corr.resize(nt);

      c = norm * (LCtrtr + LCtr); 
      sliceSum(c, bufL, Tp);
      Lresult[count].corr.resize(nt);

      for (unsigned int t=0; t<bufN.size(); t++){
        if (iSrc==2){   //correcting for the missing imag i in the third Gb spin matrix
        Nresult[count].corr[t] =  -TensorRemove(bufN[t]);
        Lresult[count].corr[t] =  -TensorRemove(bufL[t]);
        } else {
        Nresult[count].corr[t] =  TensorRemove(bufN[t]);
        Lresult[count].corr[t] =  TensorRemove(bufL[t]);
        }
        
      }
    }
  }
   //----------------------------
  write(writer, "baryon", Nresult);
  write(writer, "baryon", Lresult);
}

END_MODULE_NAMESPACE

END_HADRONS_NAMESPACE

#endif // Hadrons_MContraction_Baryon_hpp_


































