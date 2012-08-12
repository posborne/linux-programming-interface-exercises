#!/usr/bin/env python
import time
import sys
import subprocess


def main():
    st = time.time()
    subprocess.call(" ".join(sys.argv[1:]), shell=True)
    print "time: %0.3fs" % (time.time() - st, )


if __name__ == '__main__':
    main()
