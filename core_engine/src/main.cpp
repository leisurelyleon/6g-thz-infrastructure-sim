#include "../include/ai_native_ran.hpp"
#include "../include/thz_beamforming.hpp"
#include "../include/cellular_sensing.hpp"
#include "../include/zero_energy_iot.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <memory>
#include <random>

using namespace sixg::core;

// Global atomic flag for safe thread termination
std::atomic<bool> engine_running{true};

// Simulated Websocket Dispatcher for the React/Three.js Dashboard
void DispatchTelemetryToFabric(const std::string& telemetry_data) {
    // In a production build, this pipes to ZeroMQ or WebSockets
    std::cout << "[TELEMETRY-TX] " << telemetry_data << "\n";
}

// Master 6G Sumulation Loop
void Run6GCoreSimulation(
    std::shared_ptr<ai_native_ran::NeuralScheduler> scheduler,
    std::shared_ptr<ai_native_ran::SemanticRouter> router,
    std::shared_ptr<thz_beamforming::HolographicMimo> holo_mimo,
    std::shared_ptr<thz_beamforming::OrbitalAngularMomentum> oam_multiplexer,
    std::shared_ptr<cellular_sensing::RfRadarMapper> radar_mapper,
    std::shared_ptr<cellular_sensing::SpatialDoppler> doppler,
    std::shared_ptr<zero_energy_iot::EnergyHarvester> iot_harvester) {

    std::cout << "\n[SYSTEM] 6G Terahertz Core Engine Online.\n";
    std::cout << "[SYSTEM] Engaging Joint Communication and Sensing (JCAS) Loop...\n\n";

    uint64_t nanosecond_tick = 0;
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Simulating environmental variables
    std::uniform_real_distribution<> rf_noise_dist(-100.0, -70.0);
    std::uniform_real_distribution<> target_dist(1.0, 15.0);

    while (engine_running) {
        // --- STEP 1: SENSING LAYER (JCAS) ---
        // Generate simulated multipath echoes for the radar mapper
        std::vector<cellular_sensing::RfReflection> current_echoes = {
            {target_dist(gen), 0.5, 0.2, rf_noise_dist(gen) + 30.0, 15.0}, // Moving object
            {target_dist(gen), -0.5, 0.1, rf_noise_dist(gen) + 20.0, 0.0}  // Static wall
        };
        
        auto point_cloud = radar_mapper->GenerateSpatialMap(current_echoes);
        auto kinetic_entities = doppler->TrackMovement(current_echoes, current_echoes); // Simplified 1-frame mock

        // --- STEP 2: ZERO-ENERGY IOT MESH ---
        // Simulate ambient wave generation and harvesting
        std::vector<zero_energy_iot::AmbientWaveform> ambient_waves = {
            {300.0, rf_noise_dist(gen) + 40.0, 0.0} // 300 GHz stray wave
        };
        
        bool iot_boot_triggered = iot_harvester->HarvestAmbientRf(ambient_waves, 1.0); // 1ms duration
        if (iot_boot_triggered) {
            iot_harvester->ConsumeEnergy(2.5); // Micro-burst consumption
        }

        // --- STEP 3: AI-NATIVE RAN & ROUTING ---
        // Evaluate simulated user equipment (UE)
        std::vector<ai_native_ran::UeContext> active_ues = {
            {"UE-HOLO-1", {target_dist(gen), 2.0, 1.5}, 0.99} // High QoS required
        };
        ai_native_ran::ChannelStateTensor csi_matrix; // Mock empty matrix
        
        auto allocations = scheduler->AllocateThzBlocks(active_ues, csi_matrix);

        // Analyze intent of incoming payload
        ai_native_ran::SemanticPayload holographic_payload{"HOLO-VIDEO-STREAM-HASH", ai_native_ran::IntentCategory::HOLOGRAPHIC_TELEPRESENCE};
        auto optimized_path = router->DeterminePathByIntent(holographic_payload);

        // --- STEP 4: THz BEAMFORMING (PHYSICAL LAYER) ---
        if (optimized_path.use_orbital_angular_momentum) {
            std::vector<thz_beamforming::DataStream> streams = { {"STREAM-1", holographic_payload.intent_hash} };
            auto vortex_wave = oam_multiplexer->MultiplexStreams(streams);
        }
        
        // Dynamically compute the metasurface gradient for the UE
        holo_mimo->ComputePhaseGradient({active_ues[0].coordinates.x, active_ues[0].coordinates.y, active_ues[0].coordinates.z});

        // --- STEP 5: DASHBOARD TELEMETRY ---
        if (nanosecond_tick % 50 == 0) { // Throttle telemetry to dashboard
            std::string payload = "{\"tick\": " + std::to_string(nanosecond_tick) + 
                                  ", \"tracked_entities\": " + std::to_string(kinetic_entities.size()) +
                                  ", \"iot_charge_pct\": " + std::to_string(iot_harvester->GetChargePercentage()) + "}";
            DispatchTelemetryToFabric(payload);
        }

        nanosecond_tick++;
        
        // 6G operates on micro/nanosecond frames, simulating rapid loop
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    std::cout << "=======================================================\n";
    std::cout << "  6G TERAHERTZ & AI-NATIVE SENSING INFRASTRUCTURE \n";
    std::cout << "=======================================================\n";

    // Booting the massive parallel subsystems
    auto scheduler = std::make_shared<ai_native_ran::NeuralScheduler>(1024);
    auto router = std::make_shared<ai_native_ran::SemanticRouter>();
    auto holo_mimo = std::make_shared<thz_beamforming::HolographicMimo>(256, 256); // 65,536 element metasurface
    auto oam_multiplexer = std::make_shared<thz_beamforming::OrbitalAngularMomentum>();
    auto radar_mapper = std::make_shared<cellular_sensing::RfRadarMapper>(300.0); // 300 GHz radar
    auto doppler = std::make_shared<cellular_sensing::SpatialDoppler>(300.0);
    auto iot_harvester = std::make_shared<zero_energy_iot::EnergyHarvester>(15.0); // 15 uJ micro-capacitor

    std::cout << "[SYSTEM] All quantum and THz subsystems initialized. Booting hyper-visor thread...\n";

    // Launch the 6G simulation on a dedicated high-priority thread
    std::thread sim_thread(Run6GCoreSimulation, scheduler, router, holo_mimo, oam_multiplexer, radar_mapper, doppler, iot_harvester);

    // Keep the main thread alive, listen for manual shutdown
    std::cout << "[SYSTEM] Press Enter to execute emergency core shutdown...\n";
    std::cin.get();

    // Graceful spin-down
    std::cout << "[SYSTEM] Core shutdown signal received. Collapsing THz fields...\n";
    engine_running = false;
    
    if (sim_thread.joinable()) {
        sim_thread.join();
    }

    std::cout << "[SYSTEM] 6G Core offline. Safe to disconnect.\n";
    return 0;
}
