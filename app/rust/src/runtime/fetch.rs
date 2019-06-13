use futures::Future;
use reqwest::r#async::Response;
use serde::Deserialize;

use crate::runtime::isolate::Isolate;
use crate::runtime::{ptr_to_string, string_to_ptr, Buf, DenoC, OpAsyncFuture};
use futures::stream::Stream;
use libc::{c_char, c_void};

extern "C" {
    fn resolve_promise(d: *const DenoC, promise_id: u32, data: *const c_char) -> *mut c_char;
}

lazy_static! {
    pub static ref CLIENT: reqwest::r#async::Client = reqwest::r#async::Client::new();
}

#[derive(Deserialize, Debug)]
pub struct User {
    name: String,
}

unsafe fn string_ptr_to_boxed_bytes(ptr: *mut c_char) -> Buf {
    let s = ptr_to_string(ptr).unwrap();
    string_to_boxed_bytes(s)
}

unsafe fn string_to_boxed_bytes(s: String) -> Buf {
    s.to_owned().into_boxed_str().into_boxed_bytes()
}

pub fn fetch_async(deno: *const DenoC, url: String, promise_id: u32) -> OpAsyncFuture {
    let raw = |res: Response| {
        res.into_body()
            .concat2()
            .map(|body| String::from_utf8(body.to_vec()).ok())
    };

    adb_debug!(format!("Send -> {}", url));
    let deno = unsafe { deno.as_ref() };

    Box::new(
        CLIENT
            .get(&url)
            .send()
            .and_then(raw)
            .and_then(move |body| unsafe {
                match body {
                    Some(body) => {
                        let deno = deno.unwrap();
                        let result = resolve_promise(deno, promise_id, string_to_ptr(body));
                        Ok(string_ptr_to_boxed_bytes(result))
                    }
                    None => Ok(string_to_boxed_bytes("{}".to_string())),
                }
            })
            .map_err(|e| adb_debug!(e)),
    )
}

#[no_mangle]
fn fetch(ptr: *const c_void, url: *mut c_char, promise_id: u32) {
    if let Some(url) = unsafe { ptr_to_string(url) } {
        let isolate = unsafe { Isolate::from_raw_ptr(ptr) };
        isolate
            .pending_ops
            .push(fetch_async(isolate.deno, url, promise_id));
    };
}
