import serial
import time

PORT = "COM4"
BAUD_RATE = 9600


def read_data():
    try:
        # Connect to Arduino
        with serial.Serial(PORT, BAUD_RATE, timeout=1) as arduino:
            print(f"Connected to {PORT}...")
            time.sleep(2)  # Wait for Arduino to reset

            while True:
                # Checks if there is data in the serial buffer
                if arduino.in_waiting > 0:
                    # Read the line and clean up whitespace
                    line = arduino.readline().decode('utf-8', errors='ignore').strip()

                    # Prints the line only if it contains text
                    if line:
                        print(line)

                # Small delay to keep the loop from running too fast
                time.sleep(0.01)

    except serial.SerialException as e:
        print(f"Error: {e}")
    except KeyboardInterrupt:
        print("\nStopped.")


if __name__ == "__main__":
    read_data()