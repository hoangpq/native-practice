use std::os::raw::{c_int, c_uint, c_void};

use jni_sys::jobject;

#[repr(C)]
#[derive(Debug, Default)]
pub struct AndroidBitmapInfo {
    pub width: c_uint,
    pub height: c_uint,
    pub stride: c_uint,
    pub format: c_int,
    pub flags: c_uint,
}

impl Drop for AndroidBitmapInfo {
    fn drop(&mut self) {
        adb_debug!(format!("Drop bitmap info {:?}", self));
    }
}

impl AndroidBitmapInfo {
    pub fn new() -> Self {
        Self {
            ..Default::default()
        }
    }
}

extern "C" {
    #[link_name = "AndroidBitmap_getInfo"]
    pub fn bitmap_get_info(
        env: *mut jni_sys::JNIEnv,
        bmp: jobject,
        info: *mut AndroidBitmapInfo,
    ) -> c_int;

    #[link_name = "AndroidBitmap_lockPixels"]
    pub fn bitmap_lock_pixels(
        env: *mut jni_sys::JNIEnv,
        bmp: jobject,
        pixels: *mut *mut c_void,
    ) -> c_int;

    #[link_name = "AndroidBitmap_unlockPixels"]
    pub fn bitmap_unlock_pixels(env: *mut jni_sys::JNIEnv, bmp: jobject) -> c_int;
}
