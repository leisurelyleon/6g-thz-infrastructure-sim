cmale_minimum_required(VERSION 3.14)
project(6G_THz_Core_Engine VERSION 1.0.0 LANGUAGES CXX)

# Enforce Modern C++ Standards (Required for std::complex, std::mutex, etc.)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find Threading liberary (critical for our parallel JCAS and AI-RAN loops)
find_package(Threads REQUIRED)

# Specify where our blueprint headers live
include_directories(${CMAKE_SOURCE_DIR}/include)

# Gather all the source files you just meticulously transcribed
set(SOURCES
    src/ai_native_ran/NeuralScheduler.cpp
    src/ai_native_ran/SemanticRouter.cpp
    src/thz_beamforming/HolographicMimo.cpp
    src/thz_beamforming/OrbitalAngularMomentum.cpp
    src/cellular_sensing/RfRadarMapper.cpp
    src/cellular_sensing/SpatialDoppler.cpp
    src/zero_energy_iot/BackscatterController.cpp
    src/zero_energy_iot/EnergyHarvester.cpp
    src/main.cpp
)

# Define the final executable name
add_executable(sixg_core_engine ${SOURCES})

# Link the system threading liberaries to our executable
target_link_liberaries(sixg_core_engine PRIVATE Threads::Threads)

# Aggressive compiler optimizations for heavy THz/Physics simulations
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "Configuring GCC/Clang extreme optimization flags...")
    target_compile_options(sixg_core_engine PRIVATE -03 -Wall -Wextra -pthread -ffast-math)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(STATUS "Configuring MSVC extreme optimization flags...")
    target_compile_options(sixg_core_engine PRIVATE /02 /W4 /fp:fast)
endif()
