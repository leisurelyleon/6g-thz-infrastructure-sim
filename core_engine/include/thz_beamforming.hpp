#pragma once

#include <vector>
#include <string>
#include <complex>
#include <mutex>
#include <optional>

namespace sixg {
namespace core {
namespace thz_beamforming {

// Data Structures
struct SpatialCoordinate {
    double x, y, z;
};

struct DataStream {
    std::string id;
    std::string hash;
};

struct VortexState {
    int topological_charge;
    std::string payload_hash;
};

struct OamWavefront {
    std::vector<VortexState> active_states;
    bool is_multiplexed = false;
};

// Core Classes
class HolographicMimo {
public:
    HolographicMimo(int rows, int cols);

    void ComputePhaseGradient(const SpatialCoordinate& target_ue);
    bool TransmitHolographicBeam(const std::vector<uint8_t>& payload);

private:
    std::mutex mimo_mutex_;
    int rows_;
    int cols_;
    std::vector<std::vector<std::complex<double>>> metasurface_grid_;
};

class OrbitalAngularMomentum {
public:
    OrbitalAngularMomentum();

    OamWavefront MultiplexStreams(const std::vector<DataStream>& streams);
    std::optional<DataStream> DemultiplexMode(const OamWavefront& incoming_wave, int target_charge);

private:
    std::mutex oam_mutex_;
    int max_topological_charge_;
};

} // namespace thz_beamforming
} // namespace core
} // namespace sixg
