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

#include "GameTheoryCellCycleModel.hpp"
#include "Exception.hpp"
#include "StemCellProliferativeType.hpp"
#include "TransitCellProliferativeType.hpp"
#include "DifferentiatedCellProliferativeType.hpp"

GameTheoryCellCycleModel::GameTheoryCellCycleModel()
: AbstractCellCycleModel(),
  // for some reason when steps are 200 causes error to ReadyToDivide
  // as it does not update, meaning that occasionally mStepTillDivision is
  // negative when it reaches ReadyToDivide. Tried to check why, could not
  // find the reason.
  mStepsTillDivision(2000.0)
{
}

AbstractCellCycleModel* GameTheoryCellCycleModel::CreateCellCycleModel()
{
    // Create a new cell-cycle model
    GameTheoryCellCycleModel* p_model = new GameTheoryCellCycleModel();

    /*
     * Set each member variable of the new cell-cycle model that inherits
     * its value from the parent.
     *
     * Note 1: some of the new cell-cycle model's member variables (namely
     * mBirthTime, mCurrentCellCyclePhase, mReadyToDivide) will already have been
     * correctly initialized in its constructor.
     *
     * Note 2: one or more of the new cell-cycle model's member variables
     * may be set/overwritten as soon as InitialiseDaughterCell() is called on
     * the new cell-cycle model.
     *
     * Note 3: the member variable mDimension remains unset, since this cell-cycle
     * model does not need to know the spatial dimension, so if we were to call
     * SetDimension() on the new cell-cycle model an exception would be triggered;
     * hence we do not set this member variable.
     */
    p_model->SetBirthTime(mBirthTime);
    p_model->SetMinimumGapDuration(mMinimumGapDuration);
    p_model->SetStemCellG1Duration(mStemCellG1Duration);
    p_model->SetTransitCellG1Duration(mTransitCellG1Duration);
    p_model->SetSDuration(mSDuration);
    p_model->SetG2Duration(mG2Duration);
    p_model->SetMDuration(mMDuration);
    p_model->SetStepsTillDivision(2000.0);
    
    return p_model;
}

void GameTheoryCellCycleModel::InitialiseDaughterCell() {
    if (!mpCell->template HasCellProperty<CellLabel>()) {
	// chance that a daugher will be mutant
	bool mutant = (rand() % 1001) > 999;
	// not everything below is necessary, but since I
	//occasionally get some random mutants these are
	//things I tried to overcome it.
	if (mutant){
	    MAKE_PTR(CellLabel, p_label);
	    mpCell->AddCellProperty(p_label);
	    mpCell->GetCellData()->SetItem("CellType", -2);
	} else {
	    mpCell->GetCellData()->SetItem("CellType", -1);
	}
    }
}


bool GameTheoryCellCycleModel::ReadyToDivide() {
    assert(mpCell != NULL);
    assert(mStepsTillDivision>0);

    if (!mReadyToDivide) {
	// decrease mStepsTillDivision by the cells fittness
	// if cooperator surrounded by cheaters the mStepsTillDivision
	//will increase possibly (Fitness could be negative)
	
	mStepsTillDivision -= mpCell->GetCellData()->GetItem("Fitness");
	
	mpCell->GetCellData()->SetItem("StepsTillDivision",mStepsTillDivision);
	if ( mStepsTillDivision < 0 ) {
	    mReadyToDivide = true;
	    std::cout << "we are ready to divide!\n";
	}
    }
    return mReadyToDivide;
}

void GameTheoryCellCycleModel::ResetForDivision() {
    assert(mReadyToDivide);
    assert(mStepsTillDivision<0);

    AbstractCellCycleModel::ResetForDivision();
    mStepsTillDivision = 2000.0;
}

void GameTheoryCellCycleModel::OutputCellCycleModelParameters(out_stream& rParamsFile) {
    // No new parameters to output, so just call method on direct parent class
    AbstractCellCycleModel::OutputCellCycleModelParameters(rParamsFile);
}

void GameTheoryCellCycleModel::SetStepsTillDivision(double stepsTillDivision) {
    mStepsTillDivision = stepsTillDivision;
    //std::cout << " \nGameTheoryCellCycleModel::SetStepsTillDivision" << mStepsTillDivision;
}

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
CHASTE_CLASS_EXPORT(GameTheoryCellCycleModel)
