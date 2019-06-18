use std::os::raw::c_char;
use std::thread;

use crate::runtime::{create_thread_pool_runtime, ptr_to_string, Worker};

#[no_mangle]
pub unsafe extern "C" fn adb_debug(p: *mut c_char) {
    if let Some(msg) = ptr_to_string(p) {
        adb_debug!(msg);
    }
}

#[no_mangle]
pub unsafe extern "C" fn init_event_loop() {
    thread::spawn(move || {
        let main_future = futures::lazy(move || {
            let mut worker = Worker::new();
            worker.execute(
                r#"
                const users = ['hoangpq', 'firebase'];

                function fetchUserInfo(user) {
                    return fetch(`https://api.github.com/users/${user}`)
                        .then(resp => resp.json());
                }

                const i2s = setInterval(() => {
                    console.log(`2s interval`);
                }, 2000);

                const start = Date.now();
                setTimeout(() => {
                    console.log(`timeout 5s: ${Date.now() - start}`);
                    // clearTimer(i2s);

                    setInterval(() => {
                        console.log(`1s interval`);
                    }, 1000);

                }, 5000);

                setTimeout(() => {
                    console.log(`timeout 3s: ${Date.now() - start}`);
                }, 3000);

                Promise.all(users.map(fetchUserInfo))
                    .then(data => {
                        const names = data.map(user => user.name).join(', ');
                        console.log(`Name: ${names}`);
                        $toast(`Name: ${names}`);
                        console.log(`api call: ${Date.now() - start}`);
                    })
                    .catch(e => console.log(e.message));

                // fetch json api
                fetch('https://freejsonapi.com/posts')
                    .then(resp => resp.json())
                    .then(resp => {
                        console.log(`Total: ${resp.data.length}`);
                        $toast(`Total: ${resp.data.length}`);
                    })
                    .catch(e => console.log(e.message));
            "#,
            );
            worker
        });

        // let rt = create_thread_pool_runtime();
        // rt.block_on_all(main_future).unwrap();
        tokio::runtime::current_thread::run(main_future);
    });
}
