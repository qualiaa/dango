#dango

Minimal go server/client written in node.js and c++.

#Download

    git clone git@github.com:Qualiaa/dango.git
    cd dango
    git submodule init
    git submodule update

#Build

To build the client:

    mkdir build
    cmake [dango-root-dir]
    make

#Run

Run the client as a normal program

    ./dango

Server runs as a script

    node --use_strict --harmony server.js [board_size] [port]
