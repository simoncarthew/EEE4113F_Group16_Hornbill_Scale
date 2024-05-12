import numpy as np
import matplotlib.pyplot as plt
import os
import glob

class KalmanFilter1D:
    def __init__(self, initial_state, initial_covariance, process_noise_variance, measurement_noise_variance):
        self.x = initial_state
        self.P = initial_covariance
        self.Q = process_noise_variance
        self.R = measurement_noise_variance

    def predict(self):
        self.x = self.x
        self.P = self.P + self.Q

    def update(self, measurement):
        K = self.P / (self.P + self.R)
        self.x = self.x + K * (measurement - self.x)
        self.P = (1 - K) * self.P

def smooth_weight_signal(weight_signal, initial_state, initial_covariance, process_noise_variance, measurement_noise_variance):
    kf = KalmanFilter1D(initial_state, initial_covariance, process_noise_variance, measurement_noise_variance)
    smoothed_signal = []

    for measurement in weight_signal:
        kf.predict()
        kf.update(measurement)
        smoothed_signal.append(kf.x)

    return smoothed_signal


def get_signal(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    # Extract data
    title = lines[0].split(': ')[1].strip()
    sampling_frequency = float(lines[1].split(': ')[1])
    offset = float(lines[3].split(': ')[1].strip())  # Convert to float
    scale = float(lines[4].split(': ')[1].strip())   # Convert to float
    data = [float(line.strip()) for line in lines[5:]]

    # scale and offset
    data = [(x - offset) / scale for x in data]

    return data, title

def plot_signal(file_path,orig_sig,smoothed_sig,title):
     # Plot the original and smoothed signals
    plt.figure(figsize=(10, 6))
    plt.plot(orig_sig, label='Original Signal', color='blue')
    plt.plot(smoothed_sig, label='Smoothed Signal', color='red')
    plt.title(title)
    plt.xlabel('Time')
    plt.ylabel('Weight')
    plt.legend()
    
    # Save the figure
    dir = "/home/simon/OneDrive/University/Fourth_Year/EEE4113F/EEE4113F_Group16_Hornbill_Scale/DYNAMIC_WEIGHT_ANALYSIS/results"
    plt.savefig(dir + '/' + os.path.splitext(os.path.basename(file_path))[0] + '.png')
    
    # Show the plot (optional)
    # plt.show()


# Example usage
if __name__ == "__main__":
    # Join the directory path with wildcard '*.txt' to get all text files
    search_path = os.path.join('/home/simon/OneDrive/University/Fourth_Year/EEE4113F/EEE4113F_Group16_Hornbill_Scale/DYNAMIC_WEIGHT_ANALYSIS/test_signals/real_sim', '*.txt')
    
    # Use glob to find all text files matching the pattern
    text_files = glob.glob(search_path)
    
    # Loop through each text file found
    for file_path in text_files:
        orig_sig,title = get_signal(file_path)

        # Define Kalman filter parameters
        initial_state = orig_sig[0]
        initial_covariance = 1  # You may need to tune this based on your specific application
        process_noise_variance = 0.000001  # Assuming no process noise for constant state
        measurement_noise_variance = 0.00001  # Assuming measurement noise, obtained from sensor data sheets

        # Smooth the weight signal
        smoothed_signal = smooth_weight_signal(orig_sig, initial_state, initial_covariance, process_noise_variance, measurement_noise_variance)

        plot_signal(file_path,orig_sig,smoothed_signal,title)