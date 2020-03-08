# palm-pilot-tools
Palm sync and data exchange software - a revival of the dead pilot-link project. Note: webpage _palm-pilot.org_ is dead/empty.

For those who still own one of the old Palm organizers like my TX and would like to still use this, I'm attempting to
update the source code of the original palm-link software (see `archive/palm-link`) to make it work with modern cloud-based organizers/address books.

Personal goal: get the Palm to _conveniently_ sync with OwnCloud/NextCloud caldav/carddav devices.

Project deadline: 30.06.2020 (if I can't make it by then, this is taking too long to be practical)

## Target platform

For now I'm focusing on Linux (Ubunutu 18.04, but the stuff should be pretty generic and run on other platforms). I'm trying
to keep external project/library dependencies to a minimum, so porting to other platforms/distros won't be that hard (I hope).

### Syncing func

- Focus is on USB/Cradle sync, Bluetooth and network appear more tricky, so that's lower priority
- Addressbook and calender are my key priorities - everything else is less useful since the device is hardware-wise significantly outdated (though it still works nicely as mp3 player)

## Steps

See wiki for details and findings during the process.

- [x] check if kernel module 'visor' still works and recognizes my Palm
- [x] get at least part of palm-pilot source to compile
- [x] try to get `pilot-dlps` tool to compile
- [x] try to talk to my Palm TX via `pilot-dlps`
- [x] try to get coldsync to work with the Palm, to have a second piece of code to get ideas from
- [ ] analyze and understand the original palm-pilot code base (or coldsync), try to write a small serial communication part - i.e. re-implement 
      some of the pilot-link stuff in a more robust way

### Documentation

- [ ] document alternative way of syncing with VirtualBox + WinXP + visor blacklisted  + usbforward + Palm+Desktop

### Palm data

- collect prc files from various sources

