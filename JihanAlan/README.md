# I2S connection with microcontroller
<img src="assets\connections.png" width = "550" title = "I2S connections on F446RE MCU" >

# Sub directory contents
*  Code for connections from Secondary microcontroller to the microphone (F446RE sub folder) 
*  Python script for converting the data collected from the microcontroller into a .wav file (CSVWAV subfolder)

# Future plans
- Integrating the data analysis (FFT) in real time using RTOS
    - Combining the FFT and python conversion file into microcontroller (preferably **Raspberry PI**)
- Indicating ML output on the hardware side (*like blinking red LED when the baby is sad*)

# References
[1]:  [Data conversion From CSV to WAVE file](https://stackoverflow.com/questions/41209733/csv-to-wav-python)

[2]:  [Real time FFT algorithm](https://os.mbed.com/teams/onsemi/code/mbed-os/docs/tip/group__ComplexFFT.html) 

