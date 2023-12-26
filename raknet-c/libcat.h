#pragma once

#if LIBCAT_SECURITY == 1

#ifdef __cplusplus
extern "C"
{
#endif
    static const int MAC_BYTES = 8;
    static const int IV_BYTES = 3;
    static const unsigned int OVERHEAD_BYTES = IV_BYTES + MAC_BYTES;

    CookieJar *CookieJar_new();
    void CookieJar_drop(CookieJar *self);

    AuthenticatedEncryption *AuthenticatedEncryption_new();
    bool AuthenticatedEncryption_GenerateProof(AuthenticatedEncryption *self, unsigned char *local_proof, int proof_bytes);
    bool AuthenticatedEncryption_ValidateProof(AuthenticatedEncryption *self, const unsigned char *remote_proof, int proof_bytes);
    bool AuthenticatedEncryption_Decrypt(AuthenticatedEncryption *self, unsigned char *buffer, unsigned int *buf_bytes);
    bool AuthenticatedEncryption_Encrypt(AuthenticatedEncryption *self, unsigned char *buffer, unsigned int buffer_bytes, unsigned int *msg_bytes);
    void AuthenticatedEncryption_drop(AuthenticatedEncryption *self);

    ClientEasyHandshake *ClientEasyHandshake_new();
    bool ClientEasyHandshake_GenerateServerKey(ClientEasyHandshake *self, void *out_public_key, void *out_private_key);
    bool ClientEasyHandshake_Initialize(ClientEasyHandshake *self, const void *in_public_key);

    bool ClientEasyHandshake_SetIdentity(ClientEasyHandshake *self, const void *in_public_key,
                                         const void *in_private_key);

    bool ClientEasyHandshake_GenerateChallenge(ClientEasyHandshake *self, void *out_challenge);

    bool ClientEasyHandshake_ProcessAnswer(ClientEasyHandshake *self, const void *in_answer,
                                           AuthenticatedEncryption *auth_enc);

    bool ClientEasyHandshake_ProcessAnswerWithIdentity(ClientEasyHandshake *self, const void *in_answer,
                                                       void *out_identity,
                                                       AuthenticatedEncryption *auth_enc);
    void ClientEasyHandshake_drop(ClientEasyHandshake *self);

    ServerEasyHandshake *ServerEasyHandshake_new();
    bool ServerEasyHandshake_GenerateServerKey(ServerEasyHandshake *self, void *out_public_key, void *out_private_key);
    void ServerEasyHandshake_FillCookieJar(ServerEasyHandshake *self, CookieJar *jar);
    bool ServerEasyHandshake_Initialize(ServerEasyHandshake *self, const void *in_public_key,
                                        const void *in_private_key);
    bool ServerEasyHandshake_ProcessChallenge(ServerEasyHandshake *self, const void *in_challenge,
                                              void *out_answer,
                                              AuthenticatedEncryption *auth_enc);
    bool ServerEasyHandshake_VerifyInitiatorIdentity(ServerEasyHandshake *self, const void *in_answer,
                                                     const void *in_proof,
                                                     void *out_public_key);
    void ServerEasyHandshake_drop(ServerEasyHandshake *self);

#ifdef __cplusplus
}
#endif

#endif