# AnlinOS
**AnlinOS** is a custom Linux-based operating system from the ground up for both the mobile
(Android-based mobile devices) and desktop environments, taking the idea of LFS (Linux From
Scratch) into a fully customized idea. This operating system aims to merge the best parts
from other operating systems,

- Windows containing the Compatibility Layer
- Linux on the core itself (yes, it is a kernel)
- Android being virtualized on the Type-1 Layer (see `wine` and
  [`alde`](https://github.com/BC100Dev/alde))

This will break quite many packages that would be ported, since there are ideas on refining
the Root File Structure (`/`) itself. The idea behind it is to make the File Structure more
beginner-friendly. Certain directories like `/dev`, `/sys` and `/proc` will stay, but I will
mainly reconstruct the use of `/usr`, `/home` and other directories. I might aswell make two
different builds, one for starters and one for Linux superiors, but I don't have my head this
far into the future.

Note that this is not another ROM, nor a custom Android fork. AnlinOS is an independent OS
built with full developer control and modular design.

---

## Key Concepts
- **Built on customized LFS**: This OS does not contain a repackaged distribution base, like
  Zorin OS, taking the distro base of Ubuntu. AnlinOS is constructed using LFS for complete
  control for each component.
- **Android Framework being optional**: Unlike AOSP-based systems, AnlinOS does not depend on
  the Android Runtime, SystemUI or typical Android HALs. This means that the Android Framework
  Support is translated to the glibc Layer (see [`alde`](https://github.com/BC100Dev/alde) for
  reference).
- **Compatibility Layers**: The core idea is to get most programs to work with a compatibility
  layer, similar to how Windows operates, making older shared libraries still being present,
  even when they are no longer being up-to-date.
- **DE compatible with Mobile and Desktops**: A desktop environment, in which we know with
  GNOME and KDE Plasma, are integral to a functional system for newbies. This OS will deliver
  a custom Graphical Environment that is capable for Mobile and Desktop systems, powered with
  the use of a Safety Protocol we all love, *Wayland*.
- **GSI-like experience**: Android in itself gives you the possibility to have a GSI (Generic
  System Image) on an Android device, meaning one ROM can support several devices. This OS
  takes that concept in developing a suitable solution, especially for those that are mainly
  afraid of unlocking the bootloader and possibly bricking the device in the process. Don't
  worry, you still need to unlock the bootloader :skull:

---

## Current Status
AnlinOS is currently under heavy development. Most components are in early implementation or
experimental testing. Code will remain hosted on GitHub during this stage, but may migrate to a
self-hosted Git repository once the codebase grows or outpaces GitHub’s project structure.

As of now, AnlinOS is currently in the planning stages. Once this project goes out from the
planning stages, it will go into the second stage, heavy development. Most components will
mainly be rewritten to maintain the core concept of a ***potential*** new C-library that isn't
`glibc`, but having complete support for `glibc` and `bionic` themselves, merging the two
C-systems into one. This is merely an idea of the possibilities, but wouldn't say that it would
be a 100% idea. Maybe for the future, but for now, AnlinOS will mainly reside with `glibc`.

Another thing to mention is keeping the status of this entire thing being open source. Now,
sure, GitHub can help me store the sources for now, but once I hit the limitations, I would
have to move the source files over to my own server, bypassing the limitations in the process,
so that it may or may not even grow up to 10-50 GB in size. I don't have any specific
expectations, as to what storage space it'll finalize at, when it comes to the source code, but
I assume that it will end up at somewhere approximately 20 GB. Hopefully, not more. Oh, and
I clearly don't want to rely on the `repo` tool developed by Google themselves, especially
that AOSP went into private development.

---

## Planned Release Strategy
Now, I might have an idea on how I could keep this OS up-to-date, while also having the mind on
keeping the OS secure. Here is the outline:

- **Feature Updates**: Released every year or two, similar to Windows' major version changes.
  Focused on introducing new capabilities, graphical updates and/or architectural changes.
- **Minor Updates**: Rolled out approximately every two months, in which it includes Security
  Updates, Bug Fixes, Translation and Device Support Improvements.
- **No forced monthly releases**: AnlinOS prioritizes stability and long-term maintainability
  over rapid feature churn.

This way, I make sure that AnlinOS has the stability in mind, contains the long-term
maintainability and I also make sure that I keep myself sane (because someone eventually also
needs a break :skull: ), so that updates are happening gradually instead of rapidly (looking
at Arch Linux over here).