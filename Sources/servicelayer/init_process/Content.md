# Init Process
This houses the main init process of the entire system. It is responsible for
booting both the initramfs, the system and the recovery itself.

## Main Process (initramfs)
At the initramfs stage, the init process sets up and configures the basic system
to be able to get executed further. This involves loading in the kernel modules,
some of them being known as drivers, along with loading in the device firmware
and other things. Apart from that, if the user has an encrypted data partition,
the init system needs to prompt the user for the disk password to continue.

## Recovery Init Process
Documentation coming up

## System Init Process
Documentation coming up

## Special key combinations
To avoid having to boot into the system and being required to explicitly go into
Settings and boot into the recovery on next boot, you can perform one of these key
combinations to boot into the recovery.

### Booting into recovery from init - mobile
You may already guess the combination. It's Volume Up and the Power button. Hold
it for 5 seconds, until the device logo shows up, and it will boot directly into
the recovery system.

### Booting into recovery from init - desktops
On laptops, along with desktop systems, we got the power of a keyboard. You can
power on into the recovery by pressing and holding down the F8 key. After 5
seconds, it will boot into the recovery system.

Alternatively, for specific recovery commands, there are a few other things that
can be used. Some of them include:

- `F8` + `F` -> Boot into recovery and prepare flashing
- `F8` + `W` -> Boot into recovery and wipe data + cache
- `F8` + `C` -> Boot into recovery and wipe cache
- `F8` + `S` -> Boot into recovery and mount system in RW
- `F8` + `T` -> Boot into a recovery shell
- `F8` + `K` -> Boot into recovery and prepare the system for a clean device state
  - This will perform a complete data + cache wipe, once all installed applications
    are moved from the user data to the system itself. It is generally used to
    prepare a system and can be helpful with providing custom system images.
  - Once a system image has been prepared, boot into the recovery shell and type
    in `system_image export`, and you'll get thrown into an interactive application.
    You can either export the system image via network or by exporting it to a
    USB drive or other storage media.

You can combine multiple key combinations, such as `F8` + `S` + `T` to perform
both of these actions, although it depends on which actions need to be taken.