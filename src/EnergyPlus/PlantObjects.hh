#ifndef PlantObjects_hh_INCLUDED
#define PlantObjects_hh_INCLUDED

// C++ Headers
#include <string>

// EnergyPlus Headers
#include <DataPrecisionGlobals.hh>

namespace EnergyPlus {

class PlantLocation {
public:
	int loopNum = -1;
	int loopSideNum = -1;
	int branchNum = -1;
	int compNum = -1;
	int inletNodeNum = -1;
	int outletNodeNum = -1;
	std::string inletNodeName = "";
	std::string outletNodeName = "";
	Real64 inletTemperature;
	Real64 outletTemperature;
	Real64 massFlowRate;
	Real64 volumeFlowRate;

	void setPlantLocation(
		int _loopNum,
		int _loopSideNum,
		int _branchNum,
		int _compNum
	) {
		loopNum = _loopNum;
		loopSideNum = _loopSideNum;
		branchNum = _branchNum;
		compNum = _compNum;
	}

	void setInletOutletNodes(
		int _inletNodeNum,
		int _outletNodeNum
	) {
		inletNodeNum = _inletNodeNum;
		outletNodeNum = _outletNodeNum;
	}

};

class PlantObjectBase {
public:

	// All plant components should have a name
	std::string name = " ";
	
	// All plant components should have a type-of number
	int typeOfNum = -1;
	
	// All plant components should follow a pattern of these (pure virtual) functions
	virtual void initialize() = 0;
	virtual void calculate() = 0;
	virtual void report() = 0;

};

}

#endif
