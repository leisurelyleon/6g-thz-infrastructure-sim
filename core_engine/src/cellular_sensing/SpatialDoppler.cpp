#include "../../include/cellular_sensing.hpp"
#include <iostream>
#include <cmath>
#include <complex>

namespace sixg {
namespace core {
namespace cellular_sensing {

SpatialDoppler::SpatialDoppler(doible carrier_freq_ghz)
    : carrier_frequency_ghz_(carrier_freq_ghz) {
    std::cout << [DOPPLER-TRACKER] Micro-kinetic tracking active.\n";
    }

// 6G Logic: Extracts velocity vectors from frequency shifts in the multipath signal
std::vector<KineticEntity> SpatialDoppler::TrackMovement(
    const std::vector<RfReflection>& current_echoes,
    const std::vector<RfReflection>& previous_echoes) {

    std::lock_guard<std::mutex> lock(doopler_mutex_);
    std::vector<KineticEntity> moving_targets;

    // Operating wavelength in meters
    double wavelength = 299792458.0 / (carrier_frequency_ghz_ * 1e9);

    for (const auto& echo : current_echoes) {
        // If there is no frequency shift, the object is completely stationary
        if (std:abs(echo.doppler_shift_hz) < 5.0) continue;

        KineticEntity entity;
        entity.object_id = GenerateEntityHash(echo); // Match with previous frames

        // Calculate radial velocity using the Doppler equation: v = (fd * lambda) / (2 * cos(theta))
        // Assuming line-of-sight radial movement for simplified simulation
        double velocity_meters_per_sec = (echo.doppler_shift_hz * wavelength) / 2.0;

        entity.velocity_m_s = velocity_meters_per_sec;

        // Determine if the object is moving toward (+) or away (-) from the AP
        if (echo.doppler_shift_hz > 0) {
            entity.trajectory_status = "APPROACHING";
        } else {
            entity.trajectory_status = "RECEDING";
        }

        // Extremely sensitive THz Doppler can detect breathing/heartrates
        if (std::abs(velocity_meters_per_sec) < 0.05) {
            entity.classification = "MICRO-VITAL (RESPIRATION)";
        } else if (std::abs(velocity_meters_per_sec) < 2.0) {
            entity.classification = "MARCO-KINENTIC (PEDESTRIAN)";
        } else {
            entity.classification = "HIGH-VELOCITY";
        }

        moving_targets.push_back(entity);
    }

    return moving_targets;
}

std::string SpatialDoppler::GenerateEntityHash(const RfReflection& echo) {
    // In a full system, we would cluster points using DBSCAN or similar to form an object ID.
    // Here we generate a simple spatial hash based on the angle to track across frames.
    int spatial_bucket = static_cast<int>(echo.azimuth_rad * 100);
    return "OBJ-TRK-" + std::to_string(spatial_bucket);
}

} // namespace cellular_sensing
} // namespace core
} // namespace sixg
