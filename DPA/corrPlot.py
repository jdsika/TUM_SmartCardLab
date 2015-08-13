import scipy.io
import numpy as np
import matplotlib.pyplot as plt
import sys

if len(sys.argv) > 1:
    filename = sys.argv[1]
else:
    filename = 'corrsData.npz'

data = np.load(filename)
nTraces = data['nTraces']
maxCorrs = data['maxCorrs']

figure = plt.figure(1)
plt.plot(nTraces, maxCorrs, '0.8')
plt.plot(nTraces, maxCorrs[:,0xE0])

plt.axis([50, 500, 0, 0.035])
plt.ylabel('Correlation Coefficient')
plt.xlabel('Number of traces')

plt.savefig('figure-corrs.png')
plt.savefig('figure-corrs.pdf', format='pdf')
plt.show()
