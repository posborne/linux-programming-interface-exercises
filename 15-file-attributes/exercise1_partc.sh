#!/bin/bash
function run_test_on {
    local dir="test/$1"
    local dirt="test/$1t"
    echo "== +$1, -$2 =="

    # Create the directory with the right perms and
    # setup therewithin
    mkdir "$dir"
    mkdir "$dirt"
    for i in {1..20}; do
	echo "test" > "$dir/mv$i"
    done
    echo "test" > "$dir/f"
    echo "test" > "$dir/f2"
    echo "test" > "$dir/f3"
    echo "test" > "$dirt/f"
    chmod "u+$1" "$dir"
    chmod "u+$1" "$dirt"
    chmod "+t" "$dirt"
    chmod "u-$2" "$dir"
    chmod "u-$2" "$dirt"

    # Test creation of file within directory
    touch "$dir/a" > /dev/null 2>&1
    if [[ "$?" == "0" ]]; then
	echo "   create: PASS"
    else
	echo "   create: ----"
    fi

    # Test Read
    echo "$dir/f" > /dev/null 2>&1
    if [[ "$?" == "0" ]]; then
	echo "   read:   PASS"
    else
	echo "   read:   ----"
    fi

    # Test Write
    (echo "test" >> "$dir/f") > /dev/null 2>&1
    if [[ "$?" == "0" ]]; then
	echo "   write:  PASS"
    else
	echo "   write:  ----"
    fi

    # Test Remove
    rm "$dir/f" > /dev/null 2>&1
    if [[ "$?" == "0" ]]; then
	echo "   delete: PASS"
    else
	echo "   delete: ----"
    fi

    # Test Remove Sticky
    rm "$dirt/f" > /dev/null 2>&1
    if [[ "$?" == "0" ]]; then
	echo "   delt:   PASS"
    else
	echo "   delt:   ----"
    fi

}

function run_rename_tests {
    local rootdir="test"
    
    echo "== Rename Tests =="
    # iterate over all combinations
    set -- `find test -maxdepth 1 -mindepth 1 -type d -exec basename '{}' \;`
    local srcfile=""
    local dstfile=""
    for srcdir; do
	shift
	for dstdir; do
	    for i in {1..20}; do
		# find source/dest that should work
		if [ -f "test/$srcdir/mv$i" ]; then
		    srcfile="$srcdir/mv$i"
		    break
		fi
	    done
	    for i in {1..20}; do
		if [ ! -f "test/$dsstdir/mvdst$i" ]; then
		    dstfile="$dstdir/mvdst$i"
		    break
		fi
	    done
	    (mv "test/$srcfile" "test/$dstfile") > /dev/null 2>&1
	    if [[ "$?" == "0" ]]; then
		echo -ne "   $srcdir -> $dstdir:\t\tPASS\n"
	    else
		echo -ne "   $srcdir -> $dstdir:\t\t----\n"
	    fi
        done
    done
}

if [ -d test ]; then
    rm -rf test
fi
mkdir test

run_test_on "rwx" ""
run_test_on "rw"  "x"
run_test_on "rx"  "w"
run_test_on "xw"  "r"
run_test_on "x"   "rw"
run_test_on "w"   "rx"
run_test_on "r"   "wx"
run_rename_tests
