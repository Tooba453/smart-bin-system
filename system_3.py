import csv
import matplotlib.pyplot as plt


def read_csv(file_name):
    number = []
    fill_level = []

    try:
        with open(file_name, mode='r') as file:
            reader = csv.DictReader(file)

            for row in reader:

                number.append(int(row['Number']))
                fill_level.append(float(row['Fill']))

    except FileNotFoundError:
        print(f"Error: The file {file_name} does not exist. Run the collector first!")
    except Exception as e:
        print(f"An error occurred: {e}")

    return number, fill_level


def plot_data(numbers, fill_level):
    plt.figure(figsize=(10, 6))

    # Plotting the Fill Level
    plt.plot(numbers, fill_level, marker='o', linestyle='-', color='dodgerblue', label="Fill Distance (cm)")

    plt.axhline(y=10, color='red', linestyle='--', linewidth=2, label="Full Threshold (10cm)")

    plt.title('Smart Bin: Fill Level Monitoring', fontsize=16)
    plt.xlabel('Data Point Number', fontsize=12)
    plt.ylabel('Distance from Sensor (cm)', fontsize=12)

    # Adding a note that lower distance = fuller bin
    plt.text(numbers[0], 22, ' <-- Bin is getting FULL', color='red', fontweight='bold')

    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.show()


if __name__ == "__main__":
    file_name = "smart_bin_readings.csv"

    number, fill_level = read_csv(file_name)

    if number and fill_level:
        plot_data(number, fill_level)