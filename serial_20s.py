import serial
import wave

# Configure serial port
ser = serial.Serial('COM3', 115200)

# Configure .wav file
wf = wave.open("output.wav", "wb")
wf.setnchannels(1)
wf.setsampwidth(2)
wf.setframerate(44100)

# Read data from serial port
samples_recorded = 0
while samples_recorded < 882000:
    # Read a line of data (terminated by \r\n)
    line = ser.readline().decode().strip()

    # Convert data to integer
    try:
        data = int(line)
    except ValueError:
        continue

    # Write data to .wav file
    wf.writeframesraw(data.to_bytes(2, 'little'))

    # Print progress
    #print(".", end="", flush=True)

    # Update sample count
    samples_recorded += 1

# Close .wav file and serial port
wf.close()
ser.close()
print("\nDone!")