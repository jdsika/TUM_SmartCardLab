from dpa import perform_dpa
from aes import testKey, testCardKey, testTestKey
import scipy.io
import numpy as np
import sys
import h5py
import time
import matplotlib.pyplot as plt

def hexVector2number(row):
    result = 0x00
    for bytenum in range(16):
        result = result | (int(row[bytenum]) << (15 - bytenum) * 8)
    return result

if len(sys.argv) > 1:
    filename = sys.argv[1]
else:
    filename = 'measurement.mat'
print 'Using file: %s' % (filename)

# Manual switch for v7.3 .mat files
if False:
    data = h5py.File(filename, 'r')
    plaintext = data['plaintext'][()].astype(int).T
    ciphertext = data['ciphertext'][()].astype(int).T
    traces = data['traces'][()].T
else:
    data = scipy.io.loadmat(filename, appendmat=False)
    plaintext = data['plaintext']
    ciphertext = data['ciphertext']
    traces = data['traces']

print 'Starting DPA...'

startTraces = 1500
stepSize = 100
numRuns = startTraces/stepSize
traces = traces[:startTraces,:]
plaintext = plaintext[:startTraces,:]
ciphertext = ciphertext[:startTraces,:]

# numRuns = 100
# mask = np.zeros(len(traces), dtype=bool)
# mask[:200] = True

maxCorrs = np.zeros((numRuns, 256))
nTraces = np.zeros(numRuns)
timing = np.zeros(numRuns)
results = np.zeros(numRuns)

for run in range(numRuns):

    # mask = np.random.permutation(mask)
    # curTraces = traces[mask,:]
    # curPlaintext = plaintext[mask,:]
    # curCiphertext = ciphertext[mask,:]

    curTraces = traces[stepSize*run:,:]
    curPlaintext = plaintext[stepSize*run:,:]
    curCiphertext = ciphertext[stepSize*run:,:]

    start = time.clock()
    dpaResult = perform_dpa(curPlaintext, curTraces, showPlot=False, fullPlot=False, silent=True)
    end = time.clock()

    masterKeyVec = dpaResult[0]
    maxCorrs[run, :] = dpaResult[1]

    duration = end - start
    print "\nDPA %d. run:" % (run + 1)
    print "\tTraces:\tn = %d (%d samples per trace)" % (curTraces.shape[0], curTraces.shape[1])
    print "\tTime:\t%0.2lfs (%0.2lfs per key byte)" % (duration, duration/16)

    masterKey = hexVector2number(masterKeyVec)
    print "\tKey:\t0x%0.32X" % masterKey


    ## Test Key from DPA:

    samplePlainText  = hexVector2number(curPlaintext[2,:])
    sampleCipherText = hexVector2number(curCiphertext[2,:])

    #print "Testing Key with provided Plaintext and Ciphertext: "
    success = testKey(masterKey, sampleCipherText, samplePlainText)
    print ""

    timing[run] = duration
    nTraces[run] = traces.shape[0]
    results[run] = success

timeMean = np.mean(timing)
successRate = np.mean(results)

print ""
print "Success Rate: %lf" % successRate
print "Time:\t%0.2lfs (%0.2lfs per key byte)" % (timeMean, timeMean/16)

np.savez("results/corrsData", maxCorrs=maxCorrs, nTraces=nTraces, timing=timing)
