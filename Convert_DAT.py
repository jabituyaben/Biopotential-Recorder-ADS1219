import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
from scipy.signal import butter, lfilter, resample
import yasa
from scipy.io.wavfile import write
import scipy.signal
from sklearn.preprocessing import MinMaxScaler

sf = 200
f0 = 50
Q = 10  # Quality factor

def butter_highpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = signal.butter(order, normal_cutoff, btype = "high", analog = False)
    return b, a

def butter_highpass_filter(data, cutoff, fs, order=5):
    b, a = butter_highpass(cutoff, fs, order=order)
    y = signal.filtfilt(b, a, data)
    return y

def butter_bandpass(lowcut, highcut, fs, order=3):
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype='band')
    return b, a

def butter_bandpass_filter(data, lowcut, highcut, fs, order=1):
    b, a = butter_bandpass(lowcut, highcut, fs, order=order)
    y = lfilter(b, a, data)
    return y

b, a = signal.iirnotch(f0, Q, sf);

analogFile = input('enter filename without .dat extension e.g. ANALOG00: ')

with open(analogFile + ".dat") as f:
    rectype = np.dtype(np.int32)
    eeg = np.fromfile(f, dtype=rectype)

    eeg = signal.filtfilt(b, a, eeg)

    eeg = butter_bandpass_filter(eeg, 1, 38, sf, order=2)
    
    fig = yasa.plot_spectrogram(eeg, sf,fmin=0, fmax=60,cmap='Spectral_r')
    
    plt.show()

print(len(eeg))

np.savetxt(analogFile + ".csv", eeg, delimiter=",")
scaler = MinMaxScaler(feature_range=(0, 65535))
eeg = scaler.fit_transform([[x] for x in eeg])
write(analogFile + ".wav", sf, eeg.astype(np.int16))
