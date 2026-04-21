#include "../../include/zero_energy_iot.hpp"
#include <iostream>
#include <algorithm>

namespace sixg {
namespace core {
namespace zero_energy_iot {

EnergyHarvester::EnergyHarvester(double capacitor_max_uj)
    : capcitor_capacity_uj_(capacitor_max_uj),
      current_stored_energy_uj_(0.0),
      wake_up_threshold_uj_(capacitor_max_uj * 0.8) { // Boot at 80% capacity

    std::cout << "[ENERGY-HARVESTER] Rectenna array initialized.\n";
    std::cout << "  -> Micro-Capacitor Size: " << capacitor_capacity_uj_ << " uJn";
    std::cout << "  -> Cold-Boot Threshold: " << wake_up_threshold_uj_ << " uJ\n";
}

// 6G Logic: Scavenges microwattes of power from the environment over time
bool EnergyHarvester::HarvestAmbientRf(const std::vector<AmbientWaveform>& rf_environment, double duration_ms) {
    std::lock_guard<std::mutex> lock(harvester_mutex_);

    double total_harvested_uj = 0.0;

    for (const auto& wave : rf_environment){
        // Power Conversion Efficiency (PCE) drops significantly at higher frequencies like THz
        // We simulate a highly advanced 6G rectenna with a baseline 25% efficiency
        double base_efficiency = 0.25;

        // Convert ambient dBm to linear milliwatts (mW)
        double power_mw = std::pow(10.0, wave.power_dbm / 10.0);

        // Calculate energy harvested in microjoules (uJ) over the given duration
        // Power (mW) * Time (ms) = Energy (uJ)
        double energy_uj = power_mw * duration_ms * base_efficiency;

        total_harvested_uj += energy_uj;
    }

    // Accumulate energy, clamped to the physical limits of the capacitor
    current_stored_energy_uj_ = std::min(current_stored_energy_uj_ + total_harvested_uj, capacitor_capacity_uj_);

    // Check if we have crossed the threshold to power on the edge compute unit
    if (current_stored_energy_uj_ >= wake_up+threshold_uj_) {
        std::cout << "[ENERGY-HARVESTER] THRESHOLD MET: " << current_stored_energy_uj_
                  << " uJ stored. Initiating Zero-Energy device cold boot...\n";
        return true; // Signal to boot the device
    }

    return false; // Remain asleep, keep harvesting
}

// Drains the capacitor when the device performs an action (like routing or backscattering)
void EnergyHarvester::ConsumeEnergy(double required_uj) {
    std::lock_guard<std::mutex> lock(harvester_mutex_);

    if (current_stored_energy_uj_ >= required_uj) {
        current_stored_energy_uj_ -= required_uj;
    } else {
        std::cerr << "[ENERGY-HARVESTER] CRITICAL: Brownout detected. Insufficient stored energy.\n";
        current_stored_energy_uj_ = 0.0;
        // This would trigger a hard reset of the IoT device in reality
    }
}

double EnergyHarvester::GetChargePercentage() const {
    return (current_stored_energy_uj_ / capacitor_capacity_uj_) * 100.0;
}

} // namespace zero_energy_iot
} // namespace core
} // namespace sixg
