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

def plot_filters(signals,original_file_name,plot_dir, no_sig):
    # create time axis
    samp_per = float(1/signals[0][1])
    no_samples = len(signals[0][3])
    t = np.linspace(0,samp_per*no_samples,no_samples)

    # find the original signal
    orig = None
    for signal in signals:
        if signal[2] == "Original":
            orig = signal

    fig, axes = plt.subplots(len(signals) - 1, 1, figsize=(10, 10), sharex=True)
    fig.suptitle('FILTERING SIMULATION', fontsize=16, fontweight='bold')

    # Iterate over signals and plot each one on a separate subplot
    no_plots = 0
    for signal in signals:
        if signal[2] != "Original":
            ax = axes[no_plots]
            ax.plot(t, orig[3], label=orig[2])
            ax.plot(t, signal[3], label="Filtered")
            ax.set_title(signal[2],fontweight='bold')
            ax.set_xlabel('Time (s)')
            ax.set_ylabel('Weight (g)')
            # ax.grid(True)
            ax.legend()
            no_plots+=1

    plt.tight_layout()  # Adjust layout to prevent overlap
        
    # Save plot
    save_path = plot_dir + "/" + original_file_name + "_filtered.png"
    plt.savefig(save_path)
    plt.close()

#### SIMULATION FIGURING ####

def conversion():
    # paths to the two signals
    raw_path = "simulation/outputs/conv_raw.txt"
    proc_path = "simulation/outputs/conversion_val.txt"
    bird_per_path = "simulation/outputs/bird_per_int.txt"

    # read in bird period 
    with open(bird_per_path, 'r') as file:
        lines = file.readlines()
        if len(lines) >= 2:
            start = int(lines[0].strip())
            end = int(lines[1].strip())

    # read in the signals
    raw_sig = read_signal(raw_path)
    proc_sig = read_signal(proc_path)

    # create the time axis
    samp_per = float(1/raw_sig[1])
    no_samples = len(raw_sig[3])
    t = np.linspace(0,samp_per*no_samples,no_samples)

    # plot the two signals
    fig, axes = plt.subplots(1, 2, figsize=(10, 6), sharex=True)
    fig.suptitle('WEIGHT CONVERSION AND BIRD PRESENT SIMULATION', fontsize=16, fontweight='bold')

    # plot the raw signal
    ax = axes[0]
    ax.plot(t, raw_sig[3],color='red')
    ax.set_title("Raw Signal",fontweight='bold')
    ax.set_xlabel('Time (s)')
    ax.set_ylabel('Value')
    # ax.grid(True)
    ax.legend()

    # plot the processed signal
    ax = axes[1]
    ax.plot(t, proc_sig[3])
    ax.set_title("Processed Weight Signal",fontweight='bold')
    ax.set_xlabel('Time (s)')
    ax.axvline(x=start*samp_per, color='g', linestyle='--', label='Bird Present Start')
    ax.axvline(x=end*samp_per, color='g', linestyle='--', label='Bird Present End')
    ax.set_ylabel('Weight (g)')
    # ax.grid(True)
    ax.legend()

    # save the plot
    plt.savefig("simulation/results/conversion.png")
    plt.close()

def validating():
    # path to validations
    val_path = "simulation/outputs/validations.txt"

    # read in the validations
    sig_names = []
    val_status = []
    with open(val_path, "r") as file:
        for line in file:
            parts = line.strip().split(",")
            sig_names.append(parts[0])
            val_status.append(int(parts[1]))

    fig, axes = plt.subplots(4, 1, figsize=(10, 8), sharex=True)
    fig.suptitle('SIGNAL VALIDATION SIMULATION', fontsize=16, fontweight='bold')

    # Iterate over signals and plot each one on a separate subplot
    for i, sig_name in enumerate(sig_names):
        # read in signal
        signal = read_signal("simulation/outputs/" + sig_name + "_val.txt")

        # create time axis
        samp_per = float(1/signal[1])
        no_samples = len(signal[3])
        t = np.linspace(0,samp_per*no_samples,no_samples)

        ax = axes[i]
        ax.plot(t, signal[3], label="Validation Status: " + str(val_status[i]))
        ax.axhline(y=500, color='g', linestyle='--', label='Minimum Weight',linewidth = 1)
        ax.axhline(y=1500, color='g', linestyle='--', label='Maximum Weight',linewidth = 1)
        ax.set_title(signal[0],fontweight='bold')
        ax.set_xlabel('Time (s)')
        ax.set_ylabel('Weight (g)')
        # ax.grid(True)
        ax.legend()

    plt.tight_layout()  # Adjust layout to prevent overlap
        
    # Save plot
    plt.savefig("simulation/results/validations.png")
    plt.close()

def filtering():
    # directory of ssignal files
    directory = "simulation/outputs"

    # define filter types
    filter_type_ext = ["_orig","_mov","_exp_mov","_med","_whit","_kal"]
    orig_sig_names = []
    signals = []

    # get the signal files
    file_paths = []
    for filename in os.listdir(directory):
        file_path = os.path.join(directory, filename)
        if os.path.isfile(file_path):
            if any(ft in file_path for ft in filter_type_ext) and "full_sys" not in file_path:
                file_paths.append(file_path)
                signal = read_signal(file_path)
                if signal[0] not in orig_sig_names:
                    orig_sig_names.append(signal[0])
                    signals.append([])
                signals[orig_sig_names.index(signal[0])].append(signal)
    
    # plot the signals
    for i,sig_name in enumerate(orig_sig_names):
        plot_filters(signals[i],sig_name,"simulation/results",i + 1)

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
    
    # plot the signal
    samp_per = float(1/signal[1])
    no_samples = len(signal[3])
    t = np.linspace(0,samp_per*no_samples,no_samples)
    fig, ax = plt.subplots(figsize=(8, 6))
    fig.suptitle('WEIGHT ESTIMATION AND ASSOCIATED ERROR SIMULATION\n', fontsize=16, fontweight='bold')
    ax.plot(t, signal[3])
    ax.axvline(x=data[2] * samp_per, color='r', linestyle='--', label='Stable Start')
    ax.axvline(x=data[3] * samp_per, color='r', linestyle='--', label='Stable End')
    ax.set_title("Estimated Weight: " + str(data[0]) + "\nPercentage Error: " + str(data[1]),fontweight='bold')
    ax.set_xlabel('Time (s)')
    ax.set_ylabel('Weight (g)')
    ax.legend()
    plt.savefig("simulation/results/estimation.png")
    plt.close()

def full_system():
    sig_paths = ["simulation/outputs/full_sys_orig_sig.txt", "simulation/outputs/full_sys_mov_ave_sig.txt", "simulation/outputs/full_sys_exp_mov_ave_sig.txt", "simulation/outputs/full_sys_med_sig.txt", "simulation/outputs/full_sys_whit_sig.txt", "simulation/outputs/full_sys_kal_sig.txt"]
    est_paths = ["simulation/outputs/full_sys_est_mov.txt", "simulation/outputs/full_sys_est_exp_mov.txt", "simulation/outputs/full_sys_est_med.txt","simulation/outputs/full_sys_est_whit.txt", "simulation/outputs/full_sys_est_kal.txt"]

    # read in signals
    signals = []
    for sig_path in sig_paths:
        signals.append(read_signal(sig_path))

    # create time axis
    samp_per = float(1/signals[0][1])
    no_samples = len(signals[0][3])
    t = np.linspace(0,samp_per*no_samples,no_samples)

    # read in estimations
    estimations = []
    for est_path in est_paths:
        with open(est_path, 'r') as file:
            lines = file.readlines()
        estimations.append([float(line.strip()) for line in lines if line.strip()])

    # plot the signals and thier estimations
    fig, axes = plt.subplots(len(signals) - 1, 1, figsize=(10, 10), sharex=True)
    fig.suptitle('FULL SYSTEM SIMULATION', fontsize=16, fontweight='bold')
    for i in range(len(signals) - 1):
        ax = axes[i]
        ax.plot(t, signals[0][3], label=signals[0][2])
        ax.plot(t, signals[i + 1][3])
        ax.set_title(signals[i + 1][0] + "\nEstimated Weight: " + str(estimations[i][0]) + "\nPercentage Error: " + str(estimations[i][1]),fontsize = 10)
        ax.axvline(x=estimations[i][2]*samp_per, color='r', linestyle='--', label='Stable Start')
        ax.axvline(x=estimations[i][3]*samp_per, color='r', linestyle='--', label='Stabe End')
        ax.legend(fontsize='small')
        ax.set_xlabel('Time (s)')
        ax.set_ylabel('Force')
        # ax.grid(True)
        ax.legend()

    plt.tight_layout()
    plt.savefig("simulation/results/full_system.png")
    plt.close()

if __name__ == "__main__":
    plt.rcParams['legend.fontsize'] = 8
    plt.rcParams['axes.titlesize'] = 10
    conversion()
    filtering()
    estimation()
    validating()
    full_system()