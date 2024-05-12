import os
import matplotlib.pyplot as plt

def plot_signal(file_path,save_path):

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

    # Plot
    plt.figure(figsize=(10, 6))
    plt.plot(data)
    plt.title(title)
    plt.xlabel('Time')
    plt.ylabel('Value')
    plt.grid(True)
    
    # Save plot
    plt.savefig(save_path)
    plt.close()

def process_child_directories():
    test_sig_path = os.path.join(os.getcwd(), "test_signals")
    sim_envs = [name for name in os.listdir(test_sig_path) if os.path.isdir(os.path.join(test_sig_path, name))]
    
    for sim_env in sim_envs:
        for root, dirs, files in os.walk(os.path.join(test_sig_path,sim_env,"sig_files")):
            for file in files:
                if file.endswith(".txt"):
                    plot_signal(os.path.join(test_sig_path,sim_env,"sig_files") + "/" + file,os.path.join(test_sig_path,sim_env,"plotted_sigs") + "/" + os.path.basename(file)[:-4] + ".png")

if __name__ == "__main__":
    process_child_directories()
