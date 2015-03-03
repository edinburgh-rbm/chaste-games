/*

Copyright (c) 2005-2013, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of Chaste.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

#include "NeighbourTrackingModifier.hpp"
#include "MeshBasedCellPopulation.hpp"
#include "RandomNumberGenerator.hpp"
#include "AbstractCellKiller.hpp"
#include "Node.hpp"
//#include "GameNode.hpp"
#include "BoundaryNodeWriter.hpp"
//#include "AbstractTissue.hpp"

#include<set>
#include <map>
//#include "simplemap.h"
//#include"map.h"
#include <iostream>
#include <cstdlib>
//#include <QtCore/QcoreApplication>
//#include <QMap>
//#include <QDebug>
//#include<unordered_map>

template<unsigned DIM>//,//>
//unsigned ELEMENT_DIM, unsigned SPACE_DIM>
NeighbourTrackingModifier<DIM>::NeighbourTrackingModifier()
: AbstractCellBasedSimulationModifier<DIM>()
  //, AbstractCellKiller<DIM>(rCellPopulation)
  //: AbstractCellKiller<DIM>(rCellPopulation)
  //AbstractCellPopulationWriter<ELEMENT_DIM, SPACE_DIM>()

  {
  }


template<unsigned DIM>
NeighbourTrackingModifier<DIM>::~NeighbourTrackingModifier()
{
}

template<unsigned DIM>
void NeighbourTrackingModifier<DIM>::UpdateAtEndOfTimeStep(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
{
    ticks += 1;
//    cout << "step " << ticks << ", cells " << rCellPopulation.GetNumNodes();
    flush(cout);

    UpdateCellData(rCellPopulation);
//    cout << "... done\n";
    flush(cout);
}

template<unsigned DIM>
void NeighbourTrackingModifier<DIM>::SetupSolve(AbstractCellPopulation<DIM,DIM>& rCellPopulation, std::string outputDirectory)
{
    ticks = 0;
    /*
     * We must update CellData in SetupSolve(), otherwise it will not have been
     * fully initialised by the time we enter the main time loop.
     */
    UpdateCellData(rCellPopulation);
}

template<unsigned DIM>
void NeighbourTrackingModifier<DIM>::OutputSimulationModifierParameters(out_stream& rParamsFile)
{
//    *rParamsFile << "\t\t\t<ReferenceTargetArea>" << mReferenceTargetArea << "</ReferenceTargetArea>\n";

    // Next, call method on direct parent class
    AbstractCellBasedSimulationModifier<DIM>::OutputSimulationModifierParameters(rParamsFile);
}


