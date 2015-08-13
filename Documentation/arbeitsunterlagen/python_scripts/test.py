# -*- coding: utf-8 -*-
###########################################################
##                                                       ##
## (c) Institute for Security in Information Technology  ##
##     Prof. Dr.-Ing. Georg Sigl                         ##
##     Technische Universität München                    ##
##                                                       ##
##     Authors: Hermann Seuschek                         ##
##              hermann.seuschek@tum.de                  ##
##              Martin Strasser                          ##
##              strasser@tum.de                          ##
##                                                       ##
## A test script to demonstrate the SmartCard interface. ##
##                                                       ##
## It sends a single encrypted chunk key to the          ##
## SmartCard and prints out the result.                  ##
##                                                       ##
###########################################################


## Include all library elements:
from smartcard.CardType import ATRCardType
from smartcard.ATR import ATR
from smartcard.CardConnection import CardConnection
from smartcard.CardRequest import CardRequest
from smartcard.util import toHexString, toBytes, toASCIIString, bs2hl
## Import an AES algorithm for checking the results:
import Crypto.Cipher.AES

## Here, we define the card type which we want to
## listen to:
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

## Print the Answer To Reset (ATR), which should be: 3B 90 11 00
print toHexString( cardservice.connection.getATR() )

## The data to be decrypted
## (this usually is the encrypted chunk key from the stream):
DATA=[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]

## Form the command to be sent to the card:
DECRYPT_KEY = [0x88, 0x10, 0, 0, len(DATA)] + DATA + [0x10]
GET_RESPONSE = [0x88, 0xc0, 0x00, 0x00, 0x10]

## First, we send the DECRYPT_KEY command.
## This triggers the decryption in the card. The blue
## light will flash, and the toggle output will show
## a spike.
apdu = DECRYPT_KEY
print 'sending ' + toHexString(apdu)
response, sw1, sw2 = cardservice.connection.transmit( apdu )
print 'response: ', response, ' status words: ', "%x %x" % (sw1, sw2)
## There will be no response here, but the card answers with sw1=0x61, sw2=0x10,
## indicating that there are 16 (=0x10) bytes to read now.

## Now we fetch the decrypted chunk key using the GET_RESPONSE command:
apdu = GET_RESPONSE
print 'sending ' + toHexString(apdu)
response, sw1, sw2 = cardservice.connection.transmit( apdu )
print 'response: ', response, ' status words: ', "%x %x" % (sw1, sw2)

## If we want to check if an assumed key is correct, we calculate the
## result for this assumed key and print it:
ASSUMED_KEY = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
aes_device = Crypto.Cipher.AES.new( toASCIIString( ASSUMED_KEY ) , Crypto.Cipher.AES.MODE_ECB )
response = aes_device.decrypt( toASCIIString( DATA ) )
print 'assumed:  ', bs2hl( response )
## That's it.

