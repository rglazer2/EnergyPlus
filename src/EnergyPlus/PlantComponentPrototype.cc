// EnergyPlus Headers
#include <PlantComponentPrototype.hh>
#include <General.hh>
#include <UtilityRoutines.hh>
#include <InputProcessor.hh>
#include <NodeInputManager.hh>
#include <DataIPShortCuts.hh>
#include <OutputProcessor.hh>
#include <DataLoopNode.hh>
#include <BranchNodeConnections.hh>
#include <DataPlant.hh>
#include <PlantUtilities.hh>
#include <DataGlobals.hh>

#include <iostream>

namespace EnergyPlus {

	using DataPlant::TypeOf_PrototypePlantComponent;
	using DataPlant::ScanPlantLoopsForObject;
	using DataPlant::PlantLoop;
	using PlantUtilities::InitComponentNodes;
	using PlantUtilities::RegisterPlantCompDesignFlow;
	using PlantUtilities::SetComponentFlowRate;
	using DataLoopNode::Node;

	void PlantComponentPrototype::initialize() {
		
		if ( this->oneTimeInit ) {
			this->typeOfNum = TypeOf_PrototypePlantComponent;
			this->designMassFlowRate = this->designVolFlowRate * 998.0;
			// Locate the unit on the plant loops for later usage
			bool errFlag( false );
			ScanPlantLoopsForObject( this->name, this->typeOfNum, this->loc.loopNum, this->loc.loopSideNum, this->loc.branchNum, this->loc.compNum, _, _, _, _, _, errFlag );
			if ( errFlag ) {
				ShowFatalError( "PlantComponent:Prototype: Program terminated due to previous condition(s)." );
			}
			RegisterPlantCompDesignFlow( this->loc.inletNodeNum, PlantLoop( this->loc.loopNum ).MaxVolFlowRate );
			this->oneTimeInit = false;
		}

		//begin environment inits
		if ( DataGlobals::BeginEnvrnFlag && this->beginEnvrnFlag ) {
			InitComponentNodes( 0.0, this->designMassFlowRate, this->loc.inletNodeNum, this->loc.outletNodeNum, this->loc.loopNum, this->loc.loopSideNum, this->loc.branchNum, this->loc.compNum );
			this->beginEnvrnFlag = false;
		}
		if ( ! DataGlobals::BeginEnvrnFlag ) this->beginEnvrnFlag = true;

		// set the current mass flow rate to design, then try to request it -- it may come back different
		this->loc.massFlowRate = this->designMassFlowRate;
		SetComponentFlowRate( this->loc.massFlowRate, this->loc.inletNodeNum, this->loc.outletNodeNum, this->loc.loopNum, this->loc.loopSideNum, this->loc.branchNum, this->loc.compNum );

		// also set up the inlet temperature
		this->loc.inletTemperature = Node( this->loc.inletNodeNum ).Temp;

	}

	void PlantComponentPrototype::calculate() {
		if ( this->loc.massFlowRate < 0.00001 ) {
			this->loc.outletTemperature = this->loc.inletTemperature;
		} else {
			this-> loc.outletTemperature = this->loc.inletTemperature + this->myCurrentLoad / ( this->loc.massFlowRate * 4180 );
		}
	}

	void PlantComponentPrototype::report() {
		Node( this->loc.outletNodeNum ).Temp = this->loc.outletTemperature;
	}

	namespace PlantComponentPrototypeNamespace {

		bool stillNeedToGetInput = true;
		std::vector< PlantComponentPrototype > prototypeComponents;
		
		// Main public interface for this plant component
		void simPrototype(
			std::string const & prototypeName,
			int & compIndex,
			bool const RunFlag,
			bool const InitLoopEquip,
			Real64 const & MyLoad,
			bool const FirstHVACIteration
		) {
			if ( stillNeedToGetInput ) fillDataFromInputProcessor();

			// Find the correct Equipment
			int eqNum = -1;
			if ( compIndex == 0 ) {
				// First time in, try to find this equipment by name
				for ( int tmpNum = 0; tmpNum <= prototypeComponents.size() - 1; tmpNum++ ) {
					if ( prototypeName == prototypeComponents[ tmpNum ].name ) {
						eqNum = tmpNum;
						break; 
					}
				}
				// If we didn't find it, fatal out; can't continue
				if ( eqNum == -1 ) {
					ShowFatalError( "Could not locate " + PlantComponentPrototype::objectName() + " = " + prototypeName );
				}
				// Finally store this in the argument reference so it can be used later
				compIndex = eqNum;
			} else {
				// All other times, pick up the equip number directly
				eqNum = compIndex;
				// But still check the equipment number for validity
				if ( prototypeComponents[ eqNum ].checkEquipName ) {
					int const numUnits( prototypeComponents.size() );
					if ( eqNum > numUnits - 1 || eqNum < 0 ) {
						ShowFatalError( "SimOutsideEnergy:  Invalid CompIndex passed=" + General::TrimSigDigits( eqNum ) + ", Max array bound=" + General::TrimSigDigits( numUnits - 1 ) + ", Entered Unit name=" + prototypeName );
					}
					if ( prototypeName != prototypeComponents[ eqNum ].name ) {
						ShowFatalError( "SimOutsideEnergy: Invalid CompIndex passed=" + General::TrimSigDigits( eqNum ) + ", Unit name=" + prototypeName + ", stored Unit Name for that index=" + prototypeComponents[ eqNum ].name );
					}
					prototypeComponents[ eqNum ].checkEquipName = false;
				}
			}

			// Create a reference to the class
			auto & thisPrototype( prototypeComponents[ eqNum ] );
			
			// Then simulate it
			thisPrototype.myCurrentLoad = MyLoad;
			thisPrototype.initialize();
			thisPrototype.calculate();
			thisPrototype.report();

		}

