#!/usr/bin/python -u
# -*- coding: utf-8 -*-

import Crypto.Cipher.AES
import sys
from smartcard.util import toHexString, toBytes, toASCIIString, bs2hl, hl2bs
import random

## This key is stored in the SmartCard:
masterKey=[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
masterCrypt=Crypto.Cipher.AES.new( toASCIIString( masterKey ), Crypto.Cipher.AES.MODE_ECB )
## Amount of stream data to be encoded with the same key:
blocksize=1024*128
preamble="SEC-"*16

def generateNewKey():
  ## This is only a dummy function to
  ## generate random keys in the future.
  ## For now, this routine returns a constant
  ## 16 bytes key:
  retval = ""
  for i in range(16):
    retval += chr( random.randint(0,255) )
  return retval
#fi

i=0
while 1:
  ## Generate a new stream key, and encrypt it.
  theKey=generateNewKey()
  theEncryptedKey=masterCrypt.encrypt( theKey )
  ## Generate a new encrypting engine:
  streamCrypt = Crypto.Cipher.AES.new( theKey )
  ## Read 128kB of data from stdin:
  indata = b""
  indata = sys.stdin.read( blocksize )
  if ( len(indata)==0 ):
    sys.stderr.write( "Reached end of file.\n" )
    sys.exit(1)
  outdata = streamCrypt.encrypt( indata )
  ## Write the preamble:
  sys.stdout.write( preamble )
  ## Write the new encrypted stream key:
  sys.stdout.write( theEncryptedKey )
  i=i+1
  sys.stderr.write( "Writing chunk " + str(i) + "\n" )
  ## Finally write the data chunk:
  sys.stdout.write( outdata )
#end while

