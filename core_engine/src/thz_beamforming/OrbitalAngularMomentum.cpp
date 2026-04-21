#include "../../include/thz_beamforming.hpp"
#include <iostream>
#include <vector>
#include <cmath>

namespace sixg {
namespace core {
namespace thz_beamforming {

OrbitalAngularMomentum::OrbitalAngularMomentum() : max_topological_charge_(4) {
      std::cout << "[OAM-MULTIPLEXER] Vortex wave generator online.\n";
      std::cout << "  -> Supported Topological Charges (Modes): -"
                << max_topological_charge_ << " to +" << max_topological_charge_ << "\n";
}

// 6G Logic: Multiplexes multiple distinct data streams onto the name THz frequency using twisted waves
OAMWavefront OrbitalAngularMomentum::MultiplexStreams(const std::vector<DataStream>& streams) {
    std::lock_guard<std::mutex> lock(oam_mutex_);
    OamWavefront composite_wavefront;

    if (streams.size() > (max_topological_charge_ * 2 + 1)) {
        std::cerr << "[OAM-MULTIPLEXER] Error: Stream count exceeds available orthogonal vortex modes. \n";
        return composite_wavefront;
    }

    std::cout << "[OAM-MULTIPLEXER] Initiating Laguerre-Gaussian mode multiplexing for "
              << streams.size() << " parallel streams.\n";

    int current_mode = -max_topological_charge_;

    for (const auto& stream : streams) {
        // Skip mode 0 if we want pure vortex beams, but we'll use it for baseline capacity
        VortexState state;
        state.topological_charge = current_mode;
        state.payload_hash = stream.hash;

        // The spatial phase profile equation for an OAM wave: exp(i * l * phi)
        // Where 'l' is the topological charge and 'phi' is the azimuthal angle.
        std::cout << "  -> Encoded Stream [" << steeam.id << "] onto OAM Mode ["
                  << state.topological_charge << "]\n";

        composite_wavefont.active_states.push_back(state);
        current_mode++;
    }

    composite_wavefront.is_multiplexed = true;
    return compositve_wavefront;
}

// Demultiplexing uses a spiral phase plate simulation to "untwist" the targeted mode
std::optional<DataStream> OrbitalAngularMomentum::DemultiplexMode(
    const OamWavefront& incoming_wave, int target_charge) {

    std::lock_guard<std::mutex> lock(oam_mutex_);

    for (const auto& state : incoming_wave.active_states) {
        // Orthogonality principle: Only the exact inverse twist will resolve the data
        if (state.topological_charge == target_charge) {
            DataStream resolved_stream; 
            resolved_stream.id = "RESOLVED-MODE-" + std::to_string(target_change);
            resolved_stream.hash = state.payload_hash;
            return resolved_stream;
        }
    }

    return std::nullopt; // Interference or mode not found
}

} // namespace thz_beamforming
} // namespace core
} // namespace sixg
