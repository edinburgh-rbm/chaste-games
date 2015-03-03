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

#ifndef GAMETHEORYCELLCYCLEMODEL_HPP_
#define GAMETHEORYCELLCYCLEMODEL_HPP_

#include "AbstractCellCycleModel.hpp"
#include "RandomNumberGenerator.hpp"

/**
 *
 */
class GameTheoryCellCycleModel : public AbstractCellCycleModel
{

private:

    /*
     * A counter for specifying when cell divides, decreased by the fitness at each step.
     */
    double mStepsTillDivision;

    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Archive the cell-cycle model and random number generator, never used directly - boost uses this.
     *
     * @param archive the archive
     * @param version the current version of this class
     */
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version) {
        archive & boost::serialization::base_object<AbstractCellCycleModel>(*this);
        archive & mStepsTillDivision;
    }

public:

    /**
     * Constructor - just a default, mBirthTime is now set in the AbstractCellCycleModel class.
     * mG1Duration is set very high, it is set for the individual cells when InitialiseDaughterCell is called
     */
    GameTheoryCellCycleModel();

    /**
     * Overridden builder method to create new copies of
     * this cell-cycle model.
     */
    AbstractCellCycleModel* CreateCellCycleModel();

    /**
     * Initialise the new daughter cell's cycle model after a cell division.
     *
     * This is called by Cell::Divide once the new cell object
     * has been fully created, to perform any initialisation of the
     * cell cycle which requires access to the cell.
     *
     * Note that much initialisation can be performed using the
     * combination of ResetForDivision() (called on the parent prior to
     * division) and CreateCellCycleModel() (called on the reset
     * parent to create the new cell-cycle model object).
     */
    virtual void InitialiseDaughterCell();

    /**
     * This method must be implemented by subclasses in order to set the phase
     * the cell-cycle model is currently in. It is called from ReadyToDivide()
     * just prior to deciding whether to divide the cell based on how far through
     * the cell cycle it is, i.e. whether it has completed M, G1, S and G2 phases.
     *
     *
     */
    virtual void UpdateCellCyclePhase() { assert(0); }

    /**
     * Overridden ReadyToDivide Method to decrease the division counter
     * and decide when to divide.
     *
     *
     *
     * Determine whether the cell is ready to divide (enter M phase).
     *
     * The intention is that this method is called precisely once at
     * each timestep of the simulation. However this does not appear
     * to always be the case at present, and so it can cope with more
     * unusual usage patterns.
     */
    virtual bool ReadyToDivide();

    /**
     * Overridden ResetForDivision Method to reset the mStepsTillDivision counter
     *
     * Each cell-cycle model must be able to be reset 'after' a cell division.
     *
     * Actually, this method is called from Cell::Divide() to
     * reset the cell cycle just before the daughter cell is created.
     * CreateCellCycleModel() can then clone our state to generate a
     * cell-cycle model instance for the daughter cell.
     */
    virtual void ResetForDivision();

    /**
     * Setter method
     */
    void SetStepsTillDivision(double stepsTillDivision);

    /**
     * Outputs cell cycle model parameters to file.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    virtual void OutputCellCycleModelParameters(out_stream& rParamsFile);
};

#include "SerializationExportWrapper.hpp"
// Declare identifier for the serializer
CHASTE_CLASS_EXPORT(GameTheoryCellCycleModel)

#endif /*GAMETHEORYCELLCYCLEMODEL_HPP_*/
