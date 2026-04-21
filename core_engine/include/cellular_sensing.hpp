#pragma once

#include <vector>
#include <string>
#include <mutex>

namespace sixg {
namespace core {
namespace cellular_sensing {

// Data Structures
struct RfReflection {
    double time_of_flight_ns;
    double azimuth_rad;
    double elevation_rad;
    double signal_strength_dbm;
    double doppler_shift_hz;
};

struct SpatialPoint {
    double x, y, z;
    double reflectivity_index; // 0.0 to 1.0
};

struct PointCloud {
    std::vector<SpatialPoint> points;
};

struct KineticEntity {
    std::string object_id;
    double velocity_m_s;
    std::string trajectory_status;
    std::string classification;
};

// Core Classes
class RfRadarMapper {
public:
    explicit RfRadarMapper(double carrier_freq_ghz);

    PointCloud GenerateSpatialMap(const std::vector<RfReflection>& raw_echoes);

private:
    double CalculateMaterialDensity(double rssi_dbm, double distance);

    std::mutex mapper_mutex_;
    double carrier_frequency_ghz_;
    double point_cloud_resolution_;
};

class SpatialDoppler {
public:
    explicit SpatialDoppler(double carrier_freq_ghz);

    std::vector<KineticEntity> TrackMovement(const std::vector<RfReflection>& current_echoes, const std::vector<RfReflection>& previous_echoes);

private:
    std::string GenerateEntityHash(const RfReflection& echo);

    std::mutex doppler_mutex_;
    double carrier_frequency_ghz_;
};

} // namespace cellular_sensing
} // namespace core
} // namespace sixg