template<unsigned DIM>
void NeighbourTrackingModifier<DIM>::UpdateCellData(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
{
    // Make sure the cell population is updated
    rCellPopulation.Update();

    // Only works for Mesh based at the moment
    assert(dynamic_cast<MeshBasedCellPopulation<DIM>*>(&rCellPopulation));
    unsigned population_size = 0;

    //max population
    unsigned max_population = 300;

    // Iterate over cell population
    for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = rCellPopulation.Begin();
	 cell_iter != rCellPopulation.End();
	 ++cell_iter) {
	//Get size of cell population
	population_size++;
	
	// Get the location index corresponding to this cell
	unsigned index = rCellPopulation.GetLocationIndexUsingCell(*cell_iter);
	
	//const unsigned boundary_index = index;
	
	//bool boundary = GameNode<*boundary_index>::IsBoundaryNode();
	
	std::set<unsigned> neighbour_indices = rCellPopulation.GetNeighbouringNodeIndices(index);
	unsigned num_neighbours = neighbour_indices.size();

	// Store the number of neighbours
	cell_iter->GetCellData()->SetItem("NumNeighbours", num_neighbours);

	//Find if the cell is a cheater or a cooperator
	// default is cooperator (-1)
	signed int cell_type = -1;
	//get cell data using index
	CellPtr p_cell = rCellPopulation.GetCellUsingLocationIndex(index);
	//if it is a cheater this is true
	bool cell_cheater = p_cell->template HasCellProperty<CellLabel>();

	// cheater labeled -2, cooperator -1
	if (cell_cheater){
	    cell_type = -2;
	    //cout << " bool cell cheater " << cell_cheater;
	} else {
	    MAKE_PTR(DifferentiatedCellProliferativeType, p_differentiated_type);
	    cell_type = -1;
	    p_cell->SetCellProliferativeType(p_differentiated_type);
	}

	// Store the cell type
	cell_iter->GetCellData()->SetItem("CellType", cell_type);

	//Get the number of producers and number of cheaters
	//find what the neighbours are
	unsigned cooperators = 0;
	unsigned cheaters = 0;
	
	std::set<unsigned>::iterator it;
	
	//it iterator;
	//iterate over neighbours to find their nature
	for (it = neighbour_indices.begin(); it != neighbour_indices.end(); it++){
	    // get cell data using the index
	    CellPtr p_cell_A = rCellPopulation.GetCellUsingLocationIndex(*it);
	    
	    //if the cell is a cheater this is true
	    bool cell_A_cheater = p_cell_A->template HasCellProperty<CellLabel>();
	    //cout << " \nbool cell A cheater " << cell_A_cheater;
	    if (cell_A_cheater){
		cheaters = cheaters + 1;
	    } else {
		cooperators = cooperators + 1;
	    }
	}

	unsigned num_cheaters = cheaters;
	unsigned num_cooperators = cooperators;

	// Store the number of cheaters/producers
	cell_iter->GetCellData()->SetItem("NumCheaters", num_cheaters);
	cell_iter->GetCellData()->SetItem("NumCooperators", num_cooperators);

	// the following checks neighbours in regards to diffusion
	// (to be used in the public good equation accordingly

	//change here to increase diffusion
	unsigned diffusion = 5;

	unsigned diffusion_neighbours = num_neighbours;
	std::set<unsigned> diffusion_neighbour_indices = neighbour_indices;

	unsigned diffusion_num_cheaters = num_cheaters;
	unsigned diffusion_num_cooperators = num_cooperators;

	//in case diffusion is bigger than 1
	
	if (diffusion > 1) {
	    std::set<unsigned>::iterator it;
	    std::set<unsigned> diffusion_neighbour_indices_new;
	    std::set<unsigned> set_difference;
	    std::set<unsigned> difference;

	    for (unsigned i = 0; i < diffusion-1; i++) {
		//it iterator;
		//iterate over neighbours to find the subsequent wave of neighbours
		for (it = diffusion_neighbour_indices.begin(); it != diffusion_neighbour_indices.end(); it++){
		    // get cell data using the index
		    diffusion_neighbour_indices_new = rCellPopulation.GetNeighbouringNodeIndices(*it);
		    //}

		    // sort sets
		    // according to http://www.cplusplus.com/reference/stl/set/
		    // The elements in the container follow a strict order at all times. All inserted elements are given a position in this order.
		    // as such no need to sort them

		    //difference contains all elements that are in diffusion_neighbour_indices_new, but not in diffusion_neighbour_indices
		    std::set_difference(diffusion_neighbour_indices_new.begin(), diffusion_neighbour_indices_new.end(),
					diffusion_neighbour_indices.begin(), diffusion_neighbour_indices.end(),
					std::inserter(difference, difference.begin()));
		}
		//add the neighbours to the diffusion_neighbour_indices
		diffusion_neighbour_indices.insert(difference.begin(), difference.end());
	    }

	    //remove current cell
	    diffusion_neighbour_indices.erase(diffusion_neighbour_indices.find(index));

	    //get the new number of diffusion neighbours
	    diffusion_neighbours = diffusion_neighbour_indices.size();

	    // repeat as before to find the number of cheaters and the number of producers

	    //Get the number of producers and number of cheaters
	    //find what the neighbours are
	    unsigned cooperators_diffusion = 0;
	    unsigned cheaters_diffusion = 0;
	    std::set<unsigned>::iterator it_diffusion;

	    //it iterator;
	    //iterate over neighbours to find their nature
	    for (it_diffusion = diffusion_neighbour_indices.begin(); 
		 it_diffusion != diffusion_neighbour_indices.end(); 
		 it_diffusion++) {
		// get cell data using the index
		CellPtr p_cell_Diffusion = rCellPopulation.GetCellUsingLocationIndex(*it_diffusion);

		//if the cell is a cheater this is true
		bool cell_Diffusion_cheater = p_cell_Diffusion->template HasCellProperty<CellLabel>();

		if (cell_Diffusion_cheater) {
		    cheaters_diffusion = cheaters_diffusion + 1;
		} else {
		    cooperators_diffusion = cooperators_diffusion + 1;
		}
	    }
	    diffusion_num_cheaters = cheaters_diffusion;
	    diffusion_num_cooperators = cooperators_diffusion;
	}
	
	// Store the number of diffusion cheaters/producers
	cell_iter->GetCellData()->SetItem("DiffNumCheaters", diffusion_num_cheaters);
	cell_iter->GetCellData()->SetItem("DiffNumCooperators", diffusion_num_cooperators);
	// Store the number of diffusion neighbours
	cell_iter->GetCellData()->SetItem("DiffNumNeighbours", diffusion_neighbours);
    }

    // Iterate over cell population and clear fitness
    for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = rCellPopulation.Begin();
	 cell_iter != rCellPopulation.End();
	 ++cell_iter) {
	double cost = 0.6;

	cell_iter->GetCellData()->SetItem("Fitness", 0.0);

	// Get the location index corresponding to this cell
	unsigned index = rCellPopulation.GetLocationIndexUsingCell(*cell_iter);

	CellPtr p_cell = rCellPopulation.GetCellUsingLocationIndex(index);

	//if I simply call it 0: Assertion `mStepsTillDivision>0' failed.
	double cell_fitness = p_cell->GetCellData()->GetItem("Fitness");
	double num_neighbours_total = p_cell->GetCellData()->GetItem("DiffNumNeighbours");
	double num_cooperators_total = p_cell->GetCellData()->GetItem("DiffNumCooperators");

	//** uncomment below for a reverse prisoner's dilemma
	//double num_cheaters_total = p_cell->GetCellData()->GetItem("DiffNumCheaters");

	//constract a sigmoid fitness based on the neighbours
	double sigmoid = 1;
	double effect = 2;

	//General Benefit equation 1/[1 + exp(-s*(j-k)/n]
	//V(j)
	double exponential = exp (-sigmoid * (num_cooperators_total - (num_neighbours_total/effect))/num_neighbours_total);
	double benefitGeneral = 1/(1 + exponential);

	//** uncomment below for a reverse prisoner's dilemma (Up to line 352)
	//** the value 1.1 represent the increase in public good production of the cheaters
	//** increase for greater effect. Since it is a reverse
	//** prisoner's dilemma, the cheaters produce more public
	//** good (i.e. division factor), than the producers
	//double percentage_increase = 1.1;
	//if ((num_cheaters_total * percentage_increase) < num_neighbours_total){
	//	num_cheaters_total = num_cheaters_total * percentage_increase;
	//}else{
	//	num_cheaters_total = num_neighbours_total;
	//}
	//double exponential_cheater = exp (-sigmoid * (num_cheaters_total - (num_neighbours_total/effect))/num_neighbours_total);
	
	//double benefitGeneral_cheater = 1/(1 + exponential_cheater);

	//V(n)
	exponential = exp (-sigmoid * (num_neighbours_total - (num_neighbours_total/effect))/num_neighbours_total);
	double benefitTotal = 1/(1 + exponential);

	//V(0)
	exponential = exp (-sigmoid * (0 - (num_neighbours_total/effect))/num_neighbours_total);
	double benefitZero = 1/(1 + exponential);

	//Specific Benefit Equation [V(j) - V(0) / V(n)-V(0)]
	double benefitSpecific = (benefitGeneral - benefitZero)/(benefitTotal - benefitZero);
	
        //** uncomment below for a reverse prisoner's dilemma
	//double benefitSpecificCheater = (benefitGeneral_cheater - benefitZero)/(benefitTotal - benefitZero);
	//avoid 0/number
	if (benefitZero == benefitGeneral) {
	    benefitSpecific = 0;
	}

	//** uncomment below for a reverse prisoner's dilemma
	//avoid 0/number
	//if (benefitZero == benefitGeneral_cheater){
	//benefitSpecificCheater = 0;
	//}


	//add the cost for the producers (if it has a label it is a cheater)
	//first check if the cell is a producer
	bool cell_cheater = p_cell->template HasCellProperty<CellLabel>();
	//cout << " \ncell_cheater " << cell_cheater;
	
	//if cell is a cheater give payoff
	//traditional prisoners dilemma
	if (cell_cheater){
	    //** unshade below for Prisoners Dilemma
	    cell_fitness = benefitSpecific;
	    
	    //** unshade below for Reverse Prisoners Dilemma
	    //** the value 1.1 represents the increase in cost of the cheaters
	    //cell_fitness = (benefitSpecific + benefitSpecificCheater) - (cost * percentage_increase);
	    
	    
	    //** unshade below for Coordination Game, i.e. cheaters divide irrespective of cooperators
	    //** if I use '1' the causes error to ReadyToDivide
	    //** as it does not update, meaning that occasionally mStepTillDivision is
	    //** negative when it reaches ReadyToDivide. Tried to check why, could not
	    //** find the reason.
	    //cell_fitness = 0.99;


	    // if cell is a producer give payoff - cost
	} else {
	    //** unshade below for Prisoners Dilemma
	    cell_fitness = (benefitSpecific - cost);
	    //** unshade below for Reverse Prisoners Dilemma
	    //cell_fitness = (benefitSpecific + benefitSpecificCheater) - (cost);
	}

	p_cell->GetCellData()->SetItem("Fitness",cell_fitness );
    }

    //celldeath
    if (rCellPopulation.GetNumAllCells() >= max_population) {
	signed lowest = -1;
	signed highest = -1;
	double cell_fitness_initial = 0;
	double cell_fitness_initial_death = 0;
	
	double select_death = 0;
	double select_divide = 0;
	//a map of the id and fitness together. This will be used to create an abstract fitness line
	std::map<double, unsigned> cell_id_fitness;
	std::map<double, unsigned> cell_id_fitness_death;
	
	cell_id_fitness[cell_fitness_initial] = 0;
	cell_id_fitness_death[cell_fitness_initial_death] = 0;
	
	//check through the population and find the fitness
	for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = rCellPopulation.Begin();
	     cell_iter != rCellPopulation.End();
	     ++cell_iter) {
	    // Get the location index corresponding to this cell
	    unsigned index = rCellPopulation.GetLocationIndexUsingCell(*cell_iter);
	    CellPtr p_cell = rCellPopulation.GetCellUsingLocationIndex(index);
	    double cell_fitness = p_cell->GetCellData()->GetItem("Fitness");//
	    double cell_fitness_death = 1/cell_fitness;

	    // make sure all data can go sequentially on the abstract line
	    if (cell_fitness < 0) {
		//cell_fitness = cell_fitness * (-1);
		//if (cell_fitness < 1){
		cell_fitness = 0;
	    }

	    if (cell_fitness_death < 0) {
		double _pow = cell_fitness * (-10);
		cell_fitness_death = pow (100, _pow);
	    }

	    cell_fitness_initial = cell_fitness_initial+ cell_fitness;
	    cell_fitness_initial_death = cell_fitness_initial_death + cell_fitness_death;

	    cell_id_fitness[cell_fitness_initial] = index;
	    cell_id_fitness_death[cell_fitness_initial_death] = index;
	}
	//assign for death the first element (i.e. the one with the lower fitness)
	//select a random number representing the cell to die
	//add a new random seed
	//srand(time(NULL));

	select_divide = fmod(rand(), cell_fitness_initial);
	select_death = fmod(rand(), cell_fitness_initial_death);

	//unsigned test;
	double previous_element = 0;
	//select element to divide

	for(std::map<double, unsigned>::const_iterator it = cell_id_fitness.begin(); 
	    it != cell_id_fitness.end();
	    ++it) {
	    if ((previous_element < select_divide && it->first > select_divide) ||
		it->first == select_divide) {
		highest = it->second;
	    }
	    previous_element = it->first;
	}

	//select element to die
	previous_element = 0;
	for(std::map<double, unsigned>::const_iterator it = cell_id_fitness_death.begin(); 
	    it != cell_id_fitness_death.end(); 
	    ++it) {
// AMBIGUOUS PARENS
//			if ((previous_element < select_death) && (it -> first > select_death)|| it->first == select_death){
	    if (((previous_element < select_death) && (it -> first > select_death)) || it->first == select_death){
		lowest = it->second;
	    }
	    previous_element = it->first;
	}

	for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = rCellPopulation.Begin();
	     cell_iter != rCellPopulation.End();
	     ++cell_iter) {
	    // Get the location index corresponding to this cell
	    unsigned index = rCellPopulation.GetLocationIndexUsingCell(*cell_iter);

	    if (index == lowest) {
		cell_iter->Kill();
	    }
	    // to ensure that only one cell divides at each time slot
	    if (index == highest) {
		cell_iter->GetCellData()->SetItem("mStepsTillDivision", 0.01);
	    } else {
		cell_iter->GetCellData()->SetItem("mStepsTillDivision", 2000);
	    }
	}
    }


    // find boundaries
    unsigned diffusion = 5;
    std::set<unsigned>boundary_nodes;
    for (typename AbstractMesh<DIM, DIM>::NodeIterator node_iter = rCellPopulation.rGetMesh().GetNodeIteratorBegin();
	 node_iter != rCellPopulation.rGetMesh().GetNodeIteratorEnd();
	 ++node_iter) {
	if(node_iter->IsBoundaryNode()) {
	    unsigned index = node_iter->GetIndex();
	    boundary_nodes.insert(index);
	}
    }

    // find the neighbours of the boundary nodes and call them boundary
    if (diffusion > 1) {
	std::set<unsigned>::iterator it;
	std::set<unsigned> boundary_nodes_new;
	std::set<unsigned> set_difference;
	std::set<unsigned> difference;

	for (unsigned i = 0; i < diffusion-1; i++) {
	    //it iterator;
	    //iterate over neighbours to find the subsequent wave of neighbours
	    
	    for (it = boundary_nodes.begin(); it != boundary_nodes.end(); it++){
		// get cell data using the index
		boundary_nodes_new = rCellPopulation.GetNeighbouringNodeIndices(*it);
		//}
		// sort sets
		// according to http://www.cplusplus.com/reference/stl/set/
		// The elements in the container follow a strict order at all times. All inserted elements are given a position in this order.
		// as such no need to sort them
		
		std::set_difference(boundary_nodes_new.begin(), boundary_nodes_new.end(),
				    boundary_nodes.begin(), boundary_nodes.end(),
				    std::inserter(difference, difference.begin()));
		
	    }
	    //add the neighbours to the diffusion_neighbour_indices
	    boundary_nodes.insert(difference.begin(), difference.end());
	}
    }

    for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = rCellPopulation.Begin();
	 cell_iter != rCellPopulation.End();
	 ++cell_iter) {
	signed boundary = -5;
	
	cell_iter->GetCellData()->SetItem("Boundary", boundary);
	unsigned index = rCellPopulation.GetLocationIndexUsingCell(*cell_iter);

	for (std::set<unsigned>::iterator it = boundary_nodes.begin() ; 
	     it != boundary_nodes.end(); 
	     ++it) {
	    if (index == *it){
		boundary = -10;
		cell_iter->GetCellData()->SetItem("Boundary", boundary);
	    }
	}
    }

    //find the average, min, and max of relevant values, boundary nodes are
    //ignored from the calculations. Extent of boundary nodes depends on diffusion
    double count_producers =0;
    unsigned count_cheaters = 0;
    double percentage_producers = 0;
    unsigned min_neighbours = 1000;
    unsigned max_neighbours = 0;
    double mean_neighbours = 0;
    double max_fitness = 0;
    double min_fitness = 100;
    double mean_fitness = 0;
    double general_counter = 0;
    
    double assortment_cheater = 0;
    double assortment_cooperator = 0;
    double assortment_general = 0;
    
    double two_neighbours =0;
    double three_neighbours =0;
    double four_neighbours =0;
    double five_neighbours =0;
    double six_neighbours =0;
    double seven_neighbours =0;
    double eight_neighbours =0;
    double nine_neighbours =0;
    double ten_neighbours =0;
    double eleven_neighbours =0;
    double twelve_neighbours =0;
    double thirteen_neighbours =0;
    
    double perc_two_neighbours =0;
    double perc_three_neighbours =0;
    double perc_four_neighbours =0;
    double perc_five_neighbours =0;
    double perc_six_neighbours =0;
    double perc_seven_neighbours =0;
    double perc_eight_neighbours =0;
    double perc_nine_neighbours =0;
    double perc_ten_neighbours =0;
    double perc_eleven_neighbours =0;
    double perc_twelve_neighbours =0;
    double perc_thirteen_neighbours =0;

    for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = rCellPopulation.Begin();
	 cell_iter != rCellPopulation.End();
	 ++cell_iter) {
	unsigned index = rCellPopulation.GetLocationIndexUsingCell(*cell_iter);

	CellPtr p_cell = rCellPopulation.GetCellUsingLocationIndex(index);

	signed cell_type = p_cell->GetCellData()->GetItem("CellType");
	//counts direct neighbours
	unsigned number_neighbours = p_cell->GetCellData()->GetItem("NumNeighbours");
	//counts diffusion neighbours
	//double diff_number_coop_neighbours = p_cell->GetCellData()->GetItem("DiffNumCooperators");
	//double diff_number_neighbours = p_cell->GetCellData()->GetItem("DiffNumNeighbours");
	
	//counts direct neighbours
	double diff_number_coop_neighbours = p_cell->GetCellData()->GetItem("NumCooperators");
	double diff_number_neighbours = p_cell->GetCellData()->GetItem("NumNeighbours");

	double individual_fitness = p_cell->GetCellData()->GetItem("Fitness");
	signed individual_boundary = p_cell->GetCellData()->GetItem("Boundary");

	// if cells are not boundary nodes
	if (individual_boundary == -5) {
	    //if cell is a producer or cheater
	    if (cell_type == -2) {
		count_cheaters++;
		assortment_cheater = assortment_cheater + (diff_number_coop_neighbours/diff_number_neighbours);
	    } else if (cell_type == -1) {
		count_producers++;
		assortment_cooperator = assortment_cooperator + (diff_number_coop_neighbours/diff_number_neighbours);
	    }

	    // find lowest fitness
	    if (individual_fitness < min_fitness) {
		min_fitness = individual_fitness;
	    }
	
	    // find highest fitness
	    if (individual_fitness > max_fitness) {
		max_fitness = individual_fitness;
	    }

	    // find lowest neighbours
	    if (number_neighbours < min_neighbours) {
		min_neighbours = number_neighbours;
	    }
	    // find highest neighbours
	    if (number_neighbours > max_neighbours) {
		max_neighbours = number_neighbours;
	    }

	    //frequencies of neighbours
	    if (number_neighbours == 2) {
		two_neighbours++;
	    }

	    if (number_neighbours == 3) {
		three_neighbours++;
	    }

	    if (number_neighbours == 4) {
		four_neighbours++;
	    }

	    if (number_neighbours == 5) {
		five_neighbours++;
	    }

	    if (number_neighbours == 6) {
		six_neighbours++;
	    }

	    if (number_neighbours == 7) {
		seven_neighbours++;
	    }

	    if (number_neighbours == 8) {
		eight_neighbours++;
	    }

	    if (number_neighbours == 9) {
		nine_neighbours++;
	    }

	    if (number_neighbours == 10) {
		ten_neighbours++;
	    }

	    if (number_neighbours == 11) {
		eleven_neighbours++;
	    }

	    if (number_neighbours == 12) {
		twelve_neighbours++;
	    }

	    if (number_neighbours == 13) {
		thirteen_neighbours++;
	    }

	    //add to the mean
	    mean_neighbours = mean_neighbours + number_neighbours;
	    mean_fitness = mean_fitness + individual_fitness;
	    general_counter++;
	}
    }

    if (general_counter > 0) {
	mean_neighbours = mean_neighbours/general_counter;
	mean_fitness = mean_fitness/general_counter;
	percentage_producers = (count_producers/general_counter) * 100;
	//cout << "percentage_producers = " <<percentage_producers <<endl;
	
	if (two_neighbours > 0) {
	    perc_two_neighbours = (two_neighbours/general_counter) * 100;
	}
	if (three_neighbours> 0) {
	    perc_three_neighbours = (three_neighbours/general_counter) * 100;
	}
	if (four_neighbours> 0) {
	    perc_four_neighbours = (four_neighbours/general_counter) * 100;
	}
	if (five_neighbours> 0) {
	    perc_five_neighbours = (five_neighbours/general_counter) * 100;
	}
	if (six_neighbours> 0) {
	    perc_six_neighbours = (six_neighbours/general_counter) * 100;
	}
	if (seven_neighbours> 0) {
	    perc_seven_neighbours = (seven_neighbours/general_counter) * 100;
	}
	if (eight_neighbours> 0) {
	    perc_eight_neighbours = (eight_neighbours/general_counter) * 100;
	}
	if (nine_neighbours> 0) {
	    perc_nine_neighbours = (nine_neighbours/general_counter) * 100;
	}
	if (ten_neighbours> 0) {
	    perc_ten_neighbours = (ten_neighbours/general_counter) * 100;
	}
	if (eleven_neighbours> 0) {
	    perc_eleven_neighbours = (eleven_neighbours/general_counter) * 100;
	}
	if (twelve_neighbours> 0) {
	    perc_twelve_neighbours = (twelve_neighbours/general_counter) * 100;
	}
	if (thirteen_neighbours> 0) {
	    perc_thirteen_neighbours = (thirteen_neighbours/general_counter) * 100;
	}
    }

    if (assortment_cooperator > 0 && count_producers > 0) {
	assortment_cooperator = assortment_cooperator/count_producers;
    }

    if (assortment_cheater > 0 && count_cheaters > 0) {
	assortment_cheater = assortment_cheater/count_cheaters;
    }

    assortment_general = assortment_cooperator - assortment_cheater;

    //pass the information in all the cells
    unsigned writer = 0;
    for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = rCellPopulation.Begin();
	 cell_iter != rCellPopulation.End();
	 ++cell_iter) {
	cell_iter->GetCellData()->SetItem("MaxNeighbours", max_neighbours);
	cell_iter->GetCellData()->SetItem("MeanNeighbours", mean_neighbours);
	cell_iter->GetCellData()->SetItem("MinNeighbours", min_neighbours);
	cell_iter->GetCellData()->SetItem("MaxFitness", max_fitness);
	cell_iter->GetCellData()->SetItem("MeanFitness", mean_fitness);
	cell_iter->GetCellData()->SetItem("MinFitness", min_fitness);
	cell_iter->GetCellData()->SetItem("PercProducers", percentage_producers);
	cell_iter->GetCellData()->SetItem("Assortment", assortment_general);
	cell_iter->GetCellData()->SetItem("perc_two_neighbours", perc_two_neighbours);
	cell_iter->GetCellData()->SetItem("perc_three_neighbours", perc_three_neighbours);
	cell_iter->GetCellData()->SetItem("perc_four_neighbours", perc_four_neighbours);
	cell_iter->GetCellData()->SetItem("perc_five_neighbours", perc_five_neighbours);
	cell_iter->GetCellData()->SetItem("perc_six_neighbours", perc_six_neighbours);
	cell_iter->GetCellData()->SetItem("perc_seven_neighbours", perc_seven_neighbours);
	cell_iter->GetCellData()->SetItem("perc_eight_neighbours", perc_eight_neighbours);
	cell_iter->GetCellData()->SetItem("perc_nine_neighbours", perc_nine_neighbours);
	cell_iter->GetCellData()->SetItem("perc_ten_neighbours", perc_ten_neighbours);
	cell_iter->GetCellData()->SetItem("perc_eleven_neighbours", perc_eleven_neighbours);
	cell_iter->GetCellData()->SetItem("perc_twelve_neighbours", perc_twelve_neighbours);
	cell_iter->GetCellData()->SetItem("perc_thirteen_neighbours", perc_thirteen_neighbours);
	cell_iter->GetCellData()->SetItem("Writer", writer);
	writer = 1;
    }
}


/////////////////////////////////////////////////////////////////////////////
// Explicit instantiation
/////////////////////////////////////////////////////////////////////////////

template class NeighbourTrackingModifier<1>;
template class NeighbourTrackingModifier<2>;
template class NeighbourTrackingModifier<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(NeighbourTrackingModifier)



