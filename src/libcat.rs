use std::io::Write;

use crate::ffi;

pub const PRIVATE_KEY_BYTES: usize = ffi::PRIVATE_KEY_BYTES as usize;
pub const PUBLIC_KEY_BYTES: usize = ffi::PUBLIC_KEY_BYTES as usize;
pub const CHALLENGE_BYTES: usize = ffi::CHALLENGE_BYTES as usize;
pub const ANSWER_BYTES: usize = ffi::ANSWER_BYTES as usize;

pub const OVERHEAD_BYTES: usize = ffi::OVERHEAD_BYTES as usize;

pub struct CookieJar {
    inner: *mut ffi::CookieJar,
}
unsafe impl Sync for CookieJar {}
unsafe impl Send for CookieJar {}

impl CookieJar {
    pub fn new() -> Self {
        Self {
            inner: unsafe { ffi::CookieJar_new() },
        }
    }
}

impl Drop for CookieJar {
    fn drop(&mut self) {
        unsafe { ffi::CookieJar_drop(self.inner) }
    }
}

pub struct AuthenticatedEncryption {
    inner: *mut ffi::AuthenticatedEncryption,
}
unsafe impl Sync for AuthenticatedEncryption {}
unsafe impl Send for AuthenticatedEncryption {}

impl AuthenticatedEncryption {
    pub fn new() -> Self {
        Self {
            inner: unsafe { ffi::AuthenticatedEncryption_new() },
        }
    }

    pub fn GenerateProof(&mut self, length: usize) -> Option<Vec<u8>> {
        let mut v: Vec<u8> = vec![0; length];

        let res = unsafe {
            ffi::AuthenticatedEncryption_GenerateProof(self.inner, v.as_mut_ptr(), v.len() as i32)
        };

        if res {
            Some(v)
        } else {
            None
        }
    }

    pub fn ValidateProof(&mut self, proof: &[u8]) -> bool {
        unsafe {
            ffi::AuthenticatedEncryption_ValidateProof(
                self.inner,
                proof.as_ptr(),
                proof.len() as i32,
            )
        }
    }

    pub fn Encrypt(&mut self, msg: &[u8]) -> Option<Vec<u8>> {
        let mut v: Vec<u8> = Vec::with_capacity(msg.len() + OVERHEAD_BYTES);
        v.write(msg).unwrap();
        v.write(&[0; OVERHEAD_BYTES]).unwrap();

        let mut msg_bytes = msg.len() as u32;

        let res = unsafe {
            ffi::AuthenticatedEncryption_Encrypt(
                self.inner,
                v.as_mut_ptr(),
                v.len() as u32,
                &mut msg_bytes,
            )
        };

        if res {
            v.resize(msg_bytes as usize, 0);
            Some(v)
        } else {
            None
        }
    }

    pub fn Decrypt(&mut self, msg: &[u8]) -> Option<Vec<u8>> {
        let mut v = msg.to_vec();
        let mut msg_bytes = v.len() as u32;

        let res = unsafe {
            ffi::AuthenticatedEncryption_Decrypt(self.inner, v.as_mut_ptr(), &mut msg_bytes)
        };

        if res {
            v.resize(msg_bytes as usize, 0);
            Some(v)
        } else {
            None
        }
    }
}

impl Drop for AuthenticatedEncryption {
    fn drop(&mut self) {
        unsafe { ffi::AuthenticatedEncryption_drop(self.inner) }
    }
}

pub struct ClientEasyHandshake {
    inner: *mut ffi::ClientEasyHandshake,
}
unsafe impl Sync for ClientEasyHandshake {}
unsafe impl Send for ClientEasyHandshake {}

impl ClientEasyHandshake {
    pub fn new() -> Self {
        Self {
            inner: unsafe { ffi::ClientEasyHandshake_new() },
        }
    }

    pub fn GenerateServerKey(
        &mut self,
    ) -> Option<([u8; PUBLIC_KEY_BYTES], [u8; PRIVATE_KEY_BYTES])> {
        let mut public_key = [0; PUBLIC_KEY_BYTES];
        let mut private_key = [0; PRIVATE_KEY_BYTES];

        let res = unsafe {
            ffi::ClientEasyHandshake_GenerateServerKey(
                self.inner,
                public_key.as_mut_ptr() as _,
                private_key.as_mut_ptr() as _,
            )
        };

        if res {
            Some((public_key, private_key))
        } else {
            None
        }
    }

