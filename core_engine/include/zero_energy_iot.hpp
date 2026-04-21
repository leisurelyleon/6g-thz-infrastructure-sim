#pragma once

#include <vector>
#include <string>
#include <mutex>

namespace sixg {
namespace core {
namespace zero_energy_iot {

// Data Structures
struct AmbientWaveform {
    double frequency_ghz;
    double power_dbm;
    double phase_offset_rad;
};

struct BackscatterSymbol {
    double target_carrier_frequency;
    double reflection_coefficient;
    double phase_shift_rad;
};

enum class ImpedanceState {
    ABSORPTIVE,
    REFLECTIVE
};

// Core Classes
class BackscatterController {
public:
    explicit BackscatterController(const std::string& device_id);

    std::vector<BackscatterSymbol> ModulatePayload(const std::vector<uint8_t>& raw_data, const AmbientWaveform& ambient_carrier);

private:
    void SwitchImpedance(ImpedanceState new_state);

    std::mutex controller_mutex_;
    std::string device_id_;
    ImpedanceState current_impedance_state_;
};

class EnergyHarvester {
public:
    explicit EnergyHarvester(double capacitor_max_uj);

    bool HarvestAmbientRf(const std::vector<AmbientWaveform>& rf_environment, double duration_ms);
    void ConsumeEnergy(double required_uj);
    double GetChargePercentage() const;

private:
    std::mutex harvester_mutex_;
    double capacitor_capacity_uj_;
    double current_stored_energy_uj_;
    double wake_up_threshold_uj_;
};

} // namespace zero_energy_iot
} // namespace core
} // namespace sixg
