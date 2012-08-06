#! /bin/bash
echo '= build_10 ='
time ./build_10/prog_copy testfile.txt testfile2.txt

echo '= build_10_sync ='
time ./build_10_sync/prog_copy testfile.txt testfile2.txt

echo '= build_512 ='
time ./build_512/prog_copy testfile.txt testfile2.txt

echo '= build_512_sync ='
time ./build_512_sync/prog_copy testfile.txt testfile2.txt

echo '= build_512000 ='
time ./build_512000/prog_copy testfile.txt testfile2.txt

echo '= build_512000_sync ='
time ./build_512000_sync/prog_copy testfile.txt testfile2.txt

