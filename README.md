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
- [ ] try to talk to my Palm TX via `pilot-dlps`

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
It is currently unclear, when this happens. It occurs while the Palm is in the HotSync-process, sometimes
afterwards.



- [ ] analyze and understand the original palm-pilot code base

### Documentation

- [ ] document alternative way of syncing with VirtualBox + WinXP + visor blacklisted  + usbforward + Palm+Desktop
- [ ] document setup of usb-comm on modern linux (visor kernel module + dialout group + ...)

### Palm data

- collect prc files from various sources
