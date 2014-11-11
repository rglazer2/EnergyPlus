#ifndef Pipes_hh_INCLUDED
#define Pipes_hh_INCLUDED

// ObjexxFCL Headers
#include <ObjexxFCL/FArray1D.hh>

// EnergyPlus Headers
#include <EnergyPlus.hh>
#include <DataGlobals.hh>
#include <DataPlant.hh>

namespace EnergyPlus {

class PlantComponentPipe : public DataPlant::PlantComponent {

public:

	// Create a new pipe object, read from input for a given index
	PlantComponentPipe(int const PipeType,
		std::string & PipeName,
		int LoopNum, // Index of plant loop where this pipe resides
		int LoopSide, // Index of plant loop side where this pipe resides
		int BranchIndex, // Index of plant Branch index where this pipe resides
		int CompIndex // Index of plant Comp index where this pipe resides
	);
	
	// Don't really need to do much here
	virtual ~PlantComponentPipe();

	void simulateComponent();

private:

	std::string Name; // main plant (cooling) loop ID
	int TypeOf; // type of pipe
	int InletNodeNum; // Node number on the inlet side of the plant
	int OutletNodeNum; // Node number on the inlet side of the plant
	int LoopNum; // Index of plant loop where this pipe resides
	int LoopSide; // Index of plant loop side where this pipe resides
	int BranchIndex; // Index of plant Branch index where this pipe resides
	int CompIndex; // Index of plant Comp index where this pipe resides
	bool EnvrnFlag;

};

	//     NOTICE

	//     Copyright © 1996-2014 The Board of Trustees of the University of Illinois
	//     and The Regents of the University of California through Ernest Orlando Lawrence
	//     Berkeley National Laboratory.  All rights reserved.

	//     Portions of the EnergyPlus software package have been developed and copyrighted
	//     by other individuals, companies and institutions.  These portions have been
	//     incorporated into the EnergyPlus software package under license.   For a complete
	//     list of contributors, see "Notice" located in main.cc.

	//     NOTICE: The U.S. Government is granted for itself and others acting on its
	//     behalf a paid-up, nonexclusive, irrevocable, worldwide license in this data to
	//     reproduce, prepare derivative works, and perform publicly and display publicly.
	//     Beginning five (5) years after permission to assert copyright is granted,
	//     subject to two possible five year renewals, the U.S. Government is granted for
	//     itself and others acting on its behalf a paid-up, non-exclusive, irrevocable
	//     worldwide license in this data to reproduce, prepare derivative works,
	//     distribute copies to the public, perform publicly and display publicly, and to
	//     permit others to do so.

	//     TRADEMARKS: EnergyPlus is a trademark of the US Department of Energy.

} // EnergyPlus

#endif
