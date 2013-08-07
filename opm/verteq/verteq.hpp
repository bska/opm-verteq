#ifndef OPM_VERTEQ_VERTEQ_HPP_INCLUDED
#define OPM_VERTEQ_VERTEQ_HPP_INCLUDED

// Copyright (C) 2013 Uni Research AS
// This file is licensed under the GNU General Public License v3.0

#include <string>
#include <vector>

#ifndef OPM_VERTEQ_VISIBILITY_HPP_INCLUDED
#include <opm/verteq/visibility.hpp>
#endif /* OPM_VERTEQ_VISIBILITY_HPP_INCLUDED */

// forward declaration
struct UnstructuredGrid;
struct Wells;

namespace Opm {

struct IncompPropertiesInterface;
struct TwophaseState;

namespace parameter {
struct ParameterGroup;
} // namespace parameter

/**
 * @brief Vertical Equilibrium Upscaling
 *
 * This class acts like a wrapper around a three-dimensional setup of
 * grid, fluid and rock properties and provide a view of a two-
 * dimensional upscaling based on the vertical equilibrium model.
 *
 * The objects provided by the upscaling may be passed into the
 * simulator as if read from a deck, and the results translated back
 * into a full model afterwards.
 *
 * Notice that the following constraints apply on the grid:
 *
 * * It must be structured, i.e. every element is (i,j,k)-addressable
 * * No degenerate faces (on the top), i.e. no crossing grid axes
 * * It must have no horizontal faults
 */
class OPM_VERTEQ_PUBLIC VertEq
{
protected:
	// you are not supposed to call the constructor yourself; instead use
	// the static create function provided
	VertEq () {}
public:
	/**
	 * @brief Pseudo-constructor of VertEqUpscaling objects
	 *
	 * You own the object that is returned, and is responsible for
	 * calling delete on the pointer.
	 *
	 * @param title Name of the case, gotten from getTITLE().name(); this
	 *              may be used to set grid-specific properties.
	 * @param args Parameters
	 * @param fullGrid Grid obtained elsewhere. This object is not
	 *        adopted, but is assumed to be live over the lifetime
	 *        of the upscaling.
	 * @param fullWells Wells list with indices in the three-dimensional
	 *                  grid. This is typically initialized from a deck.
	 * @param fullSrc List of volumetric source term for each element
	 *                in the grid.
	 * @param gravity Gravity vector (three-dimensional); must contain
	 *                three elements, whereas the last is for depth.
	 *                Usually this is {0., 0., Opm::unit::gravity}.
	 *
	 * @return A new instance implementing this interface.
	 */
	static VertEq* create (const std::string& title,
	                       const Opm::parameter::ParameterGroup& args,
	                       const UnstructuredGrid& fullGrid,
	                       const IncompPropertiesInterface& fullProps,
	                       const Wells* fullWells,
	                       const std::vector<double>& fullSrc,
	                       const double* gravity);

	// virtual destructor, actual functionality relayed to real impl.
	virtual ~VertEq () {}

	/**
	 * @brief Accessor method for the upscaled grid
	 *
	 * This method is inexpensive; the grid is not constructed upon
	 * every invocation.
	 *
	 * @return Grid that may be passed to other components in the
	 *         simulator. You do NOT own this object!
	 *
	 */
	virtual const UnstructuredGrid& grid () = 0;

	/**
	 * @brief Accessor method for the list of upscaled wells.
	 *
	 * This method is inexpensive; the listis not constructed upon
	 * every invocation.
	 *
	 * @return List of wells that may be passed to other components
	 *         in the simulator. You do NOT own this object!
	 */
	virtual const Wells* wells () = 0;

	/**
	 * @brief props Accessor method for the upscaled "fluid" objects
	 *
	 * This method is inexpensive; upscaling is not done upon every
	 * invocation.
	 *
	 * @return Rock and hydrological properties that may be passed
	 *         other components in the simulator. You do NOT own this
	 *         object!
	 */
	virtual const IncompPropertiesInterface& props () = 0;

	/**
	 * @brief Accessor method for the upscaled source terms.
	 *
	 * This method is inexpensive; upscaling is not done upon every
	 * invocation.
	 *
	 * @return A volumetric flux in the metric units (cubic meters/second)
	 *        for each grid block in the upscaled grid.
	 */
	virtual const std::vector<double>& src () = 0;

	/**
	 * Create an upscaled view of the domain state.
	 *
	 * This must be done in a separate method since the state is not
	 * available at construction of the simulator. Prefer to use the
	 * VertEqState object instead of calling this method yourself.
	 *
	 * @param fineScale    Initialized state for the fine-scale domain.
	 *
	 * @param coarseScale  Object that will receive the corresponding
	 *                     state for the upscaled domain. This should
	 *                     be a constructed object but where the init
	 *                     method has not yet been called.
	 *
	 * @see VertEqState
	 */
	virtual void upscale (const TwophaseState& fineScale,
	                      TwophaseState& coarseScale) = 0;

	/**
	 * Update the internal variables based on the state.
	 *
	 * Logically, the VE model is bound to a certain state because it
	 * needs to know where the plume has been in order to get the
	 * aggregate saturation correct.
	 *
	 * Prefer to use the VertEqState object instead of calling this
	 * method yourself.
	 *
	 * @param coarseScale State of the upscaled domain. This should
	 *                    have been initialized with the upscale
	 *                    method.
	 *
	 * @see VertEqState, VertEq::upscale
	 */
	virtual void notify(const TwophaseState& coarseScale) = 0;
};

} // namespace Opm

#endif /* OPM_VERTEQ_VERTEQ_HPP_INCLUDED */
