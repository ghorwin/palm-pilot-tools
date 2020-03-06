# palm-pilot-tools
Palm sync and data exchange software - a revival of the dead pilot-link project.

For those who still own one of the old Palm organizers like my TX and would like to still use this, I'm attempting to 
update the source code of the original palm-link software to make it work with modern cloud-based organizers/address books.

Personal goal: get the Palm to _convieniently_ sync with OwnCloud/NextCloud caldav/carddav devices.

## Target platform

For now I'm focusing on Linux (Ubunutu 18.04, but the stuff should be pretty generic and run on other platforms). I'm trying 
to keep external project/library dependencies to a minimum, so porting to other platforms/distros won't be that hard (I hope).

### Syncing func

- Focus is on USB/Cradle sync, Bluetooth and network appear more tricky, so that's lower priority

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

- [x] get at least part of palm-pilot source to compile
- [ ] analyze and understand the original palm-pilot code base
- [x] try to get `pilot-dlps` tool to compile
- [ ] try to talk to my Palm TX via `pilot-dlps`


