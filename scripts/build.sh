#!/bin/bash


script_dir=`readlink -f ${0%/*}`
current_dir=`pwd`

build_dir="build-stm32"

gcc_cortex_m_dir=/opt/launchpad/gcc-arm-none-eabi-cortex-m
gcc_cross_prefix="arm-none-eabi"

if [ ! -f "$gcc_cortex_m_dir/bin/$gcc_cross_prefix-gcc" ]; then
    echo " not found  $gcc_cortex_m_dir/bin/$gcc_cross_prefix-gcc"
    exit 1
fi

export PATH=$gcc_cortex_m_dir/bin:$PATH

host="${gcc_cross_prefix}"
build="x86_64-linux"
configure_arg="--build=$build --host=$host --enable-static "

export CFLAGS="$CFLAGS -mtune=cortex-m4  -mfloat-abi=hard -mfpu=fpv4-sp-d16 -nostdlib -nostartfiles -nodefaultlibs -ffreestanding "

mkdir -p ../$build_dir

cd ..

autoreconf -vif
if [ $? -ne 0 ]; then
  echo "autoreconf error"
  cd $script_dir
  exit 1
fi

cd $build_dir

echo "../configure ${configure_arg}"
../configure ${configure_arg}
if [ $? -ne 0 ]; then
  echo "configure error"
  cd $script_dir
  exit 1
fi


make
if [ $? -ne 0 ]; then
  echo "make error"
  cd $script_dir
  exit 1
fi

cd $script_dir



