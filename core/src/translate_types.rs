//! Shared session ABI types, without HTTP clients or a local model runner.
//! The Linux text-only frontend never enters translation mode. These types
//! keep the session ABI usable by existing clients and deterministic tests.

#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub enum JobState {
    #[default]
    Idle,
    Running,
    Done,
    Error,
}

#[derive(Clone, Debug, Default, PartialEq, Eq)]
pub struct Snapshot {
    pub state: JobState,
    pub source: String,
    pub text: String,
    pub error: String,
}
