extern crate cc;

use std::env;
use std::fs::File;
use std::io::Read;
use std::path::{Path, PathBuf};
use std::process::exit;

fn main() {
    let dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    /*println!(
        "cargo:rustc-link-search=native={}",
        Path::new(&dir).join("libnode/bin/x86").display()
    );*/
    println!("cargo:rustc-link-search=dylib={}", "node");

    let dst = PathBuf::from(env::var_os("OUT_DIR").unwrap());
    let build = dst.join("build");

    let mut cfg = cc::Build::new();

    cfg.out_dir(&build)
        .cpp(true)
        .flag_if_supported("-w")
        .flag_if_supported("-Wno-unused-parameter")
        .include("/Users/hoangpq/Library/Android/sdk/ndk-bundle/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include")
        .include("libnode/include/node")
        .include("build")
        .file("build/util/util.cpp")
        .file("build/v8_jni/wrapper.cpp")
        .file("build/api.cpp")
        .compile("api");
}
