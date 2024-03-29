# Biopotential Recorder ADS1219

This simple Arduino project is built around the ADS1219 where this ADC is used and configured to read data at 250Hz in differential mode, in my case it’s EEG readings. This data is then stored on an SD-Card as contiguous 24 bit integers.

It configures a timer with interrupts that requests new ADC readings and I've implemented double buffering for saving data to an SD-Card for more efficient data storage. This timer should work for most Arduino based boards.


There is an ADS1219 driver available for Arduino elsewhere but I’ve modified it and added my own copy here which you’ll need to use. I think there might be some mistakes with the config part in the original one and also I wanted to use the device without the Data Ready pin just for an easier hardware setup. You only really need the data ready pin and hardware interrupts for higher sample rates.

I have also included a python script that can be used to convert the recorded data into both wav and csv files as well as display a spectrogram of the data, below is an example of the output.

![Figure_2](https://github.com/jabituyaben/Biopotential-Recorder-ADS1219/assets/74158243/f0326b23-cd38-4a77-8dde-1026ea4b1138)
