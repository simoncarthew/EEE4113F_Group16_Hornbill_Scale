#### IMPORTS ####
import serial
import os

#### FUNCTIONS ####

def append_to_file(filename, data):
    try:
        with open(filename, 'a') as file:
            file.write(data)
            file.write('\n')  # Add a newline after each entry if desired
    except IOError as e:
        print("Error appending data to", filename, ":", e)

def create_file(filename):
    try:
        # Open the file in write mode, which will create a new file or overwrite an existing file
        with open(filename, 'w'):
            pass  # Just create an empty file
        print("File", filename, "created or overwritten")
    except IOError as e:
        print("Error creating or overwriting file", filename, ":", e)

#### EXECUTE ####
if __name__ == "__main__":
    # Open serial port
    ser = serial.Serial('/dev/ttyUSB0', 921600)  # Adjust port and baud rate as necessary

    start = input()
    while start != "start":
        start = input()
    send_in = "start"
    ser.write(send_in.encode('utf-8'))

    while True:
        # get the response
        response = ser.readline()
        response = response.decode('utf-8')

        # print the response
        print(response, end = '')

        # send response if asked
        if 'enter' in response or 'happy' in response or 'grams' in response or 'begin' in response:
            user_input = input()  # Wait for user input
            send_in = user_input if user_input == "start" else user_input + '\n'
            ser.write(send_in.encode('utf-8'))  # Encode user input to bytes and write to serial port
            # print(send_in)
            continue


        # if ready for next test
        if 'next' in response:
            # get the file name of the experiment
            print("Enter the file name of the experiment: ")
            file = input()

            # create the file
            file_path = '/home/simon/OneDrive/University/Fourth_Year/EEE4113F/EEE4113F_Group16_Hornbill_Scale/DYNAMIC_WEIGHT_ANALYSIS/test_signals/real_sim/' + file + '.txt'
            create_file(file_path)

            # get the name of the experiment
            print("Enter the title of the experiment: ")
            title = input()
            append_to_file(file_path,'Title: ' + title)

            # add the sampling frequency
            append_to_file(file_path,'Sampling Frequency: ' + str(10))

            # get the weight of the object
            print("Enter the weight of the object in grams: ")
            weight = input()
            append_to_file(file_path,'Weight: ' + str(weight))

            # get the offset value
            off = ser.readline()
            off = off.decode('utf-8')
            append_to_file(file_path,'Offset: ' + str(off).strip('\n'))

            # get the scale value
            scale = ser.readline()
            scale = scale.decode('utf-8') 
            append_to_file(file_path,'Scale: ' + str(scale).strip('\n'))

            # check if waiting for user
            response = ser.readline()
            response = response.decode('utf-8')
            if 'begin' in response:
                print("Enter \"start\" when ready to record.")
                start = input()
                while start != "start":
                    start = input()
                send_in = "start"
                ser.write(send_in.encode('utf-8'))

            # get the load cell values
            response = ser.readline()
            response = response.decode('utf-8')
            print(response,end = '')
            while ('Sent' not in response):
                if response.isdigit() or response[1:].isdigit():
                    val = int(response)
                    append_to_file(file_path, str(val))  # Write the value to the file
                response = ser.readline().decode('utf-8').strip()  # Read the next line and remove leading/trailing whitespace
                print(response)  # Print the response
            
            print("Saved Experiment.")

            continue

        
    # Close serial port
    ser.close()