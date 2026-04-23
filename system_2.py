import serial
import time
import csv

PORT = "COM4"
BAUD_RATE = 9600
NUM_READINGS = 25
FILE_NAME = 'smart_bin_readings.csv'


def parse_data(data_line):


    try:

        parts = data_line.split('|')

        #  Extract values by splitting each part at the colon ':'
        # [1] gets the data after the colon, .strip() removes spaces
        fill = parts[0].split(':')[1].strip()
        waste_type = parts[1].split(':')[1].strip()
        hand_dist = parts[2].split(':')[1].strip()
        motion = parts[3].split(':')[1].strip()
        alert = parts[4].split(':')[1].strip()
        status = parts[5].split(':')[1].strip()

        return fill, waste_type, hand_dist, motion, alert, status
    except Exception as e:
        print(f"Error parsing line: {e}")
        return None, None, None, None, None, None


def read_data():
    try:
        with serial.Serial(PORT, BAUD_RATE, timeout=2) as arduino:
            print(f"Connected to {PORT}. Collecting {NUM_READINGS} points...")
            time.sleep(2)  # Arduino Reset Buffer

            with open(FILE_NAME, 'w', newline='') as csvfile:
                # Updated headers to match Smart Bin project
                fieldnames = ['Number', 'Fill', 'Type', 'Hand', 'Motion', 'Alert', 'Collection_Status']
                writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
                writer.writeheader()

                i = 1
                while i <= NUM_READINGS:
                    if arduino.in_waiting > 0:
                        line = arduino.readline().decode('utf-8', errors='ignore').strip()

                        # Only process the line if it looks like data string
                        if "Fill:" in line and "status:" in line:
                            fill, w_type, hand, mot, alrt, stat = parse_data(line)

                            if fill is not None:
                                writer.writerow({
                                    'Number': i,
                                    'Fill': fill,
                                    'Type': w_type,
                                    'Hand': hand,
                                    'Motion': mot,
                                    'Alert': alrt,
                                    'Collection_Status': stat
                                })
                                print(f"Data point {i}: Fill: {fill} | Collection: {stat}")
                                i += 1

                    time.sleep(0.1)  # Fast check for the next 1-second interval

            print(f"\n--- SUCCESS: Data saved to {FILE_NAME} ---")

    except serial.SerialException as e:
        print(f"Serial Error: {e}")
    except KeyboardInterrupt:
        print("Exiting...")


if __name__ == "__main__":
    read_data()