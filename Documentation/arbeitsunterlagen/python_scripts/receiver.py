#!/usr/bin/python -u
# -*- coding: utf-8 -*-

from smartcard.CardType import ATRCardType
from smartcard.ATR import ATR
from smartcard.CardConnection import CardConnection
from smartcard.CardRequest import CardRequest
from smartcard.util import toHexString, toBytes, toASCIIString, bs2hl, hl2bs
import Crypto.Cipher.AES
import sys


## Amount of stream data to be encoded with the same key:
blocksize=1024*128
preamble="SEC-"*16
chunksize=len( preamble ) + 16 + blocksize

## Connect to the card:
cardtype = ATRCardType( toBytes( "3B 90 11 00" ) )
atr = ATR( toBytes( "3B 90 11 00" ) )
if not atr.isT0Supported():
  print "Error: Card does not support T0 protocol."
#fi

cardrequest = CardRequest( timeout=5, cardType=cardtype )

## Wait for the card to be present:
## In case the card is in the reader, this command will return immediately.
cardservice = cardrequest.waitforcard()

## Connect to the card using T0 protocol.
cardservice.connection.connect( CardConnection.T0_protocol )

print toHexString( cardservice.connection.getATR() )

## This function decrypts the chunk key using the SmartCard:
def decryptKey( key ):
  global cardservice
  ## Form the command to be sent to the card:
  DECRYPT_KEY = [0x88, 0x10, 0, 0, len( bs2hl( key ) )] + bs2hl( key ) + [0x10]
  GET_RESPONSE = [0x88, 0xc0, 0x00, 0x00, 0x10]
  response, sw1, sw2 = cardservice.connection.transmit( DECRYPT_KEY )
  response, sw1, sw2 = cardservice.connection.transmit( GET_RESPONSE )
  return hl2bs( response )
#end def


def processChunk( chunk ):
  encKey = b""
  encKey = chunk[ len( preamble ): len( preamble )+16 ]
  encData = b""
  encData = chunk[ len( preamble )+16: ]
  newKey = b""
  newKey = decryptKey( encKey )
  sys.stderr.write( "New key: " + toHexString( bs2hl( newKey ) ) + "\n")
  decryptEngine = Crypto.Cipher.AES.new( newKey )
  decryptData = b""
  decryptData = decryptEngine.decrypt( encData )
  sys.stdout.write( decryptData )
#end def


## First step, syncronize:
indata = b""
## Read at least two times the blocksize, the preamble and the key length to make sure
## that inside indata, there is a complete start of a chunk:
#n=2 ## For non-aligned stream
n=1 ## For well-aligned stream (debug)
indata = sys.stdin.read( chunksize * n )
position = indata.rfind( preamble )
remaining = position - chunksize * n + chunksize
if remaining > chunksize:
  ## Preamble is exactly at end of buffer:
  position = position + chunksize
  remaining = remaining - chunksize
indata = indata[position:] + sys.stdin.read( remaining )

while 1:
  processChunk( indata )
  indata = sys.stdin.read( chunksize )
  if not indata:
    sys.stderr.write( "End of stream.\n" )
    sys.exit( 1 )
  #fi
#end while

