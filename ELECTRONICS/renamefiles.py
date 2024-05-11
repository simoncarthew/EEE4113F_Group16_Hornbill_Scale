import os

# Set the directory path where the files are located
directory = "Data/Pictures/9th-11th/"

# Loop through all files in the directory
for filename in os.listdir(directory):
    # Get the full file path
    file_path = os.path.join(directory, filename)

    # Check if the file is a regular file (not a directory)
    if os.path.isfile(file_path):
        # Check if the file has an extension
        if '.' not in filename:
            # If no extension, add '.jpg' extension
            new_filename = filename + ".jpg"
            new_file_path = os.path.join(directory, new_filename)
            os.rename(file_path, new_file_path)
            print(f"Renamed {filename} to {new_filename}")
        else:
            # If the file already has an extension, skip it
            print(f"Skipped {filename}")