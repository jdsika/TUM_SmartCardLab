from dpa import perform_dpa, preprocessing
from aes import testKey, testCardKey, testTestKey
import scipy.io
import numpy as np
import sys
import h5py
import time

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

start = time.clock()
traces = preprocessing(traces)

#dpaResult = perform_dpa(plaintext[1000:1250], traces[1000:1250], showPlot=True, fullPlot=True)
dpaResult = perform_dpa(plaintext, traces, showPlot=True, fullPlot=False)
end = time.clock()
duration = end - start
print "\tTime:\t%0.2lfs" % (duration)

# masterKeyVec = dpaResult[0]
# maxCorrsVec = dpaResult[1]
#
# print "\nDPA Result:"
# print "\tTraces:\tn = %d (%d samples per trace)" % (traces.shape[0], traces.shape[1])
# print "\tTime:\t%0.2lfs (%0.2lfs per key byte)" % (duration, duration/16)
#
# masterKey = hexVector2number(masterKeyVec)
# print "\tKey:\t0x%0.32X" % masterKey
#
#
# ## Test Key from DPA:
#
# samplePlainText  = hexVector2number(plaintext[2,:])
# sampleCipherText = hexVector2number(ciphertext[2,:])
#
# #print "Testing Key with provided Plaintext and Ciphertext: "
# success = testKey(masterKey, sampleCipherText, samplePlainText)
# print ""
