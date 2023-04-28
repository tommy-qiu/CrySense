import serial
import numpy as np
import wave
import time

# Set up the serial port
ser = serial.Serial('COM3', 115200)

# Set up the PDM to PCM conversion parameters
I2S_SAMPLE_RATE = 44000
PCM_SAMPLE_RATE = 48000
CONVERSION_RATIO = int(PCM_SAMPLE_RATE / I2S_SAMPLE_RATE)
I2S_BUFFER_SIZE = 256
PCM_BUFFER_SIZE = I2S_BUFFER_SIZE * CONVERSION_RATIO

# Open the output WAV file
with wave.open('audio.wav', 'wb') as f:
    f.setnchannels(1) # Set the number of channels to 1 (mono)
    f.setsampwidth(2) # Set the sample width to 2 bytes (16 bits)
    f.setframerate(PCM_SAMPLE_RATE) # Set the sample rate to 48 kHz

    # Record for 10 seconds
    start_time = time.time()
    end_time = start_time + 10

    while time.time() < end_time:
        # Read the 16-bit unsigned data from the serial port
        line = ser.readline()
        values = line.decode().strip().split('\n')

        # Convert the 16-bit unsigned data to 16-bit signed data
        i2s_buffer = np.array([np.uint16(value) for value in values])
        pcm_buffer = np.int16((i2s_buffer - 2**15))

        # Write the PCM data to the WAV file
        f.writeframes(pcm_buffer.tobytes())

    print('Recording complete')

    # Update the WAV file header with the correct number of frames
    num_frames = len(pcm_buffer) / f.getnchannels() / (f.getsampwidth() / 2)
    f.setnframes(num_frames)

# Close the serial port
ser.close()