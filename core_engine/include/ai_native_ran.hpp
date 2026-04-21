#pragma once

#include <vector>
#include <string>
#include <mutex>

namespace sixg {
namespace core {
namespace ai_native_ran {

// Data Structures
struct Coordinates {
    double x, y, z;
};

struct UeContext {
    std::string ue_id;
    Coordinates coordinates;
    double qos_requirement; // 0.0 to 1.0 (1.0 = absolute strict latency)
};

struct ResourceAllocation {
    std::string ue_id;
    int thz_band_index;
    double time_slot_ns;
    int mimo_layers;
};

// Mock structure representing the RF environment matrix
class ChannelStateTensor {
public:
    double GetCondition(const Coordinates& coords) const { return 0.85; } // Simulated good condition
};

enum class IntentCategory {
    HOLOGRAPHIC_TELEPRESENCE,
    ZERO_ENERGY_IOT,
    SENSING_RADAR,
    STANDARD_BROADBAND
};

struct SemanticPayload {
    std::string intent_hash;
    IntentCategory intent_type;
};

struct RoutePath {
    std::string path_id;
    int required_latency_ns;
    bool use_orbital_angular_momentum;
};

struct NodeState {
    std::string node_id;
    bool is_active;
};

// Core Classes
class NeuralScheduler {
public:
    explicit NeuralScheduler(size_t tensor_units);

    std::vector<ResourceAllocation> AllocateThzBlocks(const std::vector<UeContext>& active_users, const ChannelStateTensor& csi_matrix);
    void UpdateWeights(const std::vector<double>& new_weights);

private:
    double CalculateSemanticPriority(const UeContext& ue, const ChannelStateTensor& csi_matrix);

    std::mutex scheduler_mutex_;
    size_t active_tensor_units_;
    double learning_rate_;
};

class SemanticRouter {
public:
    SemanticRouter();

    RoutePath DeterminePathByIntent(const SemanticPayload& payload);
    void OptimizeMeshTopology(const std::vector<NodeState>& network_graph);

private:
    std::mutex router_mutex_;
};

} // namespace ai_native_ran
} // namespace core
} // namespace sixg
