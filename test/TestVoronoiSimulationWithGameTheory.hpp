

#ifndef TESTVORONOISIMULATIONWITHGAMETHEORY_HPP_
#define TESTVORONOISIMULATIONWITHGAMETHEORY_HPP_

#include <cxxtest/TestSuite.h>

#include <cstdio>
#include <ctime>
#include <cmath>

#include "NagaiHondaForce.hpp"
#include "SimpleTargetAreaModifier.hpp"

#include "CheckpointArchiveTypes.hpp"
#include "OffLatticeSimulation.hpp"
#include "HoneycombMeshGenerator.hpp"
#include "CylindricalHoneycombMeshGenerator.hpp"



#include "CellsGenerator.hpp"
#include "GameTheoryCellCycleModel.hpp"
#include "GeneralisedLinearSpringForce.hpp"
#include "ChemotacticForce.hpp"
#include "RandomCellKiller.hpp"
#include "PlaneBasedCellKiller.hpp"



#include "PlaneBoundaryCondition.hpp"
#include "AbstractCellBasedTestSuite.hpp"
#include "NeighbourTrackingModifier.hpp"
#include "NumericFileComparison.hpp"
#include "CellBasedEventHandler.hpp"
#include "WildTypeCellMutationState.hpp"
#include "DifferentiatedCellProliferativeType.hpp"
#include "CellLabel.hpp"
#include "TransitCellProliferativeType.hpp"
#include "SmartPointers.hpp"
#include "FileComparison.hpp"
#include "CellNeighboursWriter.hpp"


#include "CellMutationStatesWriter.hpp"
#include "CellProliferativeTypesWriter.hpp"
#include "CellIdWriter.hpp"

#include "PetscSetupAndFinalize.hpp"

class TestVoronoiSimulationWithGameTheory : public AbstractCellBasedTestSuite
{
private:

    double mLastStartTime;
    void setUp()
    {
        mLastStartTime = (double) std::clock();
        AbstractCellBasedTestSuite::setUp();
    }
    void tearDown()
    {
        double time = (double) std::clock();
        double elapsed_time = (time - mLastStartTime)/(CLOCKS_PER_SEC);
        std::cout << "Elapsed time: " << elapsed_time << std::endl;
        AbstractCellBasedTestSuite::tearDown();
    }

public:

    void TestGameTheoryDemo() throw (Exception) {
        EXIT_IF_PARALLEL;    // HoneycombMeshGenerator does not work in parallel

        // Create a simple 2D MeshBasedCellPopulation
        int num_cells_depth = 2;
        int num_cells_width = 2;

        HoneycombMeshGenerator generator(num_cells_width, num_cells_depth,0);
        MutableMesh<2,2>* p_mesh = generator.GetMesh();

        std::vector<CellPtr> cells;
        MAKE_PTR(WildTypeCellMutationState, p_state);
        MAKE_PTR(DifferentiatedCellProliferativeType, p_differentiated_type);
        MAKE_PTR(CellLabel, p_label);


        for (unsigned i=0; i<p_mesh->GetNumNodes(); i++) {
	    GameTheoryCellCycleModel* p_model = new GameTheoryCellCycleModel();
            p_model->SetDimension(2);

            CellPtr p_cell(new Cell(p_state, p_model));
            p_cell->SetCellProliferativeType(p_differentiated_type);
            double birth_time = -RandomNumberGenerator::Instance()->ranf()*18.0;
            p_cell->SetBirthTime(birth_time);

            // uncomment this for all cells to become cheaters
            //    p_cell->AddCellProperty(p_label);

            cells.push_back(p_cell);
        }


        MeshBasedCellPopulation<2> cell_population(*p_mesh, cells);

	//check equivalent for nodes
        cell_population.SetWriteVtkAsPoints(true);
        cell_population.SetOutputMeshInVtk(true);
	
	cell_population.AddCellWriter<CellMutationStatesWriter>();
	cell_population.AddCellWriter<CellProliferativeTypesWriter>();
	cell_population.AddCellWriter<CellIdWriter>();
	cell_population.AddCellWriter<CellNeighboursWriter>();

        // Set up cell-based simulation
        OffLatticeSimulation<2> simulator(cell_population);

        simulator.SetOutputDirectory("VoronoiGameTheoryFINALS1D5C06");
//        simulator.SetDt(0.01);
        simulator.SetSamplingTimestepMultiple(12);

        MAKE_PTR_ARGS(NeighbourTrackingModifier<2>, p_modifier, ());
        simulator.AddSimulationModifier(p_modifier);

        // Create a force law and pass it to the simulation
        MAKE_PTR(GeneralisedLinearSpringForce<2>, p_force);

        p_force->SetCutOffLength(1.5);
        simulator.AddForce(p_force);

	//add a new random seed
	srand(time(NULL));

        //enclose the population in boundaries
        c_vector<double, 2> point = zero_vector<double>(2);
        c_vector<double, 2> normal = zero_vector<double>(2);
        //normal(1) = -1.0;
        point(1) = 9.0;
        normal(1) = 9.0;
        MAKE_PTR_ARGS(PlaneBoundaryCondition<2>, p_bc, (&cell_population, point, normal));
        simulator.AddCellPopulationBoundaryCondition(p_bc);

        point(1) = -9.0;
        normal(1) = -9.0;

        MAKE_PTR_ARGS(PlaneBoundaryCondition<2>, p_bc_2, (&cell_population, point, normal));
	simulator.AddCellPopulationBoundaryCondition(p_bc_2);

        simulator.SetEndTime(100.0);

        simulator.Solve();
    }
};

#endif /*TESTVORONOISIMULATIONWITHGAMETHEORY_HPP_*/
