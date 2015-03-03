/*

Copyright (c) 2005-2014, University of Oxford.
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

#include "CellNeighboursWriter.hpp"
#include "AbstractCellPopulation.hpp"
#include "GameTheoryCellCycleModel.hpp"
#include <iostream>

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
CellNeighboursWriter<ELEMENT_DIM, SPACE_DIM>::CellNeighboursWriter()
: AbstractCellWriter<ELEMENT_DIM, SPACE_DIM>("cellneighbour.dat")
{
    this->mVtkCellDataName = "Cell Neighbours";
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
double CellNeighboursWriter<ELEMENT_DIM, SPACE_DIM>::GetCellDataForVtkOutput(CellPtr pCell, AbstractCellPopulation<ELEMENT_DIM, SPACE_DIM>* pCellPopulation)
{
    return pCell->GetCellData()->GetItem("NumNeighbours");

}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void CellNeighboursWriter<ELEMENT_DIM, SPACE_DIM>::VisitCell(CellPtr pCell, AbstractCellPopulation<ELEMENT_DIM, SPACE_DIM>* pCellPopulation)
{
//	unsigned location_index = pCellPopulation->GetLocationIndexUsingCell(pCell);
//	unsigned cell_id = pCell->GetCellId();
    c_vector<double, SPACE_DIM> centre_location = pCellPopulation->GetLocationOfCellCentre(pCell);
    unsigned max_neighbours = pCell->GetCellData()->GetItem("MaxNeighbours");
    double mean_neighbours = pCell->GetCellData()->GetItem("MeanNeighbours");
    unsigned min_neighbours = pCell->GetCellData()->GetItem("MinNeighbours");
    double max_fitness = pCell->GetCellData()->GetItem("MaxFitness");
    double mean_fitness = pCell->GetCellData()->GetItem("MeanFitness");
    double min_fitness = pCell->GetCellData()->GetItem("MinFitness");
    double perc_producers = pCell->GetCellData()->GetItem("PercProducers");
    double assortment = pCell->GetCellData()->GetItem("Assortment");
    double perc_two_neighbours = pCell->GetCellData()->GetItem("perc_two_neighbours");
    double perc_three_neighbours =pCell->GetCellData()->GetItem("perc_three_neighbours");
    double perc_four_neighbours =pCell->GetCellData()->GetItem("perc_four_neighbours");
    double perc_five_neighbours =pCell->GetCellData()->GetItem("perc_five_neighbours");
    double perc_six_neighbours =pCell->GetCellData()->GetItem("perc_six_neighbours");
    double perc_seven_neighbours =pCell->GetCellData()->GetItem("perc_seven_neighbours");
    double perc_eight_neighbours =pCell->GetCellData()->GetItem("perc_eight_neighbours");
    double perc_nine_neighbours =pCell->GetCellData()->GetItem("perc_nine_neighbours");
    double perc_ten_neighbours =pCell->GetCellData()->GetItem("perc_ten_neighbours");
    double perc_eleven_neighbours =pCell->GetCellData()->GetItem("perc_eleven_neighbours");
    double perc_twelve_neighbours =pCell->GetCellData()->GetItem("perc_twelve_neighbours");
    double perc_thirteen_neighbours =pCell->GetCellData()->GetItem("perc_thirteen_neighbours");

    if(pCell->GetCellData() -> GetItem ("Writer") == 0) {
	*this->mpOutStream 
	    << max_neighbours << " " 
	    << mean_neighbours << " " 
	    << min_neighbours << " "
	    << max_fitness << " "
	    << mean_fitness << " "
	    << min_fitness << " "
	    << perc_producers << " "
	    << assortment << " "
	    << perc_two_neighbours << " "
	    << perc_three_neighbours << " "
	    << perc_four_neighbours << " "
	    << perc_five_neighbours << " "
	    << perc_six_neighbours << " "
	    << perc_seven_neighbours << " "
	    << perc_eight_neighbours << " "
	    << perc_nine_neighbours << " "
	    << perc_ten_neighbours << " "
	    << perc_eleven_neighbours << " "
	    << perc_twelve_neighbours << " "
	    << perc_thirteen_neighbours << " ";
    }
}



// Explicit instantiation
template class CellNeighboursWriter<1,1>;
template class CellNeighboursWriter<1,2>;
template class CellNeighboursWriter<2,2>;
template class CellNeighboursWriter<1,3>;
template class CellNeighboursWriter<2,3>;
template class CellNeighboursWriter<3,3>;

#include "SerializationExportWrapperForCpp.hpp"
// Declare identifier for the serializer
EXPORT_TEMPLATE_CLASS_ALL_DIMS(CellNeighboursWriter)
