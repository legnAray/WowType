//! Inert translation ABI for text-only builds. No threads, subprocesses,
//! network requests, model loading or allocations are started by these calls.

#![allow(clippy::missing_safety_doc)]

use std::os::raw::{c_char, c_int, c_uint};

pub struct WowtypeTranslator;
static EMPTY: [u8; 1] = [0];

#[no_mangle]
pub extern "C" fn wowtype_translate_new() -> *mut WowtypeTranslator {
    std::ptr::null_mut()
}

#[no_mangle]
pub unsafe extern "C" fn wowtype_translate_free(_handle: *mut WowtypeTranslator) {}

#[no_mangle]
pub unsafe extern "C" fn wowtype_translate_configure(
    _handle: *mut WowtypeTranslator,
    _base_url: *const c_char,
    _model: *const c_char,
    _target: *const c_char,
    _formal: c_int,
    _key_file: *const c_char,
) {
}

#[no_mangle]
pub unsafe extern "C" fn wowtype_translate_configure_local(
    _handle: *mut WowtypeTranslator,
    _model: *const c_char,
    _server: *const c_char,
    _idle_secs: c_uint,
) {
}

#[no_mangle]
pub unsafe extern "C" fn wowtype_translate_start(
    _handle: *mut WowtypeTranslator,
    _text: *const c_char,
) {
}

#[no_mangle]
pub unsafe extern "C" fn wowtype_translate_cancel(_handle: *mut WowtypeTranslator) {}

#[no_mangle]
pub unsafe extern "C" fn wowtype_translate_ready(_handle: *const WowtypeTranslator) -> c_int {
    0
}

#[no_mangle]
pub unsafe extern "C" fn wowtype_translate_local_running(
    _handle: *const WowtypeTranslator,
) -> c_int {
    0
}

#[no_mangle]
pub unsafe extern "C" fn wowtype_translate_poll(_handle: *mut WowtypeTranslator) -> c_int {
    0
}

#[no_mangle]
pub unsafe extern "C" fn wowtype_translate_text(
    _handle: *const WowtypeTranslator,
) -> *const c_char {
    EMPTY.as_ptr().cast()
}

#[no_mangle]
pub unsafe extern "C" fn wowtype_translate_source(
    _handle: *const WowtypeTranslator,
) -> *const c_char {
    EMPTY.as_ptr().cast()
}

#[no_mangle]
pub unsafe extern "C" fn wowtype_translate_error(
    _handle: *const WowtypeTranslator,
) -> *const c_char {
    EMPTY.as_ptr().cast()
}

#[no_mangle]
pub unsafe extern "C" fn wowtype_translate_tag(
    _handle: *mut WowtypeTranslator,
    _text: *const c_char,
) -> *const c_char {
    EMPTY.as_ptr().cast()
}
