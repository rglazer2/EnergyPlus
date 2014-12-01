#ifndef PlantComponentPrototype_hh_INCLUDED
#define PlantComponentPrototype_hh_INCLUDED

// C++ Headers
#include <vector>

// ObjexxFCL Headers
#include <ObjexxFCL/FArray.functions.hh>

// EnergyPlus Headers
#include <PlantObjects.hh>

namespace EnergyPlus {

	class PlantComponentPrototype : public PlantObjectBase {
	public:

		// Convenience function for returning this component object name
		static std::string objectName() { return "PlantComponent:Prototype"; }

		// we already get name and typeofNum from ancestor, plus core init, calc, and report routines
		// this component will be a 1-connection component, so just get one plantlocation
		PlantLocation loc;
		
		// any other variables can go here
		Real64 designVolFlowRate;
		Real64 designMassFlowRate;
		Real64 constantTemperature;
		Real64 myCurrentLoad;
		Real64 currentHeatTransferRate;
		bool checkEquipName = true;
		bool oneTimeInit = true;
		bool beginEnvrnFlag = true;

		// declare the overrides here from the base class
		void initialize();
		void calculate();
		void report();

	};

	namespace PlantComponentPrototypeNamespace {

		// Flag for whether we still need to get input or not
		extern bool stillNeedToGetInput;

		// Main interface to this component
		void simPrototype( 
			std::string const & PrototypeName,
			int & CompIndex,
			bool const RunFlag,
			bool const InitLoopEquip,
			Real64 const & MyLoad,
			bool const FirstHVACIteration
		);

		// Read inputs from the input processor and fill array
		void fillDataFromInputProcessor();

		// Fill a single component class with data, add to the main array
		PlantComponentPrototype createNewPrototypeFromInputData(
			std::string const & cCurrentModuleObject,
			FArray1D< std::string > & cAlphaArgs,
			FArray1D< Real64 > & rNumericArgs,
			bool & ErrorsFound
		);

	}

	// Main storage array for classes of this component type
	extern std::vector< PlantComponentPrototype > prototypeComponents;

}

#endif
