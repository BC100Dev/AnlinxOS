#!/bin/bash

# use cases: be able to install the built system to one of the followings:
# - iso: Packages the system, kernel and the LiveBoot contents into an ISO file
# - img: Builds the .img files only (system.img, bootld.img / init_boot.img, userdata.img)
# - self: Installs the system on the host (copies contents, prepares userdata (effectively replacing /Users for /home))