    pub fn Initialize(&mut self, public_key: &[u8; PUBLIC_KEY_BYTES]) -> bool {
        unsafe { ffi::ClientEasyHandshake_Initialize(self.inner, public_key.as_ptr() as _) }
    }

    pub fn SetIdentity(
        &mut self,
        public_key: &[u8; PUBLIC_KEY_BYTES],
        private_key: &[u8; PRIVATE_KEY_BYTES],
    ) -> bool {
        unsafe {
            ffi::ClientEasyHandshake_SetIdentity(
                self.inner,
                public_key.as_ptr() as _,
                private_key.as_ptr() as _,
            )
        }
    }

    pub fn GenerateChallenge(&mut self) -> Option<[u8; CHALLENGE_BYTES]> {
        let mut challenge: [u8; CHALLENGE_BYTES] = [0; CHALLENGE_BYTES];
        let res = unsafe {
            ffi::ClientEasyHandshake_GenerateChallenge(
                self.inner,
                (&mut challenge).as_mut_ptr() as _,
            )
        };

        if res {
            Some(challenge)
        } else {
            None
        }
    }

    pub fn ProcessAnswer(
        &mut self,
        answer: &[u8; ANSWER_BYTES],
    ) -> Option<AuthenticatedEncryption> {
        let authenticated_encryption = AuthenticatedEncryption::new();
        let res = unsafe {
            ffi::ClientEasyHandshake_ProcessAnswer(
                self.inner,
                answer.as_ptr() as _,
                authenticated_encryption.inner,
            )
        };

        if res {
            Some(authenticated_encryption)
        } else {
            None
        }
    }
}

impl Drop for ClientEasyHandshake {
    fn drop(&mut self) {
        unsafe { ffi::ClientEasyHandshake_drop(self.inner) }
    }
}

pub struct ServerEasyHandshake {
    _cookie_jar: CookieJar,
    inner: *mut ffi::ServerEasyHandshake,
}
unsafe impl Sync for ServerEasyHandshake {}
unsafe impl Send for ServerEasyHandshake {}

impl ServerEasyHandshake {
    pub fn new() -> Self {
        let inner = unsafe { ffi::ServerEasyHandshake_new() };
        let cookie_jar = CookieJar::new();

        unsafe {
            ffi::ServerEasyHandshake_FillCookieJar(inner, cookie_jar.inner);
        }

        Self {
            _cookie_jar: cookie_jar,
            inner,
        }
    }

    pub fn GenerateServerKey(
        &mut self,
    ) -> Option<([u8; PUBLIC_KEY_BYTES], [u8; PRIVATE_KEY_BYTES])> {
        let mut public_key = [0; PUBLIC_KEY_BYTES];
        let mut private_key = [0; PRIVATE_KEY_BYTES];

        let res = unsafe {
            ffi::ServerEasyHandshake_GenerateServerKey(
                self.inner,
                public_key.as_mut_ptr() as _,
                private_key.as_mut_ptr() as _,
            )
        };

        if res {
            Some((public_key, private_key))
        } else {
            None
        }
    }

    pub fn Initialize(
        &mut self,
        public_key: &[u8; PUBLIC_KEY_BYTES],
        private_key: &[u8; PRIVATE_KEY_BYTES],
    ) -> bool {
        unsafe {
            ffi::ServerEasyHandshake_Initialize(
                self.inner,
                public_key.as_ptr() as _,
                private_key.as_ptr() as _,
            )
        }
    }

    pub fn ProcessChallenge(
        &mut self,
        challenge: &[u8; CHALLENGE_BYTES],
    ) -> Option<(AuthenticatedEncryption, [u8; ANSWER_BYTES])> {
        let mut answer: [u8; ANSWER_BYTES] = [0; ANSWER_BYTES];
        let authenticated_encryption = AuthenticatedEncryption::new();

        let res = unsafe {
            ffi::ServerEasyHandshake_ProcessChallenge(
                self.inner,
                challenge.as_ptr() as _,
                answer.as_mut_ptr() as _,
                authenticated_encryption.inner,
            )
        };

        if res {
            Some((authenticated_encryption, answer))
        } else {
            None
        }
    }
}

impl Drop for ServerEasyHandshake {
    fn drop(&mut self) {
        unsafe { ffi::ServerEasyHandshake_drop(self.inner) }
    }
}
