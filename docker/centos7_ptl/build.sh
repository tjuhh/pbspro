#!/bin/bash
cd /home/pbsuser/pbspro
export PBS_INSTALL_DIR=/opt/pbs
cd /src/pbspro
./autogen.sh
./configure --prefix=$PBS_INSTALL_DIR --enable-ptl
make -j 7
