import os
import numpy as np
import matplotlib.pyplot as plt

#### UTILITY FUNCTIONS ####

def read_signal(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    # Extract data
    title = lines[0].split(': ')[1].strip()
    sampling_frequency = int(lines[1].split(': ')[1])
    if lines[2].split(":")[0].strip() == "Filter Type":
        filter_type = lines[2].split(': ')[1].strip()
        data = [float(line.strip()) for line in lines[4:]]
    else:
        offset = float(lines[3].split(': ')[1].strip())  # Convert to float
        scale = float(lines[4].split(': ')[1].strip())   # Convert to float
        data = [float(line.strip()) for line in lines[5:]]
        data = [(x - offset) / scale for x in data]
        filter_type = "original"

    signal = [title,sampling_frequency,filter_type,data]

    return signal

def plot_signals(signals,original_file_name,plot_dir):
    # create time axis
    samp_per = float(1/signals[0][1])
    no_samples = len(signals[0][3])
    t = np.linspace(0,samp_per*no_samples,no_samples)

    # find the original signal
    orig = None
    for signal in signals:
        if signal[2] == "Original":
            orig = signal

    fig, axes = plt.subplots(len(signals) - 1, 1, figsize=(10, 6), sharex=True)

    # Iterate over signals and plot each one on a separate subplot
    no_plots = 0
    for signal in signals:
        if signal[2] != "Original":
            ax = axes[no_plots]
            ax.plot(t, orig[3], label=orig[2])
            ax.plot(t, signal[3], label="Filtered")
            ax.set_title(signal[2])
            ax.set_xlabel('Time')
            ax.set_ylabel('Value')
            ax.grid(True)
            ax.legend()
            no_plots+=1

    plt.tight_layout()  # Adjust layout to prevent overlap
        
    # Save plot
    save_path = plot_dir + "/" + original_file_name + "_filtered.png"
    plt.savefig(save_path)
    plt.close()

#### SIMULATION FIGURING ####

def filtering():
    # directory of ssignal files
    directory = "simulation/outputs"

    # define filter types
    filter_type_ext = ["_orig","_mov","_exp_mov"]
    orig_sig_names = []
    signals = []

    # get the signal files
    file_paths = []
    for filename in os.listdir(directory):
        file_path = os.path.join(directory, filename)
        if os.path.isfile(file_path):
            if any(ft in file_path for ft in filter_type_ext):
                file_paths.append(file_path)
                signal = read_signal(file_path)
                if signal[0] not in orig_sig_names:
                    orig_sig_names.append(signal[0])
                    signals.append([])
                signals[orig_sig_names.index(signal[0])].append(signal)
    
    # plot the signals
    for i,sig_name in enumerate(orig_sig_names):
        plot_signals(signals[i],sig_name,"simulation/results")

def estimation():
    # directory of ssignal files
    est_path = "simulation/outputs/estimation.txt"
    sig_path = "simulation/outputs/est_sig.txt"

    # read in estimation values
    with open(est_path, 'r') as file:
        lines = file.readlines()  # Read all lines from the file
    data = [float(line.strip()) for line in lines if line.strip()]

    # read in estimation signal
    signal = read_signal(sig_path)

if __name__ == "__main__":
    filtering()
    estimation()