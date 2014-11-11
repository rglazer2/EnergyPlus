// ObjexxFCL Headers
#include <ObjexxFCL/FArray.functions.hh>

// EnergyPlus Headers
#include <Pipes.hh>
#include <BranchNodeConnections.hh>
#include <DataHVACGlobals.hh>
#include <DataIPShortCuts.hh>
#include <DataLoopNode.hh>
#include <DataPlant.hh>
#include <DataPrecisionGlobals.hh>
#include <General.hh>
#include <InputProcessor.hh>
#include <NodeInputManager.hh>
#include <OutputProcessor.hh>
#include <PlantUtilities.hh>
#include <UtilityRoutines.hh>

namespace EnergyPlus {

PlantComponentPipe::~PlantComponentPipe(){}

// default constructor?
PlantComponentPipe::PlantComponentPipe(
	int const PipeType,
	std::string & PipeName,
	int LoopNum, // Index of plant loop where this pipe resides
	int LoopSide, // Index of plant loop side where this pipe resides
	int BranchIndex, // Index of plant Branch index where this pipe resides
	int CompIndex // Index of plant Comp index where this pipe resides
)
{
	// SUBROUTINE INFORMATION:
	//       AUTHOR         Dan Fisher
	//       DATE WRITTEN   Sept. 1998
	//       MODIFIED       na
	//       RE-ENGINEERED  na

	using InputProcessor::GetNumObjectsFound;
	using InputProcessor::GetObjectItem;
	using namespace DataIPShortCuts;
	using NodeInputManager::GetOnlySingleNode;
	using BranchNodeConnections::TestCompSet;
	using namespace DataPrecisionGlobals;
	using namespace DataHVACGlobals;
	using namespace DataLoopNode;
	using DataPlant::TypeOf_Pipe;
	using DataPlant::TypeOf_PipeSteam;

	// SUBROUTINE LOCAL VARIABLE DECLARATIONS:
	int NumAlphas; // Number of elements in the alpha array
	int NumNums; // Number of elements in the numeric array
	int IOStat; // IO Status when calling get input subroutine
	bool ErrorsFound( true );

	this->Name = PipeName;
	this->TypeOf = PipeType;
	this->EnvrnFlag = true;
	this->LoopNum = LoopNum; // Index of plant loop where this pipe resides
	this->LoopSide = LoopSide; // Index of plant loop side where this pipe resides
	this->BranchIndex = BranchIndex; // Index of plant Branch index where this pipe resides
	this->CompIndex = CompIndex; // Index of plant Comp index where this pipe resides
	
	if ( PipeType == TypeOf_Pipe ) {
		cCurrentModuleObject = "Pipe:Adiabatic";
	} else if ( PipeType == TypeOf_PipeSteam ) {
		cCurrentModuleObject = "Pipe:Adiabatic:Steam";
	}

	int NumPipes( GetNumObjectsFound( cCurrentModuleObject ) );

	for ( int PipeIndex = 1; PipeIndex <= NumPipes; ++PipeIndex ) {
		GetObjectItem( cCurrentModuleObject, PipeIndex, cAlphaArgs, NumAlphas, rNumericArgs, NumNums, IOStat );
		if ( cAlphaArgs( 1 ) == PipeName ) {
			ErrorsFound = false;
			this->InletNodeNum = GetOnlySingleNode( cAlphaArgs( 2 ), ErrorsFound, cCurrentModuleObject, cAlphaArgs( 1 ), NodeType_Water, NodeConnectionType_Inlet, 1, ObjectIsNotParent );
			this->OutletNodeNum = GetOnlySingleNode( cAlphaArgs( 3 ), ErrorsFound, cCurrentModuleObject, cAlphaArgs( 1 ), NodeType_Water, NodeConnectionType_Outlet, 1, ObjectIsNotParent );
			TestCompSet( cCurrentModuleObject, cAlphaArgs( 1 ), cAlphaArgs( 2 ), cAlphaArgs( 3 ), "Pipe Nodes" );
		}
	}

	if ( ErrorsFound ) {
		throw std::runtime_error( "GetPipeInput: Errors getting input for pipes" );
	}

}

void PlantComponentPipe::simulateComponent()
{
	// SUBROUTINE INFORMATION:
	//       AUTHOR         Dan Fisher
	//       DATE WRITTEN   Sept. 1998
	//       MODIFIED       na
	//       RE-ENGINEERED  na

	// Using/Aliasing
	using DataPlant::PlantLoop;
	using DataPlant::ScanPlantLoopsForObject;
	using PlantUtilities::SafeCopyPlantNode;
	using PlantUtilities::InitComponentNodes;
	using DataGlobals::BeginEnvrnFlag;

	if ( BeginEnvrnFlag && EnvrnFlag ) {
		InitComponentNodes( 0.0, PlantLoop( LoopNum ).MaxMassFlowRate, InletNodeNum, OutletNodeNum, LoopNum, LoopSide, BranchIndex, CompIndex );
		EnvrnFlag = false;
	}

	if ( ! BeginEnvrnFlag ) EnvrnFlag = true;

	SafeCopyPlantNode( InletNodeNum, OutletNodeNum, LoopNum );
}


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
