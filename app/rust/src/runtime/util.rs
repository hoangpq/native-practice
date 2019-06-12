use jni::JNIEnv;

use crate::runtime::isolate::Isolate;
use crate::runtime::{create_thread_pool_runtime, ptr_to_string};
use futures::future::Future;
use futures::stream::Stream;
use jni::objects::{JObject, JValue};
use std::ffi::CStr;
use std::os::raw::c_char;
use std::time::{Duration, Instant};
use tokio_timer::Interval;

// to link with C++
extern "C" {
    fn notify_message_to_main_thread(p: *mut c_char);
}

#[no_mangle]
pub unsafe extern "C" fn adb_debug(p: *mut c_char) {
    if let Some(msg) = ptr_to_string(p) {
        adb_debug!(msg);
        notify_message_to_main_thread(p);
    }
}

#[no_mangle]
pub unsafe extern "C" fn init_event_loop(_env: &'static JNIEnv) {
    let main_future = futures::lazy(move || {
        let mut isolate = Isolate::new();
        isolate.execute(
            r#"
                const users = ['hoangpq', 'firebase'];

                function fetchUserInfo(user) {
                    return fetch(`https://api.github.com/users/${user}`)
                        .then(resp => resp.json());
                }

                setInterval(() => {
                    console.log(`2s interval`);
                }, 2000);

                const start = Date.now();
                setTimeout(() => {
                    console.log(`timeout 10s: ${Date.now() - start}`);
                }, 10000);

                setTimeout(() => {
                    console.log(`timeout 3s: ${Date.now() - start}`);
                }, 3000);

                Promise.all(users.map(fetchUserInfo))
                    .then(data => {
                        const names = data.map(user => user.name).join(', ');
                        console.log(`Name: ${names}`);
                        console.log(`api call: ${Date.now() - start}`);
                    })
                    .catch(e => console.log(e.message));

                // fetch json api
                fetch('https://freejsonapi.com/posts')
                    .then(resp => resp.json())
                    .then(resp => {
                        console.log(resp.data.length);
                    })
                    .catch(e => console.log(e.message));
            "#,
        );
        isolate
    });

    let rt = create_thread_pool_runtime();
    rt.block_on_all(main_future).unwrap();
}
