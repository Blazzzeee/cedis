#!/usr/bin/env bash

echo "Starting build phase"
set -euo pipefail
SRC="server.c"
CLIENT_SRC="client.c"
DEBUG=${DEBUG:-0}
BIN=server
CLIENT_BIN=client

cc -O0 -Wall -Wextra -g -o "$BIN" $SRC 
cc -O0 -Wall -Wextra -g -o "$CLIENT_BIN" $CLIENT_SRC

echo "Compilation Successfull"

if [ "$DEBUG" -eq 0 ]; then
    echo "Starting server without debugger"
    ./"$BIN"
else
    echo "Starting server inside gdb"
    gdb --args ./"$BIN"
fi
