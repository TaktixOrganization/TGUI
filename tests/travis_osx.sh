set -e
mkdir build

export SFML_VERSION=2.3.2
export SFML_ROOT=$HOME/SFML_OSX

if [[ ! -d "$SFML_ROOT/lib" ]]; then
  echo "$(tput setaf 3)Rebuilding SFML: no cache available$(tput sgr 0)"

  wget -O SFML.tar.gz https://github.com/SFML/SFML/archive/${SFML_VERSION}.tar.gz
  tar -xzf SFML.tar.gz
  cd SFML-${SFML_VERSION}
  cmake -DCMAKE_INSTALL_PREFIX=$SFML_ROOT -DCMAKE_INSTALL_FRAMEWORK_PREFIX=$SFML_ROOT/lib -DSFML_BUILD_FRAMEWORKS=TRUE .
  make -j2
  make install
  cd ..
else
  echo "$(tput setaf 2)Using cached SFML directory$(tput sgr 0)"
fi

cd build
cmake -DCMAKE_INSTALL_PREFIX=$HOME/TGUI_INSTALL -DSFML_INCLUDE_DIR=$SFML_ROOT/lib/SFML.framework -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ..
make -j2
make install
cd ..
