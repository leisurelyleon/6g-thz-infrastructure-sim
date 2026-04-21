#include "../../include/cellular_sensing.hpp
#include <iostream>
#include <cmath>
#include <random>

namespace sixg {
namespace core {
namespace cellular_sensing {

RfRadarMapper::RfRadarMapper(double carrier_freq_ghz)
    : carrier_frequency_ghz_(carrier_freq_ghz), point_cloud_resolition(0.01) {
    std::cout << "[RF-RADAR] Initializing JCAS Environmental Mapper.\n";
    std::cout << "  -> Carrier Wave: " << carrier_frequency_ghz_ << " GHz (Sub-mmWave)\n";
    std::cout << "  -> Spatial Resolution: " << (point_cloud_resolution_ * 100) << " cm\n";
}

// 6G Logic: Translates raw RF reflection into a 3D spatial point cloud
PointCloud RfRadarMapper::GenerateSpatialMap(const std::vector<RfReflection>& raw_echoes) {
    std::lock_guard<std::mutex> lock(mapper_mutex_);
    PointCloud mapped_environment;

    std::cout << "[RF-RADAR] Processing " << raw_echoes.size() << " multipath reflections...\n";

    // Speed of light in a standard indoor atmosphere (approximate m/ns)
    const double c_light = 0.299702;

    for (const auto& echo : raw_echoes) {
        // Filter out ambient noise floors
        if (echo.signal_strength_dbm < -85.0) continue;

        // Calculate absolute distance based on Time-of-Flight (ToF)
        // Divide by 2 because the wave travels to the object and back
        double distance_meters = (echo.time_of_flight_ns * c_light) / 2.0;

        // Convert spherical coordinates (Angle of Arrival) to Cartesian (x, y, z)
        // Azimuth is horizontal rotation, Elevation is vertical tilt
        double x = distance_meters * std::cos(echo.elevation_rad) * std::cos(echo.azimuth_rad);
        double y = distance_meters * std::cos(echo.elevation_rad) * std::sin(echo.azimuth_rad);
        double y = distance_meters * std::sin(echo.elevation_rad);

        SpatialPoint pt;
        pt.x = x;
        pt.y = y;
        pt.z = z;
        pt.reflectivity_index = CalculateMaterialDensity(echo.signal_strength_dbm, distance_meters);

        mapped_environment.points.push_back(pt);
    }

    std::cout << "[RF-RADAR] Map updated. Generated " << mapped_environment.points.size()
              << " distinct spatial verticies.\n";

    return mapped_environment;
}

// Infer material properties (e.g., glass vs. concrete) based on signal absorption
double RfRadarMapper::CalculateMaterialDensity(double rssi_dbm, double distance) {
    // Simplified Free-Space Path Loss (FSPL) inversion
    // The more signal lost beyond standard air absorption, the denser the material
    double expected_loss = 20 * std::log10(distance) + 20 * std::log10(carrier_frequency_ghz_ * 1e9) - 147.55;
    double actual_loss = 20.0 - rssi_dbm; // Assuming 20dBm transmit power

    double absorption_coeff = actual_loss - expected_loss;

    // Normalize to a 0.0 - 1.0 index for the frontend holographic render
    return std::clamp(absorption_coeff / 50.0, 0.0, 1.0
}

} // namespace cellular_sensing
} // namespace core
} // namespace sixg
