#!/bin/bash
# author:Tom
# Date:2016/01/20

#DIR_LIST=( "C_Project" "C++_Project" "Linux_Practice_Project" "Linux_Programming_EXP")
DIR_LIST=( "${PWD}" )

function Generate_Ctag_File() {
  echo "<<Create Tag database Started!>>"
  #create ctag data base
  for dir in ${DIR_LIST[@]}
  do
    cd $dir
    ctags -R .
    cd -
  done
  echo "<<Create Tag database Finished!>>"
}

function Clean_Ctag_File() {
  echo "<<Clean Tag database Started!>>"
  #create ctag data base
  for dir in ${DIR_LIST[@]}
  do
    cd $dir
    rm tags
    cd -
  done
  echo "<<Clean Tag database Finished!>>"
}


case "$1" in
  make)
  Generate_Ctag_File;
  ;;
  clean)
  Clean_Ctag_File;
  ;;
  *)
  echo "usage example $0 {make/clean}"
  ;;
esac




