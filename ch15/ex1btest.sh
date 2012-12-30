#!/bin/bash
#
# Verify that on a directory with read but not execute permissions
# that the files can be listed but not accessed, regardless of
# permissions.
#
DIR="test"
if [ -d "$DIR" ]; then
    rm -rf "$DIR"
fi
mkdir "$DIR"
touch "$DIR/a" && echo "hello, world" > "$DIR/a" && chmod +rwx "$DIR/a"
touch "$DIR/b" && echo "hello, world" > "$DIR/b" && chmod +rwx "$DIR/b"
chmod +r "$DIR"
chmod -xw "$DIR"

ls -al "$DIR"
cat "$DIR/a"
cat "$DIR/b"