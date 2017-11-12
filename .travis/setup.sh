#! /bin/bash

# Boost install
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    brew update
    brew upgrade boost
else
    if [ -e $HOME/local/include/boost/process.hpp ]; then
        echo "using cache!"
    else
        echo "build boost..."
        wget -O boost_${BOOST_VERSION}.tar.gz "${BOOST_SRC_URL}"
        tar zxvf boost_${BOOST_VERSION}.tar.gz >> /dev/null
        cd boost_${BOOST_VERSION}
        ./bootstrap.sh
        ./b2 toolset="${CC}" --with-system --with-filesystem --with-program_options --prefix=$HOME/local install
    fi
fi
