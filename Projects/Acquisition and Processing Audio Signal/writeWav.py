from scipy.io.wavfile import write
import numpy as np
from datetime import datetime
import time
samplerate = 44100; #frecventa de esantionare

h = open('waveDataOutput.txt', 'r')
nameFile = open('numeMelodie.txt','r');
 
t = time.localtime()
current_time = time.strftime("%H_%M_%S_", t)

data = h.readlines()
data2 = []
name = 'waveOut\\' + current_time +  nameFile.readline() 

for i in data:
    data2.append(float(i.replace('\n','')))
    
datanp = np.array(data2)

write(name, samplerate, datanp.astype(np.int16))