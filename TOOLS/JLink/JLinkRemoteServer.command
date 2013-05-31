#!/bin/sh

#
# Configure the path to the JLink library.
#
export DYLD_LIBRARY_PATH=.:/opt/local/lib:$DYLD_LIBRARY_PATH

#
# Change to current directory.
#
cd $(dirname $0)

#
# Get the Mac OS X version.
#
MACOSX_VERSION=$(sw_vers -productVersion | cut -f 1-2 -d '.')

#
# On Leopard, start the 32-bit version.
#
if test "${MACOSX_VERSION}" == "10.5"
then
  arch -i386 ./JLinkRemoteServer "$@"
else
  ./JLinkRemoteServer "$@"
fi

