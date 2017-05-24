#!/bin/sh
PREFIX="$1"
if [ "$PREFIX" ]; then
  export CPPFLAGS="${CPPFLAGS} -I \"${PREFIX}/include\""
  export LDFLAGS="${LDFLAGS} -L \"${PREFIX}/lib\" -Wl,-rpath -Wl,\"${PREFIX}/lib\""
fi
make
