import torch
import torch.nn as nn

class ThzChannelPredictor(nn.Module):
    def __init__(self, seq_length=100, feature_dim=2):
        """
        Time-series forecasting model for extreme-frequency Terahertz channels.
        Input features: [ambient_noise_dbm, channel_utilization]
        """
        super(ThzChannelPredictor, self).__init__()
        print("[ML-FABRIC] Booting THz Channel State Predictor (CNN-LSTM Hybrid)")
        
        # 1D Convolution to extract rapid spatial fluctuations in the THz band
        self.spatial_conv = nn.Conv1d(in_channels=feature_dim, out_channels=16, kernel_size=3, padding=1)
        self.relu = nn.ReLU()
        
        # LSTM to maintain temporal memory of the RF environment
        self.temporal_lstm = nn.LSTM(input_size=16, hidden_size=32, batch_first=True)
        
        # Final fully connected layer to output the predicted jitter/delay
        self.regression_head = nn.Linear(32, 1)

    def forward(self, rf_telemetry_sequence):
        """
        rf_telemetry_sequence shape expects: (batch_size, seq_length, feature_dim)
        """
        # PyTorch Conv1d expects (batch, channels, seq_length), so we transpose
        x = rf_telemetry_sequence.transpose(1, 2)
        
        # Extract spatial features
        c_out = self.relu(self.spatial_conv(x))
        
        # Transpose back for the LSTM: (batch, seq_length, channels)
        c_out = c_out.transpose(1, 2)
        
        # Process temporal sequence
        lstm_out, (h_n, c_n) = self.temporal_lstm(c_out)
        
        # We only care about the prediction at the final time step
        last_time_step = lstm_out[:, -1, :]
        
        # Output predicted delay (normalized via Sigmoid)
        predicted_jitter = torch.sigmoid(self.regression_head(last_time_step))
        return predicted_jitter
