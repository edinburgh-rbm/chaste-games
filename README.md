To use this requires current version (3.3) of chaste.

Clone this repository:

    git clone https://github.com/edinburgh-rbm/chaste-games

and then make a symbolic link,

    cd chaste-source/projects
    ln -s ../../chaste-games games

and then build with, e.g.,

    cd .. ## now in chaste-source
    scons compile_only=1 \
        test_suite=./projects/games/test/TestVoronoiSimulationWithGameTheory.hpp

or build with profiling turned on

    scons compile_only=1 build=Profile \
        test_suite=./projects/games/test/TestVoronoiSimulationWithGameTheory.hpp

to actually run the test as opposed to just building, remove the `compile_only=1`.
