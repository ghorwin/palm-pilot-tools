# palm-pilot-tools
Palm sync and data exchange software - a revival of the dead pilot-link project. Note: webpage _palm-pilot.org_ is dead/empty.

For those who still own one of the old Palm organizers like my TX and would like to still use this, I'm attempting to
update the source code of the original palm-link software (see `archive/palm-link`) to make it work with modern cloud-based organizers/address books.

Personal goal: get the Palm to _conveniently_ sync with OwnCloud/NextCloud caldav/carddav devices.

Project deadline: 30.06.2020 (if I can't make it by then, this is taking too long to be practical)

## Researched material

## Target platform

For now I'm focusing on Linux (Ubunutu 18.04, but the stuff should be pretty generic and run on other platforms). I'm trying
to keep external project/library dependencies to a minimum, so porting to other platforms/distros won't be that hard (I hope).

### Syncing func

- Focus is on USB/Cradle sync, Bluetooth and network appear more tricky, so that's lower priority
- Addressbook and calender are my key priorities - everything else is less useful since the device is hardware-wise significantly outdated (though it still works nicely as mp3 player)


## Steps

- [x] check if kernel module 'visor' still works and recognizes my Palm

```
[ 3029.975367] usb 1-2: new full-speed USB device number 19 using xhci_hcd
[ 3030.128488] usb 1-2: New USB device found, idVendor=0830, idProduct=0061, bcdDevice= 1.00
[ 3030.128496] usb 1-2: New USB device strings: Mfr=1, Product=2, SerialNumber=5
[ 3030.128501] usb 1-2: Product: Palm Handheld
[ 3030.128505] usb 1-2: Manufacturer: Palm, Inc.
[ 3030.128510] usb 1-2: SerialNumber: PN70U696V0WV
[ 3030.137179] visor 1-2:1.0: Handspring Visor / Palm OS converter detected
[ 3030.137673] usb 1-2: Handspring Visor / Palm OS converter now attached to ttyUSB0
[ 3030.137889] usb 1-2: Handspring Visor / Palm OS converter now attached to ttyUSB1
```

Devices are created with following users/permissions:

```
crw-rw---- 1 root dialout 188, 0 Mär  6 10:02 /dev/ttyUSB0
crw-rw---- 1 root dialout 188, 1 Mär  6 10:02 /dev/ttyUSB1
```

Users need to be in group `dialout` to read/write to those files.

- [x] get at least part of palm-pilot source to compile
- [x] try to get `pilot-dlps` tool to compile
- [x] try to talk to my Palm TX via `pilot-dlps`

Added myself to group `dialout`.

Used command line:

```bash
> pilot-dlps -i --port=usb:/dev/ttyUSB0
# and also
> pilot-dlps -i --port=usb:/dev/ttyUSB1
```

This gives:

```
Listening for incoming connection on usb:/dev/ttyUSB1...
```

and nothing afterwards. Regardless if I start HotSync on the Palm first, or while "Listening".

_Note:_ sometimes calls to `pilot-dlps` give:
```
Unable to bind to port: usb:/dev/ttyUSB0
Please use --help for more information
```
It is currently unclear, when this happens. It occurs while the Palm is in the HotSync-process, sometimes afterwards.
(_-> Explanation is found later, see below._)

### Adding debugging output

While executing the tool, I'm setting the following environment variables:

PILOT_DEBUG = DEV SLP PADP CMP NET SOCK API USER
PILOT_DEBUG_LEVEL = DEBUG

Now, when starting HotSync on the Palm, while "Listening..." is shown, the following output is received:

```
DEV POLL linuxusb found data on fd: 3
SOCK fd=3 auto=1
SOCK proto=DLP (6)
```
... then the tool hangs.

Apparently, you need to start HotSync only *after* the device is already listened to. This turned out to be wrong (see below).

Analysis: the code hangs in the loop in socket.c:470 trying to read the first 10 bytes from the socket
to guess the correct protocol. The call to select() indicates "ready to read from file descriptor", yet
read() on the fd returns 0 bytes read. This results in an infinite loop.

I need to understand why the select() call indicates "ready to read", yet no bytes are available.

### Realizing that /dev/ttyUSB1 needs serial protocol

From the coldsync.org webpage (README.usb and README.libusb docs) I learned that the `visor` kernel module actually exposes the /dev/ttyUSB0 and /dev/ttyUSB1 so that they can be talked to with plain serial port protocol.

So, also from the docs I learned that `/dev/ttyUSB1` is the actual sync port, so I'm using this in the future.

New command line:
```bash
> pilot-dlps -i --port=serial:/dev/ttyUSB1
```
Still, no data being read from the port. Debuggin into the serial protocol I stumbled across the function `get_pilot_rate()` in `utils.c`, which reads the environment variable PILOTRATE to extract the baud rate. Problem is, when the Palm has a rate of 115200 configured and PILOTRATE is set to this value - still no data exchange. *BUT* if I configure the _Cradle/Cable_ connection in the Palm to use 9600 as baud rate, then **it works!!**

###  Variants tested

#### Trial 1: run `pilot-dlps` first and then start HotSync

I start `pilot-dlps` and when the output

```
Listening for incoming connection on serial:/dev/ttyUSB1...
```
is printed, press the HotSync button. The output

```
DEV POLL unixserial found data on fd: 3
SOCK fd=3 auto=1
SOCK proto=DLP (6)
```
appears and afterwards the code hangs in the "read the first 10 bytes" loop.

Now, I monitored dmesg output
```
> dmesg -mH
```
continuously, to check what happens when I press the HotSync button. And, oha, look at the output of dmesg
```bash
# here I pressed the HotSync button
[  +0,000465] visor ttyUSB0: Handspring Visor / Palm OS converter now disconnected from ttyUSB0
[  +0,000219] visor ttyUSB1: Handspring Visor / Palm OS converter now disconnected from ttyUSB1
[  +0,000031] visor 1-1:1.0: device disconnected
[  +0,519456] usb 1-1: new full-speed USB device number 40 using xhci_hcd
[  +0,148991] usb 1-1: New USB device found, idVendor=0830, idProduct=0061, bcdDevice= 1.00
[  +0,000001] usb 1-1: New USB device strings: Mfr=1, Product=2, SerialNumber=5
[  +0,000001] usb 1-1: Product: Palm Handheld
[  +0,000001] usb 1-1: Manufacturer: Palm, Inc.
[  +0,000000] usb 1-1: SerialNumber: PN70MCM5V25U
[  +0,002176] visor 1-1:1.0: Handspring Visor / Palm OS converter detected
[  +0,000043] usb 1-1: Handspring Visor / Palm OS converter now attached to ttyUSB2
[  +0,000035] usb 1-1: Handspring Visor / Palm OS converter now attached to ttyUSB3
```

Well, no wonder reading from /dev/ttyUSB1 doesn't work anylonger. Pressing the hotsync button on the Palm disconnects the usb cable shortly
and then upon reconnect the visor kernel module steps in. Since ttyUSB1 is still open (in the running pilot-dlps) tool, it now creates ttyUSB2 and ttyUSB3 devices.

That's at least an explanation for the many forum/user group bug reports about broken pilot link functionality. Maybe the visor module functionality has changed
in recent years, or this has been a problem for longer... in any case, keeping the device file open while asking the user to press the HotSync button won't work.


#### Trial 2: start HotSync on the Palm and run `pilot-dlps` next

Well, worked 1 out of 3 times. Here's the output of the successful run:

```
DEV POLL unixserial found data on fd: 3
SOCK fd=3 auto=1
SOCK proto=DLP (6)
DEV RX unixserial read 6 bytes
DEV RX unixserial read 6 bytes from read-ahead buffer
DEV RX unixserial read 4 bytes

using NET protocol (skipped 0 bytes)
DEV RX unixserial read 1 bytes from read-ahead buffer
NET RX (3): Checking for headerless packet 1
DEV RX unixserial read 5 bytes from read-ahead buffer
DEV RX unixserial read 4 bytes from read-ahead buffer
DEV RX unixserial read 18 bytes
NET RX sd=3 type=1 txid=0xff len=0x0016
  0000  90 01 00 00 00 00 00 00 00 20 00 00 00 08 00 00   ......... ......
  0010  01 00 00 00 00 00                                 ......
DEV TX unixserial wrote 6 bytes
DEV TX unixserial wrote 50 bytes
NET TX sd=3 type=1 txid=0xff len=0x0032
  0000  12 01 00 00 00 00 00 00 00 20 00 00 00 24 ff ff   ......... ...$..
  0010  ff ff 3c 00 3c 00 00 00 00 00 00 00 00 00 c0 a8   ..<.<...........
  0020  a5 1f 04 27 00 00 00 00 00 00 00 00 00 00 00 00   ...'............
  0030  00 00                                             ..
DEV RX unixserial read 6 bytes
DEV RX unixserial read 50 bytes
NET RX sd=3 type=1 txid=0xff len=0x0032
  0000  90 01 00 00 00 00 00 00 00 20 00 00 00 08 00 00   ......... ......
  0010  01 00 00 00 00 00 92 01 00 00 00 00 00 00 00 20   ...............
  0020  00 00 00 24 ff ff ff ff 00 3c 00 3c 40 00 00 00   ...$.....<.<@...
  0030  01 00                                             ..
DEV TX unixserial wrote 6 bytes
DEV TX unixserial wrote 46 bytes
NET TX sd=3 type=1 txid=0xff len=0x002e
  0000  13 01 00 00 00 00 00 00 00 20 00 00 00 20 ff ff   ......... ... ..
  0010  ff ff 00 3c 00 3c 00 00 00 00 00 00 00 01 00 00   ...<.<..........
  0020  00 00 00 00 00 00 00 00 00 00 00 00 00 00         ..............
DEV RX unixserial read 6 bytes
DEV RX unixserial read 8 bytes
NET RX sd=3 type=1 txid=0xff len=0x0008
  0000  90 01 00 00 00 00 00 00                           ........
connected!

# afterwards much more communication and then the interactive shell starts where I can successfully execute quite a few commands...
```

In the failed cases, the communication stops after the output
```
DEV RX unixserial read 6 bytes
```

Interestingly, if I run this in the debugger and put a break point just before the first read-10-bytes call:

call stack:

```
1 s_read               unixserial.c  466  0x55fa1105f6c1
2 protocol_queue_build socket.c      472  0x55fa11058eec
3 pi_socket_init       socket.c      983  0x55fa11059bf1
4 pi_serial_accept     serial.c      495  0x55fa11057342
5 pi_accept_to         socket.c      1139 0x55fa1105a095
6 plu_connect          userland.c    80   0x55fa1106eceb
7 main                 pilot-dlpsh.c 820  0x55fa1106e824
```

and step over the first read call, I get 10 bytes. So, it appears to be a timing issue. I'm adding a `sleep(1)` call at

`socket.c:470` and now the connection appears to work all the time (no longer getting stuck after reading the first 6 bytes).

### Summary of analysis so far

- with `visor` kernel module loaded, you need to use serial protocol to talk to the Palm
- setting custom baud rate doesn't work, configure Palm HotSync Cradle connection to use 9600
- first the HotSync must be started, so that the visor module can setup /dev/ttyUSB1 correctly, *before* it is claimed by the palm-pilot tools
- modern computers are too fast for the serial connection to get startet properly. To avoid missing out on the communication (first packets get lost),
  a sleep() call is needed (at least on my machine)


Next steps:

- [ ] try to get coldsync to work with the Palm, to have a second piece of code to get ideas from
- [ ] analyze and understand the original palm-pilot code base, try to write a small serial communication part

### Documentation

- [ ] document alternative way of syncing with VirtualBox + WinXP + visor blacklisted  + usbforward + Palm+Desktop

### Palm data

- collect prc files from various sources
