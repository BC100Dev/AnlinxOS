# AnlinOS
**AnlinOS** is a custom Linux-based operating system from the ground up for both the mobile
(Android-based mobile devices) and desktop environments, taking the idea of LFS (Linux From
Scratch) into a fully customized idea. This operating system aims to merge the best parts
from other operating systems.

- Windows containing the Compatibility Layer
- Linux on the core itself (yes, it is a kernel)
- Android being virtualized on the Type-1 Layer (see `wine` and
  [`alde`](https://github.com/BC100Dev/alde))
  - Quick explanation, `alde` is similar to `wine` that translates Android calls to glibc
    calls. Initially, I archived it because it was no longer in development, thinking that
    I could reuse this concept in this OS. Maybe I'll unarchive that project, eventually
    copying the sources from here over to that repository, but only time will tell.

This will break quite many packages that would be ported, since there are ideas on refining
the FHS (File Hierarchy System) (`/`) itself. The idea behind it is to make the File Structure
more beginner-friendly. Certain directories like `/dev`, `/sys` and `/proc` will stay, but I
will mainly reconstruct the use of `/usr`, `/home` and other directories into Android
equivalents. Say, we got the System, I pack the System in a read-only `/System` partition. For
all the user files and such, we pack that into `/Data`, where the `/home` directory gets
replaced with a symlink of `/Users` that point towards `/Data/UserDir` (I got `/Data/Users`
reserved for something else). This itself already looks better. To allow Factory Resets that
Android does, we also implement a `/Recovery` partition. Oh, and `bin`, along with `lib` no
longer exists. Say hello to `/System/x64`. You can think of it as a System32-alike directory.

Now, I mentioned me having a reserved `/Data/Users`, and you may wonder, "Chris, what the hell
should that even be?" Don't worry, uncle BC100Dev here will explain. I decided to completely
nuke `/etc/passwd`, along with its `shadow` equivalent into its new User Management System,
which will end up being at that particular place. That itself is a directory, in which I will
have `login.d` (for users that can be logged in) and `services.d` (service-required users).
Something similar will also be placed under `/System/base/users`, where that would only contain
`services.d` and `system.d` (pun intended :laughing:)

In other words, you can see that AnlinOS is not another Linux distro fork (like Ubuntu being
linked to Debian), LineageOS being AOSP-equivalent and so on. Nope, AnlinOS is a
self-independent OS.

---

## Key Concepts
- **Built on customized CLFS**: This OS does not contain a repackaged distribution base, like
  ZorinOS, taking the distro base of Ubuntu. AnlinOS is constructed using CLFS (Coding Linux
  from Scratch) for complete control for each component.
- **Android Framework being included**: Unlike AOSP-based systems, AnlinOS does not depend on
  the Android Runtime, SystemUI or typical Android HALs. This means that the Android Framework
  Support is translated to the glibc Layer (see [`alde`](https://github.com/BC100Dev/alde) for
  reference).
- **Compatibility Layers**: The core idea is to get most programs to work with a compatibility
  layer, similar to how Windows operates, making older shared libraries still being present,
  even when they are no longer being up to date.
- **DE compatible with Mobile and Desktops**: A desktop environment, in which we know with
  GNOME and KDE Plasma, those are integral to a functional system for newbies. This OS will
  deliver a custom Graphical Environment that is capable for Mobile and Desktop systems, powered
  with the use of a Safety Protocol we all love, *Wayland*.
- **GSI-like experience**: Android in itself gives you the possibility to have a GSI (Generic
  System Image) on an Android device, meaning one ROM can support several devices. This OS
  takes that concept in developing a suitable solution, especially for those that are mainly
  afraid of unlocking the bootloader and possibly bricking the device in the process. Don't
  worry, you still need to unlock the bootloader :skull:

---

## Current Status
AnlinOS is currently under heavy development. Most components are in early implementation or
experimental testing. Code will remain hosted on GitHub during this stage, until I may have to
migrate to a self-hosted Git repository once the codebase grows or outpaces GitHub’s project
structure.

As of now, AnlinOS is currently in the planning stages and in heavy development. Most
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
- **No forced monthly releases**: AnlinOS prioritizes stability and long-term maintainability
  over rapid feature churn.

This way, I make sure that AnlinOS has the stability in mind, contains the long-term
maintainability, and I also make sure that I keep myself sane (because someone eventually also
needs a break :skull:), so that updates are happening gradually instead of rapidly (looking
at Arch Linux over here). Now, I know you guys don't like the 2-to-3 monthly updates, but
acknowledging of me being a solo developer, I don't want to stress myself out completely.