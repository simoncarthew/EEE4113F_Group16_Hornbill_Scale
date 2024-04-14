import os
import matplotlib.pyplot as plt
import numpy as np

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

    plt.figure(figsize=(10, 6))
    for signal in signals:
        # Plot
        plt.plot(t,signal[3],label=signal[2])
        plt.title(signal[0])
    plt.xlabel('Time')
    plt.ylabel('Value')
    plt.grid(True)
    plt.legend()
        
    # Save plot
    save_path = plot_dir + "/" + original_file_name + "_filtered.png"
    plt.savefig(save_path)
    plt.close()

if __name__ == "__main__":
    # get the names of the sim environments
    test_sig_path = os.path.join(os.getcwd(), "test_signals")
    sim_envs = [name for name in os.listdir(test_sig_path) if os.path.isdir(os.path.join(test_sig_path, name))]

    for sim_env in sim_envs:
        # get the path to the original signals
        test_sig_path_env = os.path.join(os.getcwd(),"test_signals",sim_env,"sig_files")

        # intitialise resultant sigs with the originals signals
        tests = []
        for root, dirs, files in os.walk(test_sig_path_env):
            for file in files:
                if file.endswith(".txt"):
                    file_path = os.path.join(root, file)
                    tests.append([os.path.basename(file_path)[:-4],read_signal(file_path)])
        
        # get the path to results folder
        res_sig_path_env = os.path.join(os.getcwd(),"results",sim_env,"sig_files")

        # get resultant filtered signals and group them the original signals in tests array
        for root, dirs, files in os.walk(res_sig_path_env):
            for file in files:
                if file.endswith(".txt"):
                    sig_path = os.path.join(root, file)
                    # read in filtered signal
                    signal = read_signal(sig_path)

                    # find the test that the signal corresponds to
                    for test in tests:
                        if test[0] in sig_path:
                            test.append(signal)
                    

        # plotted results director
        plot_dir = test_sig_path = os.path.join(os.getcwd(),"results",sim_env,"plotted_sigs")

        # plot the tests
        for test in tests:
            plot_signals(test[1:],test[0],plot_dir)
