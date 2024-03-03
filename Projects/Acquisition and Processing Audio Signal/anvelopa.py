import numpy as np
from scipy.signal import hilbert
signal=[]
file=open("waveData.txt","r")
for line in file:
	signal.append(int(line))
analytic_signal = hilbert(signal) #signal este vectorul aferent semnalului analizat
amplitude_envelope = np.abs(analytic_signal) #amplitude_envelope este anvelopa semnalului
file.close()
file=open("anvelopa.txt","w")
file.write("")
file.close()
file=open("anvelopa.txt","a")
for i in amplitude_envelope:
	file.write(str(i)+"\n")
