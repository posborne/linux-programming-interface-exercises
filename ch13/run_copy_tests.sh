#! /bin/bash
echo '= build_10 ='
./tim.py ./build_10/prog_copy testfile.txt testfile2.txt

echo '= build_10_sync ='
./tim.py ./build_10_sync/prog_copy testfile.txt testfile2.txt

echo '= build_512 ='
./tim.py ./build_512/prog_copy testfile.txt testfile2.txt

echo '= build_512_sync ='
./tim.py ./build_512_sync/prog_copy testfile.txt testfile2.txt

echo '= build_512000 ='
./tim.py ./build_512000/prog_copy testfile.txt testfile2.txt

echo '= build_512000_sync ='
./tim.py ./build_512000_sync/prog_copy testfile.txt testfile2.txt

