/*************************************************************************************

Grid physics library, www.github.com/paboyle/Grid 

Source file: extras/Hadrons/Modules/MGauge/FundtoHirep.hpp

Copyright (C) 2015
Copyright (C) 2016

Author: David Preti <david.preti@to.infn.it>
	Guido Cossu <guido.cossu@ed.ac.uk>

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

#ifndef Hadrons_MGauge_FundtoHirep_hpp_
#define Hadrons_MGauge_FundtoHirep_hpp_

#include <Grid/Hadrons/Global.hpp>
#include <Grid/Hadrons/Module.hpp>
#include <Grid/Hadrons/ModuleFactory.hpp>

BEGIN_HADRONS_NAMESPACE

/******************************************************************************
 *                     Update Fund configuration to Hirep                     *
 ******************************************************************************/
BEGIN_MODULE_NAMESPACE(MGauge)

class FundtoHirepPar: Serializable
{
public:
    GRID_SERIALIZABLE_CLASS_MEMBERS(FundtoHirepPar,
                                    std::string, gaugein);
};

template <typename Rep>
class TFundtoHirep: public Module<FundtoHirepPar>
{
public:
    // constructor
    TFundtoHirep(const std::string name);
    // destructor
    virtual ~TFundtoHirep(void) = default;
    // dependency relation
    virtual std::vector<std::string> getInput(void);
    virtual std::vector<std::string> getOutput(void);
    // setup
    virtual void setup(void);
    // execution
    virtual void execute(void);
};

MODULE_REGISTER_NS(FundtoAdjoint,      TFundtoHirep<AdjointRepresentation>, MGauge); 
MODULE_REGISTER_NS(FundtoTwoIndexSym,  TFundtoHirep<TwoIndexSymmetricRepresentation>, MGauge);
MODULE_REGISTER_NS(FundtoTwoIndexAsym, TFundtoHirep<TwoIndexAntiSymmetricRepresentation>, MGauge);

END_MODULE_NAMESPACE

END_HADRONS_NAMESPACE

#endif // Hadrons_MGauge_FundtoHirep_hpp_
