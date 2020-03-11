#!/usr/bin/env bash
source ~/.bash_profile
dir=`pwd`

NDK_HOME=/Users/hoangpq/contribute/android-ndk-r20b

export AARCH64LINUX_ANDROID_OPENSSL_INCLUDE_DIR="`pwd`/app/rust/openssl/include"
export AARCH64_LINUX_ANDROID_OPENSSL_LIB_DIR="`pwd`/app/rust/openssl/lib"
export AARCH64_LINUX_ANDROID_OPENSSL_DIR="`pwd`/app/rust/openssl"

export I686_LINUX_ANDROID_OPENSSL_INCLUDE_DIR="`pwd`/app/rust/openssl/include"
export I686_LINUX_ANDROID_OPENSSL_LIB_DIR="`pwd`/app/rust/openssl/lib/x86"
export I686_LINUX_ANDROID_OPENSSL_DIR="`pwd`/app/rust/openssl"

export ARMV7_LINUX_ANDROIDEABI_OPENSSL_INCLUDE_DIR="`pwd`/app/rust/openssl/include"
export ARMV7_LINUX_ANDROIDEABI_OPENSSL_LIB_DIR="`pwd`/app/rust/openssl/lib"
export ARMV7_LINUX_ANDROIDEABI_OPENSSL_DIR="`pwd`/app/rust/openssl"

NDK_STANDALONE=$HOME/ndk-standalone
export PATH="$PATH":"$NDK_STANDALONE/arm64/bin"
export PATH="$PATH":"$NDK_STANDALONE/arm/bin"
export PATH="$PATH":"$NDK_STANDALONE/x86/bin"

create_standalone_ndk() {
    rustup +nightly-2019-06-24 target add aarch64-linux-android armv7-linux-androideabi i686-linux-android
    rustup +nightly-2019-06-24 target add wasm32-unknown-unknown

    mkdir -p ${NDK_STANDALONE}
    ${NDK_HOME}/build/tools/make_standalone_toolchain.py --api 21 --arch arm64\
        --install-dir ${NDK_STANDALONE}/arm64
    ${NDK_HOME}/build/tools/make_standalone_toolchain.py --api 21 --arch arm\
        --install-dir ${NDK_STANDALONE}/arm
    ${NDK_HOME}/build/tools/make_standalone_toolchain.py --api 21 --arch x86\
        --install-dir ${NDK_STANDALONE}/x86
}
if ! [[ -d ${NDK_STANDALONE} ]]
then
    create_standalone_ndk
fi

# node ./gen-config.js
cd `pwd`/app/rust

# rm -f ./target/arm64-v8a/librust.a
# rm -f ./target/armeabi-v7a/librust.a
rm -f ./target/x86/librust.a

# RUST_BACKTRACE=1 cargo +nightly build --target aarch64-linux-android --release
# RUST_BACKTRACE=1 cargo build --target armv7-linux-androideabi --release
RUST_BACKTRACE=1 cargo +nightly-2019-06-24 build --target i686-linux-android

# mkdir -p ./target/arm64-v8a
# mkdir -p ./target/armeabi-v7a
mkdir -p ./target/x86

# cp ./target/aarch64-linux-android/release/librust.a ./target/arm64-v8a/librust.a
# cp ./target/armv7-linux-androideabi/release/librust.a ./target/armeabi-v7a/librust.a
cp ./target/i686-linux-android/debug/librust.a ./target/x86/librust.a
