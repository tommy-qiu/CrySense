# -*- coding: utf-8 -*-

#Code based on https://stackoverflow.com/questions/41209733/csv-to-wav-python

import wave
import struct
import csv
import numpy 
from scipy.io import wavfile
from scipy.signal import resample

fname = r"C:\Users\AlanJihan\..."
#Add the file path to the global above

sampleRate = 44100
#Above, change this for the sample rate of the audio
#Which is samples per second

def write_wav(data, filename, framerate, amplitude):
    wavfile = wave.open(filename,'w')
    nchannels = 1
    sampwidth = 2
    framerate = framerate
    nframes = len(data)
    comptype = "NONE"
    compname = "not compressed"
    wavfile.setparams((nchannels,
                        sampwidth,
                        framerate,
                        nframes,
                        comptype,
                        compname))
    frames = []
    for s in data:
        mul = int(s * amplitude)
        frames.append(struct.pack('h', mul))

    frames = ''.join(frames)
    wavfile.writeframes(frames)
    wavfile.close()
    print("%s written" %(filename)) 

def main():
    global fname
    global sampleRate
    data = []
    for time, value in csv.reader(open(fname, 'r'), delimiter=','):
        try:
            data.append(float(value))#Here you can see that the time column is skipped
        except ValueError:
            pass # Just skip it

    arr = numpy.array(data, dtype = numpy.float32)#Just organize all your samples into an array
    # Normalize data
    arr /= numpy.max(numpy.abs(data)) #Divide all your samples by the max sample value
    filename_head, extension = fname.rsplit(".", 1)        
    data_resampled = resample( arr, len(data) )
    wavfile.write('rec.wav', sampleRate, data_resampled) #resampling at 16khz
    print ("File written succesfully !")
    
main()