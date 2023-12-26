#include <RakNet/NativeFeatureIncludes.h>

#if LIBCAT_SECURITY == 1
#include <RakNet/SecureHandshake.h>

using namespace cat;

#include "libcat.h"

CookieJar *CookieJar_new()
{
    return new CookieJar();
}
void CookieJar_drop(CookieJar *self)
{
    delete self;
}

AuthenticatedEncryption *AuthenticatedEncryption_new()
{
    return new AuthenticatedEncryption();
}

bool AuthenticatedEncryption_GenerateProof(AuthenticatedEncryption *self, unsigned char *local_proof, int proof_bytes)
{
    return self->GenerateProof(local_proof, proof_bytes);
}

bool AuthenticatedEncryption_ValidateProof(AuthenticatedEncryption *self, const unsigned char *remote_proof, int proof_bytes)
{
    return self->ValidateProof(remote_proof, proof_bytes);
}

bool AuthenticatedEncryption_Decrypt(AuthenticatedEncryption *self, unsigned char *buffer, unsigned int *buf_bytes)
{
    return self->Decrypt(buffer, *buf_bytes);
}
bool AuthenticatedEncryption_Encrypt(AuthenticatedEncryption *self, unsigned char *buffer, unsigned int buffer_bytes, unsigned int *msg_bytes)
{
    return self->Encrypt(buffer, buffer_bytes, *msg_bytes);
}
void AuthenticatedEncryption_drop(AuthenticatedEncryption *self)
{
    delete self;
}

ClientEasyHandshake *ClientEasyHandshake_new()
{
    return new ClientEasyHandshake();
}
bool ClientEasyHandshake_GenerateServerKey(ClientEasyHandshake *self, void *out_public_key, void *out_private_key)
{
    return self->GenerateServerKey(out_public_key, out_private_key);
}
bool ClientEasyHandshake_Initialize(ClientEasyHandshake *self, const void *in_public_key)
{
    return self->Initialize(in_public_key);
}

bool ClientEasyHandshake_SetIdentity(ClientEasyHandshake *self, const void *in_public_key,
                                     const void *in_private_key)
{
    return self->SetIdentity(in_public_key, in_private_key);
}

bool ClientEasyHandshake_GenerateChallenge(ClientEasyHandshake *self, void *out_challenge)
{
    return self->GenerateChallenge(out_challenge);
}

bool ClientEasyHandshake_ProcessAnswer(ClientEasyHandshake *self, const void *in_answer,
                                       AuthenticatedEncryption *auth_enc)
{
    return self->ProcessAnswer(in_answer, auth_enc);
}

bool ClientEasyHandshake_ProcessAnswerWithIdentity(ClientEasyHandshake *self, const void *in_answer,
                                                   void *out_identity,
                                                   AuthenticatedEncryption *auth_enc)
{
    return self->ProcessAnswerWithIdentity(in_answer, out_identity, auth_enc);
}
void ClientEasyHandshake_drop(ClientEasyHandshake *self)
{
    delete self;
}

ServerEasyHandshake *ServerEasyHandshake_new()
{
    return new ServerEasyHandshake();
}
bool ServerEasyHandshake_GenerateServerKey(ServerEasyHandshake *self, void *out_public_key, void *out_private_key)
{
    return self->GenerateServerKey(out_public_key, out_private_key);
}
void ServerEasyHandshake_FillCookieJar(ServerEasyHandshake *self, CookieJar *jar)
{
    self->FillCookieJar(jar);
}
bool ServerEasyHandshake_Initialize(ServerEasyHandshake *self, const void *in_public_key,
                                    const void *in_private_key)
{
    return self->Initialize(in_public_key, in_private_key);
}
bool ServerEasyHandshake_ProcessChallenge(ServerEasyHandshake *self, const void *in_challenge,
                                          void *out_answer,
                                          AuthenticatedEncryption *auth_enc)
{
    return self->ProcessChallenge(in_challenge, out_answer, auth_enc);
}
bool ServerEasyHandshake_VerifyInitiatorIdentity(ServerEasyHandshake *self, const void *in_answer,
                                                 const void *in_proof,
                                                 void *out_public_key)
{
    return self->VerifyInitiatorIdentity(in_answer, in_proof, out_public_key);
}
void ServerEasyHandshake_drop(ServerEasyHandshake *self)
{
    delete self;
}
#endif