import torch
import json
import os
from datetime import datetime

def export_weights_for_cpp_engine(model, output_filename="sixg_neural_weights.json"):
    """
    Strips weights from a PyTorch model and formats them for the C++ core engine.
    """
    model_name = model.__class__.__name__
    print(f"\n[EXPORTER] Initiating hot-swap export for {model_name}...")
    
    flattened_weights = []
    
    # Iterate through all trainable parameters in the model
    for name, param in model.named_parameters():
        if param.requires_grad:
            # Flatten the multi-dimensional tensors into a standard 1D list of floats
            flattened_weights.extend(param.data.flatten().tolist())
            
    # Package into a structured payload matching the C++ parsing logic
    export_payload = {
        "metadata": {
            "model_architecture": model_name,
            "export_timestamp": datetime.utcnow().isoformat() + "Z",
            "total_parameters": len(flattened_weights)
        },
        "weights": flattened_weights
    }
    
    # Ensure export directory exists
    export_dir = os.path.join(os.path.dirname(__file__), "..", "exports")
    os.makedirs(export_dir, exist_ok=True)
    
    filepath = os.path.join(export_dir, output_filename)
    
    with open(filepath, 'w') as f:
        json.dump(export_payload, f, indent=2)
        
    print(f"[EXPORTER] SUCCESS: Packaged {len(flattened_weights)} parameters.")
    print(f"[EXPORTER] File saved to: {filepath}")
    print(f"[EXPORTER] Ready for C++ NeuralScheduler ingestion.")

# Quick test execution block if this file is run directly
if __name__ == "__main__":
    from channel_predictor import ThzChannelPredictor
    
    # Spin up a dummy model and export it to verify the pipeline
    dummy_model = ThzChannelPredictor()
    export_weights_for_cpp_engine(dummy_model, "test_thz_weights.json")
