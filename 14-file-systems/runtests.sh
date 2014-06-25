#!/bin/bash

echo ""
echo "** Tests with ext4 FS **"
./prog_benchmarkwrites -n 100000 tst
echo ""
./prog_benchmarkwrites -n 100000 -r tst
echo ""

echo ""
echo "** Tests with tmpfs FS **"
./prog_benchmarkwrites -n 100000 tmpfs/tst
echo ""
./prog_benchmarkwrites -n 100000 -r tmpfs/tst
echo ""

echo ""
echo "** Tests with ntfs4 FS **"
./prog_benchmarkwrites -n 100000 /media/ssd/ntfstst
echo ""
./prog_benchmarkwrites -n 100000 -r /media/ssd/ntfstst
