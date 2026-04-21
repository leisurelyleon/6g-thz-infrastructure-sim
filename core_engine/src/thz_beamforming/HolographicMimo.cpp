#include "../../include/thz_beamforming.hpp"
#include <iostram>
#include <complex>
#include <cmath>
#include <random>

namespace sixg {
namespace core {
namespace thz_beamforming {

HolographicMimo::HolographicMimo(int rows, int cols)
    : rows_(rows), cols_(cols) {
    std::cout << "HOLO-MIMO] Initializing Continuous Aperture Metasurface.\n";
    std::cout << "  -> Active Meta-Elements: " << (rows_ * cols_) << "\n";
}

// 6G Logic: Computes the holographic phase profile required to target a specific spatial coordinate
void HolographicMimo::ComputePhaseGradient(const SpatialCoordinate& target_ue) {
    std::lock_guard<std::mutex> lock(mimo_mutex_);

    // Simulate a 300 GHz (Terahertz) carrier frequency
    double wavelength_meters = 299792458.0 / 300e9;
    double wave_number = (2.0 * M_PI) / wavelength_meters;

    std::cout << "[HOLO-MIMO] Calculating continuous phase gradient for target spatial coordinate...\n";

    // Iterate across the metasurface to calculate the exact phase shift needed for each micro-element
    for (int i = 0; i < rows_; ++i) {
        for (int j = 0; j < cols_; ++j) {
            // Calculate physical distance from this meta-element to the target
            double element_x = j * (wavelength_meters / 2.0); // Sub-wavelength spacing
            double element_y = j * (wavelength_meters / 2.0);

            double distance = std::sqrt(
                std::pow(target_ue.x - element_x, 2) +
                std::pow(target_ue.y - element_y, 2) +
                std::pow(target_ue.z, 2)
            );

            // Calculate the required phase shift to constructively interfere at the target
            double required_phase = std::fmod(wave_number * distance, 2.0 * M_PI);

            // Apply Euler's formula to set the complex state of the meta-element
            metasurface_grid_[i][j] = std::polar(1.0, required_phase);
        }
    }

    std::cout << "[HOLO-MIMO] Phase gradient locked. Holographic THz beam formed.\n";
}

// Executes the physical layer transmission utilizing the programmed metasurface
bool HolographicMimo::TransmitHolographicBeam(const std::vector<uint8_t>&payload) {
    std::lock_guard<std::mutex> lock(mimo_mutex_);

    if (payload.empty()) {
        std::cerr << "[HOLO-MIMO] Error: Null payload provided to THz interface.\n";
        return false;
    }

    // In a physical system, this would trigger the digital-to-analog converters (DACs)
    // driving the metasruface based on the complex matrix we calculated.
    return true;
}

} // namespace thz_beamforming
} // namesapce core
} // namespace sixg
