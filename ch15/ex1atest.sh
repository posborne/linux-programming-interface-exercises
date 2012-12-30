#!/bin/bash
#
# Show that even if all, other, group have access to a file, but not
# user that access to the file is restricted.
#
F="test/ex1a_testfile.txt"
if [ -d "test" ]; then
    rm -rf test
fi
mkdir -p test
echo "foo" > "$F"
chmod a+rwx "$F"
chmod g+rwx "$F"
chmod o+rwx "$F"

chmod u-rwx "$F"
ls -al "$F"
cat "$F"
