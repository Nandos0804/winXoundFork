/*
BufFiPl1 - Plays a mono sample from a GEN01 function table, including sample rate conversion

DESCRIPTION
Plays a mono sample from a GEN01 function table, including sample rate conversion. The input parameters are similar to diskin (speed, loop play or play once, skiptime) plus a factor for amplitude scaling. 
See the UDO BufPlay for playing any buffer, with some more options, but without sample rate conversion.

SYNTAX
aout, kfin BufFiPl1 ifn, kplay, kspeed, kvol [, iskip [, iwrap]]

INITIALIZATION
ifn - number of the function table which contains the sample (please use GEN01 - any other GEN routine will lead to an error because it does not contain the sample rate of the soundfile)
(you can also use BufFiCt1 for creating the table - see the example below) 
iskip - skiptime in seconds
iwrap - if iwrap=0 the file is just played once (stops at end of table for positive speed and at start of table for negative speed), or stops if the direction of speed changes
iskip - skiptime (sec)
iwrap - iwrap=0 plays the file is just once (stops at end of table for positive speed and at start of table for negative speed), or stops if the direction of speed changes



PERFORMANCE
kplay - 1 plays the table, 0 or any other number stops playing
kspeed - speed and direction (negative = backwards) of the pointer through the sample, e.g. 1 = normal, 2 = double (=octave higher), -0.5 = half (octave lower) and backwards
kvol - ampltude scaling factor (1 = original amplitude)
aout - audio output
kfin - 1 if iwrap=0 and playback has finished, otherwise 0

CREDITS
joachim heintz 2008/2010
*/

  opcode BufFiPl1, ak, ikkkop
ifn, kplay, kspeed, kvol, iskip, iwrap xin
;SAFETY CHECK IF THE TABLE IS REALLY GENERATED BY GEN01
if ftsr(ifn) == 0 then
 prints    "ERROR!\n FUNCTION TABLE %d HAS NO SAMPLE RATE VALUE (NOT GENERATED BY GEN01?)\n", ifn
 prints    "PERFORMANCE HAS BEEN TURNED OF.\n"
 turnoff
endif
iftlen    =         ftlen(ifn)/ftsr(ifn)
kcps      =         kspeed / iftlen
iphs      =         iskip / iftlen
kfin      init      0
;CALCULATIONS ONLY REQUIRED FOR WRAP=0
if iwrap == 0 then
kndx      phasor    kcps, iphs
kfirst    init      1 ;don't check condition below at the first k-cycle (always true)
kprevndx  init      0
 ;end of table check:
  ;for positive speed, check if this index is lower than the previous one
  if kfirst == 0 && kspeed > 0 && kndx < kprevndx then 
kfin      =         1
  else
 ;for negative speed, check if this index is higher than the previous one
kprevndx  =         (kprevndx == iphs ? 1 : kprevndx) 
   if kfirst == 0 && kspeed < 0 && kndx > kprevndx then
kfin      =         1
   endif
kfirst    =         0 ;end of first cycle in wrap = 0
  endif
kprevndx  =         kndx ;next previous is this index
endif
;READING THE SOUND WITH POSCIL3
if kplay == 1 && kfin == 0 then
asig      poscil3   kvol, kcps, ifn, iphs
else
asig      =         0
endif
          xout      asig, kfin
  endop
 
