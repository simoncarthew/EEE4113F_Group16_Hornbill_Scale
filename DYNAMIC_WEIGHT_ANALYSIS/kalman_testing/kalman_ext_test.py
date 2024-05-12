import numpy as np
import matplotlib.pyplot as plt
import os
import glob

class ExtendedKalmanFilter1D:
    def __init__(self, initial_state, initial_covariance, process_noise_variance, measurement_noise_variance, damping_factor, spring_constant):
        self.x = initial_state  # Initial state [position, velocity]
        self.P = initial_covariance  # Initial covariance matrix
        self.Q = process_noise_variance  # Process noise covariance
        self.R = measurement_noise_variance  # Measurement noise covariance
        self.damping = damping_factor  # Damping factor of the spring
        self.spring_constant = spring_constant  # Spring constant

    def predict(self, dt):
        # State transition matrix F
        F = np.array([[1, dt],
                      [-self.spring_constant/self.damping*dt, 1-self.damping*dt]])
        
        # Predict state
        self.x = np.dot(F, self.x)
        
        # Predict covariance
        self.P = np.dot(np.dot(F, self.P), F.T) + self.Q

    def update(self, measurement):
        # Measurement matrix H
        H = np.array([[1, 0]])
        
        # Calculate Kalman gain
        S = np.dot(np.dot(H, self.P), H.T) + self.R
        K = np.dot(np.dot(self.P, H.T), np.linalg.inv(S))
        
        # Update state estimate
        self.x = self.x + np.dot(K, (measurement - np.dot(H, self.x)))
        
        # Update covariance matrix
        self.P = self.P - np.dot(np.dot(K, H), self.P)

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

def moving_average_filter(signal, window_size):
    filtered_signal = []
    for i in range(len(signal)):
        start_index = max(0, i - window_size + 1)
        end_index = i + 1
        filtered_signal.append(sum(signal[start_index:end_index]) / min(i + 1, window_size))
    return filtered_signal

# Example usage
if __name__ == "__main__":

    # Join the directory path with wildcard '*.txt' to get all text files
    search_path = os.path.join('/home/simon/OneDrive/University/Fourth_Year/EEE4113F/EEE4113F_Group16_Hornbill_Scale/DYNAMIC_WEIGHT_ANALYSIS/test_signals/real_sim', '*.txt')
    
    # Use glob to find all text files matching the pattern
    text_files = glob.glob(search_path)
    
    # Loop through each text file found
    for file_path in text_files:
        orig_sig,title = get_signal(file_path)

        # set kalman filter paramters
        initial_position = 0  # Initial position
        initial_velocity = 0  # Initial velocity
        dt = 1/10  # Time step
        num_steps = len(orig_sig)  # Number of time steps equal to the length of the signal
        process_noise_variance = 0.001  # Process noise variance
        measurement_noise_variance = 0.000001  # Measurement noise variance
        damping_factor = 1  # Damping factor of the spring
        spring_constant = 200  # Spring constant

        # Initialize Kalman filter
        initial_state = np.array([initial_position, initial_velocity])
        initial_covariance = np.eye(2)  # Identity matrix
        kf = ExtendedKalmanFilter1D(initial_state, initial_covariance, process_noise_variance, measurement_noise_variance, damping_factor, spring_constant)

        # Filter the signal
        filtered_signal = []
        for measurement in orig_sig:
            kf.predict(dt)
            kf.update(measurement)
            filtered_signal.append(kf.x[0])  # We are interested in the position estimate

        filtered_signal = moving_average_filter(orig_sig,8)

        plot_signal(file_path,orig_sig,filtered_signal,title)