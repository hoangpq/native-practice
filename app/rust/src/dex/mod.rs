use std::borrow::Cow;
use std::collections::HashMap;
use std::string::ToString;
use std::sync::Mutex;

use jni::errors::Result;
use jni::objects::{AutoLocal, GlobalRef, JClass, JObject, JString, JValue};
use jni::signature::TypeSignature;
use jni::strings::{JNIString, JavaStr};
use jni::JNIEnv;

use crate::ndk_util::jni_string_to_string;

extern "C" {
    fn throw_exception(data: *const u8, len: u32);
}

lazy_static! {
    static ref CLASS_TABLE: Mutex<HashMap<String, GlobalRef>> = Mutex::new(HashMap::new());
}

pub fn print_exception(env: &JNIEnv) {
    let exception_occurred = env.exception_check().unwrap_or_else(|e| panic!("{:?}", e));

    if exception_occurred {
        env.exception_describe()
            .unwrap_or_else(|e| panic!("{:?}", e));
    }
}

#[allow(dead_code)]
pub fn throw_js_exception(env: &JNIEnv, message: JValue) -> Result<()> {
    let message = JavaStr::from_env(env, JString::from(message.l()?))?;
    let message: Cow<str> = (&message).into();

    unsafe {
        throw_exception(message.as_ptr(), message.len() as u32);
    }

    Ok(())
}

#[allow(dead_code)]
pub fn unwrap<T>(env: &JNIEnv, res: Result<T>) -> T {
    res.unwrap_or_else(|e| {
        print_exception(&env);
        panic!(e)
    })
}

#[allow(dead_code)]
pub fn unwrap_js<T>(env: &JNIEnv, res: Result<T>) -> Option<T> {
    match res {
        Ok(result) => Some(result),
        Err(_) => None,
    }
}

#[no_mangle]
#[allow(non_snake_case)]
pub extern "C" fn Java_com_node_util_Util_createReference(
    env: JNIEnv,
    _class: JClass,
    class_name: JString,
) {
    let class_name = jni_string_to_string(&env, class_name);
    let mut table = CLASS_TABLE.lock().unwrap();

    if table.contains_key(&class_name) {
        adb_debug!(format!("Class {} already registered!", &class_name));
        return;
    }

    let atomic_ref = {
        let class = env.find_class(&class_name).unwrap();
        let local_ref = AutoLocal::new(
            &env,
            unwrap(
                &env,
                env.new_object(
                    "java/util/concurrent/atomic/AtomicReference",
                    "(Ljava/lang/Object;)V",
                    &[JValue::from(*class)],
                ),
            ),
        );
        unwrap(&env, env.new_global_ref(local_ref.as_obj()))
    };

    table.insert(class_name, atomic_ref);
}

pub fn call_static_method<'a, U, V>(
    env: &'a JNIEnv,
    class: &'a str,
    name: U,
    sig: V,
    args: &[JValue],
) -> Result<JValue<'a>>
where
    U: Into<JNIString>,
    V: Into<JNIString> + AsRef<str>,
{
    let table = CLASS_TABLE.lock().unwrap();
    let parsed = TypeSignature::from_str(&sig)?;
    let class_name = class.to_string();

    let class_ref = table.get(&class_name).unwrap();
    let value = env.call_method(class_ref.as_obj(), "get", "()Ljava/lang/Object;", &[])?;

    let class = JClass::from(value.l()?);
    env.call_static_method_unchecked(class, (class, name, sig), parsed.ret, args)
}

pub fn call_method<'a, U, V>(
    env: &'a JNIEnv,
    instance: JObject,
    name: U,
    sig: V,
    args: &[JValue],
) -> Result<JValue<'a>>
where
    U: Into<JNIString>,
    V: Into<JNIString> + AsRef<str>,
{
    let parsed = TypeSignature::from_str(&sig)?;

    let class = unwrap(
        &env,
        env.call_method(instance, "getClass", "()Ljava/lang/Class;", &[]),
    );

    let class = env.auto_local(unwrap(&env, class.l()));
    env.call_method_unchecked(instance, (&class, name, sig), parsed.ret, args)
}
