import torch
import torch.nn as nn

class SemanticIntentEncoder(nn.Module):
    def __init__(self, input_dim=512, latent_dim=64):
        """
        Deep neural network to compress 6G payload metadata into semantic intent vectors.
        """
        super(SemanticIntentEncoder, self).__init__()
        print(f"[ML-FABRIC] Initializing Semantic Intent Encoder (Latent Dim: {latent_dim})")

        # A deep compression funnel using GeLU activations
        self.encoder_stack = nn.Sequential(
            nn.Linear(input_dim, 256),
            nn.LayerNorm(256),
            nn.GELU(),
            
            nn.Linear(256, 128),
            nn.LayerNorm(128),
            nn.GELU(),
            
            nn.Linear(128, latent_dim),
            nn.Sigmoid() # Normalize intent vectors between 0.0 and 1.0
        )

    def forward(self, payload_features):
        """
        Forward pass to generate the intent vector.
        """
        return self.encoder_stack(payload_features)

    def categorize_intent(self, latent_vector):
        """
        Maps the continuous latent vector back to the strict C++ IntentCategory enum.
        0: HOLOGRAPHIC_TELEPRESENCE, 1: ZERO_ENERGY_IOT, 
        2: SENSING_RADAR, 3: STANDARD_BROADBAND
        """
        # In a full simulation, this would use a Softmax classifier head.
        # For our mock architecture, we simulate categorization based on vector mean.
        vector_mean = torch.mean(latent_vector, dim=1).item()
        
        if vector_mean > 0.8:
            return 0 # HOLOGRAPHIC
        elif vector_mean < 0.2:
            return 1 # ZERO_ENERGY
        elif 0.4 < vector_mean < 0.6:
            return 2 # SENSING
        else:
            return 3 # STANDARD
