use std::ffi::CStr;
use std::os::raw::c_char;

use jni_sys::jlong;

mod util;

#[no_mangle]
pub extern "C" fn _rust_new_string(data: *const c_char) -> jlong {
    let slice = unsafe { CStr::from_ptr(data) };
    let data = slice.to_string_lossy().into_owned();
    Box::into_raw(Box::new(data.clone())) as jlong
}

#[no_mangle]
pub extern "C" fn _rust_get_string(ptr: jlong) -> String {
    let data = unsafe { Box::from_raw(ptr as *mut String) };
    data.to_string()
}
