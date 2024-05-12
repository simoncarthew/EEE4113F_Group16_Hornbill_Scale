import csv

def sort_csv(input_file, output_file):
    # Read CSV file and store rows in a list
    with open(input_file, 'r') as file:
        reader = csv.reader(file)
        data = list(reader)

    # Separate header row from data
    header = data[0]
    data_rows = data[1:]

    # Sort data based on the last column
    sorted_data = sorted(data_rows, key=lambda row: float(row[-1]))

    # Write sorted data back to CSV file
    with open(output_file, 'w', newline='') as file:
        writer = csv.writer(file)

        # Write header row
        writer.writerow(header)

        # Write sorted data with floats formatted with maximum 3 decimal places
        for row in sorted_data:
            formatted_row = [format(float(cell), '.3f').rstrip('0').rstrip('.') if cell.replace('.', '').isdigit() else cell for cell in row]
            writer.writerow(formatted_row)

# Input and output file paths
input_file = 'test_results/filter_testing.csv'

# Sort CSV
sort_csv(input_file, input_file)

print("Results file sorted and written to", input_file)
