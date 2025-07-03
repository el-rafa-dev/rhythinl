#!/usr/bin/env bash

#if the rhythin extension exists, remove it
rm -f ./rhythin-*.vsix
# and then exec the vsce package to build the extension
exec vsce package