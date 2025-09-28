# AnlinxOS
**AnlinxOS** is a custom Linux-based operating system from the ground up for both the mobile
(Android-based mobile devices) and desktop environments, taking the idea of LFS (Linux From
Scratch) into a fully customized idea. This operating system aims to merge the best parts
from other operating systems.

- Windows containing the Compatibility Layer
- Linux on the core itself (forked from [torvalds repo](https://github.com/torvalds/linux))
- Android being virtualized on the Type-1 Layer (see `wine` and
  [`alde`](https://github.com/BC100Dev/alde))
  - Quick explanation, `alde` is similar to `wine` that translates Android calls to glibc
    calls. Initially, I archived it because it was no longer in development, thinking that
    I could reuse this concept in this OS, while rebranding it to LSA (Linux Subsystem for
    Android). This will heavily be a required component, especially on Mobile spaces.

This will break quite many packages that would be ported, since this OS will take a different
turn with the FHS (File Hierarchy System), which is known as the root (`/`) structure. Remember
`/bin` and so? Yeah, forget about them, we got `/System`, `/Recovery`, `/Data`, `/Cache` and
`/Users`, each replacing every single component that the old FHS remained. The only thing that
cannot be replaced are `/dev`, `/proc` and `/sys`, unless if I somehow manage to do that, then
this OS will get even more controversy. Oh, and remember `bin` and `lib` being separated? Say
hello to `/System/x64`, merging both of the directories into one, making a System32 alike
directory. Obviously, that `x64` can be replaced with `arm64`, but you will still witness
`/System/bin`, which is just the symlink to the directory of the system architecture of the
kernel + installation configuration.

Now, `/Data/config` will take a heavy role. If you have hated the Windows registry, then you
will hate this concept too because I am introducing LRGX (Linux Registry). This will house the
entirety of the system configurations, although probably not everything. However, this also
nukes `/etc/passwd` and `/etc/shadow` (have you forgotten that `/etc` also doesn't exist?) for
the equivalent of `/Data/config/userbase.lrgx`. By the way, yes, you can have your own Linux
Registry System being in place, along with creating your own Registries much easier. Originally,
I had `/Users` being a symlink to `/Data/UserDir`, but that ended up being not-so-great idea, so
`/Users` is its own dependent directory... which mimics `/home` either way. Oh, and before I
forget... no systemd. Be glad that I am getting rid for you, in case you hated systemd before.

Ever thought running processes is already bad enough, when it comes to libraries? No? Well fuck
that either way, I am introducing [PELF](https://github.com/BC100Dev/PELF), another fucking ELF
format that saves your sanity. Windows, Android and macOS already has a way of packaging resources
into executables, even though Android and macOS use packages in some way either way. Windows has
the `.rsrc` PE header that allows programs to embed resources in. With PELF, I decided to plan
out in making a custom ELF file that allows you to not only compile resources right into the PELF
file, but also allows you to extract it with the use of `libpelf.hpp`. Is that project bad enough
already? No? Then you'll hate me for even thinking of introducing another linker for it because
making a packager for it wasn't a good idea in the first place. However, a security feature...
well, if you even call it a "security feature" (I know you Microsoft haters) is the ELF signing
ability, meaning that you can sign the ELF file itself. I am still not sure, as to how to
accomplish the signature idea, but I will somehow bring it, don't you worry.

In other words, you can see that AnlinxOS is not another Linux distro fork (like Ubuntu being
linked to Debian), LineageOS being AOSP-equivalent and so on. Nope, AnlinxOS is a
self-independent OS.

---

## Key Concepts
- **Built on LFS**: This OS does not contain a repackaged distribution base, like ZorinOS,
  taking the distro base of Ubuntu. AnlinxOS is constructed using LFS (Linux from Scratch) for
  complete control for each component.
- **Android Framework being included**: Unlike AOSP-based systems, AnlinxOS does not depend on
  the Android Runtime, SystemUI or typical Android HALs. This means that the Android Framework
  Support is translated to the glibc Layer (see [`alde`](https://github.com/BC100Dev/alde) for
  reference).
- **GSI-like experience**: Android in itself gives you the possibility to have a GSI (Generic
  System Image) on an Android device, meaning one ROM can support several devices. This OS
  takes that concept in developing a suitable solution, especially for those that are mainly
  afraid of unlocking the bootloader and possibly bricking the device in the process. Don't
  worry, you still need to unlock the bootloader :skull:
- **HAL being customizable**: The HAL (Hardware Abstraction Layer) itself will provide generic
  modules (think of them as drivers), meaning you can load in custom drivers from the device
  vendor you own. This means that you can integrate certain device drivers that will make most
  of the performance onto the operating system.
- **Compatibility Layers**: The core idea is to get most programs to work with a compatibility
  layer, similar to how Windows operates, making older shared libraries still being present,
  even when they are no longer being up to date.
- **DE compatible with Mobile and Desktops**: A desktop environment, in which we know with
  GNOME and KDE Plasma, those are integral to a functional system for newbies. This OS will
  deliver a custom Graphical Environment that is capable for Mobile and Desktop systems, powered
  with the use of a Safety Protocol we all love, *Wayland*.

---

## Current Status
AnlinxOS is currently under heavy development. Most components are in early implementation or
experimental testing. Code will remain hosted on GitHub during this stage, until I may have to
migrate to a self-hosted Git repository once the codebase grows or outpaces GitHub’s project
structure.

As of now, AnlinxOS is currently in the planning stages and in heavy development. Most
components will stay as-is, but certain components such as the `libc` will be needed to be
rewritten due to the refactor of the FHS (File Hierarchy System). So, for the `libc` rewrite,
I thought of merging `glibc` and `bionic` together, since they are the most used C-libraries
that exists, meaning I would merge the best of these two and perform a reimplementation for
those that need new implementations themselves. Yes, this was an initial idea, but seeing
on where the planning is going, this is ending up to be a necessity to be dealt with. UNLESS,
there are configuration files that I am not aware, but then, again, `/usr` and `/etc` won't
exist.

Another thing to mention is keeping the status of this entire thing being open source. Now,
sure, GitHub can help me store the sources for now. However, once I hit the limitations, I would
have to move the source files over to my own server, bypassing the limitations in the process,
so that it may or may not even grow up to 10–50 GB in size. I don't have any specific
expectations as to what storage space it'll finalize at when it comes to the source code, but
I assume that it will end up at somewhere approximately 20 GB. Hopefully, not more. Oh, and
I don't want to rely on the `repo` tool developed by Google themselves, especially that AOSP
went into private development.

---

## Planned Release Strategy
Now, I might have an idea on how I could keep this OS up to date, while also having the mind on
keeping the OS secure. Here is the outline:

- **Feature Updates**: Released every year or two, similar to Windows' major version changes.
  Focused on introducing new capabilities, graphical updates and/or architectural changes.
- **Minor Updates**: Rolled out approximately every two months, in which it includes Security
  Updates, Bug Fixes, Translation and Device Support Improvements.
- **No forced monthly releases**: AnlinxOS prioritizes stability and long-term maintainability
  over rapid feature churn.

This way, I make sure that AnlinxOS has the stability in mind, contains the long-term
maintainability, and I also make sure that I keep myself sane (because someone eventually also
needs a break :skull:), so that updates are happening gradually instead of rapidly (looking
at Arch Linux over here). Now, I know you guys don't like the 2-to-3 monthly updates, but
acknowledging of me being a solo developer, I don't want to stress myself out completely.

---

## Future ideas
Okay, you might ask me, "Chris, why are you already planning ideas for the future?" Let uncle
Chris explain it. Basically, if you want to install this OS on an Android device, you need to
unlock the bootloader, which is common practice with custom ROMs in general. However, if the
OS isn't as bad as the other a THOUSAND unfinished projects of mine, I might consider making
devices themselves, partnering with people that are experts in making phones, and maybe even
Computers and/or Laptops in general. Again, that is a future idea I might do. However, if you
don't like this idea, I can drop it, and there won't be any problems with you guys bricking
five more Android devices, just because you tried installing AnlinxOS. Yes, I've been there,
bricked around five devices of my own. One of them was "I wanted Android 7" :skull: :laughing:
