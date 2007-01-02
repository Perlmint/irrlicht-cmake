#! /bin/sh
for i in [01]* Demo; do
  echo "Building $i";
  cd $i;
  make clean all;
  cd ..;
done