		void fillDataFromInputProcessor() {
			// Do all processing for normal operation:
			// GetObjectItem for arrays of data
			// Call addNewPrototypeFromInputData
			// Cross-reference multiple components after each are constructed, if necessary

			using namespace DataIPShortCuts;

			int NumAlphas;
			int NumNums;
			int IOStat;
			bool ErrorsFound( false );

			std::string const cCurrentModuleObject = PlantComponentPrototype::objectName();
			int const numPrototypes( InputProcessor::GetNumObjectsFound( cCurrentModuleObject ) );
			for ( int i = 0; i < numPrototypes; ++i ) {

				// Read inputs from the IP
				InputProcessor::GetObjectItem( cCurrentModuleObject, i+1, cAlphaArgs, NumAlphas, rNumericArgs, NumNums, IOStat, _, lAlphaFieldBlanks, cAlphaFieldNames );

				// Create an instance
				PlantComponentPrototype thisPrototype( createNewPrototypeFromInputData( cCurrentModuleObject, cAlphaArgs, rNumericArgs, ErrorsFound ) );

				// Process inlet/outlet nodes
				thisPrototype.loc.inletNodeNum = NodeInputManager::GetOnlySingleNode( thisPrototype.loc.inletNodeName, ErrorsFound, cCurrentModuleObject, thisPrototype.name, DataLoopNode::NodeType_Water, DataLoopNode::NodeConnectionType_Inlet, 1, DataLoopNode::ObjectIsNotParent );
				thisPrototype.loc.outletNodeNum = NodeInputManager::GetOnlySingleNode( thisPrototype.loc.outletNodeName, ErrorsFound, cCurrentModuleObject, thisPrototype.name, DataLoopNode::NodeType_Water, DataLoopNode::NodeConnectionType_Outlet, 1, DataLoopNode::ObjectIsNotParent );
				BranchNodeConnections::TestCompSet( cCurrentModuleObject, thisPrototype.name, thisPrototype.loc.inletNodeName, thisPrototype.loc.outletNodeName, "Hot Water Nodes" );

				// Construct a new class and add it to the end of the array
				prototypeComponents.push_back( thisPrototype );

				// Validate the name
				if ( i > 1 ) {
					if ( cAlphaArgs( 1 ) == "" ) {
						cAlphaArgs( 1 ) = "xxxxx";
					}
					for ( int j = 0; j < i; ++j ) {
						if ( prototypeComponents[ j ].name == cAlphaArgs( 1 ) ) {
							ShowSevereError( "Duplicate name found while processing " + cCurrentModuleObject + " objects; name = " + cAlphaArgs( 1 ) );
							ErrorsFound = true;
						}
					}
				}

			}

			if ( ErrorsFound ) {
				ShowFatalError( "Errors found in processing input for " + cCurrentModuleObject + ", Preceding condition caused termination." );
			}

			for ( int i = 0; i < numPrototypes; ++i ) {
				SetupOutputVariable( cCurrentModuleObject + " Mass Flow Rate [kg/s]", prototypeComponents[ i ].loc.massFlowRate, "System", "Average", prototypeComponents[ i ].name );
			}

			stillNeedToGetInput = false;
		}

		PlantComponentPrototype createNewPrototypeFromInputData( 
			std::string const & cCurrentModuleObject,
			FArray1D< std::string > & cAlphaArgs,
			FArray1D< Real64 > & rNumericArgs,
			bool & ErrorsFound
		) {
			PlantComponentPrototype tmpPrototype;
			
			// Process strings
			tmpPrototype.name = cAlphaArgs( 1 );
			tmpPrototype.loc.inletNodeName = cAlphaArgs( 2 );
			tmpPrototype.loc.outletNodeName = cAlphaArgs( 3 );
			
			// Process reals
			tmpPrototype.designVolFlowRate = rNumericArgs( 1 );

			return tmpPrototype;
		}

	}

}
