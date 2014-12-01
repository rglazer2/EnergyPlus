// EnergyPlus::PlantComponentPrototype Unit Tests

// Google Test Headers
#include <gtest/gtest.h>

// EnergyPlus Headers
#include <EnergyPlus/PlantComponentPrototype.hh>

using namespace EnergyPlus;
using namespace EnergyPlus::PlantComponentPrototypeNamespace;

TEST( PlantComponentPrototypeTest, PrototypeTestSuite )
{
	// Set some variables up
	bool errorsFound( false );
	PlantComponentPrototype p;
	FArray1D< std::string > alphas( {1,3}, {"PrototypeName", "InletNode", "OutletNode"} );
	FArray1D< Real64 > reals( {1,1}, 0.0001 );

	// Create a new instance
	p = createNewPrototypeFromInputData( "PlantComponentPrototype", alphas, reals, errorsFound );
	EXPECT_EQ( p.name, "PrototypeName" );

	// Not doing the initialize() call because it relies on many global things to be set up (PlantLoop, environment, etc.)
	// So setting mass flow rate up manually
	p.designMassFlowRate = p.designVolFlowRate * 998.0;
	EXPECT_TRUE( p.designMassFlowRate > 0.0009 && p.designMassFlowRate < 0.1001 );
	p.loc.massFlowRate = p.designMassFlowRate;

	p.loc.inletTemperature = 20;
	p.myCurrentLoad = 2000;
	p.calculate();
	EXPECT_TRUE( p.loc.outletTemperature > 24.79 && p.loc.outletTemperature < 24.80 );
}
