#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    BitStream* BitStream_new_empty();

    BitStream* BitStream_new_data();

    void BitStream_drop(BitStream* self);

    void BitStream_Reset(BitStream *self);

    void BitStream_IgnoreBits(BitStream *self, const BitSize_t numberOfBits);

    void BitStream_SetWriteOffset(BitStream *self, const BitSize_t offset);

    BitSize_t BitStream_GetNumberOfBitsUsed(BitStream *self);
    BitSize_t BitStream_GetWriteOffset(BitStream *self);
    BitSize_t BitStream_GetReadOffset(BitStream *self);

    void BitStream_SetReadOffset(BitStream *self, const BitSize_t newReadOffset);
    BitSize_t BitStream_GetNumberOfUnreadBits(BitStream *self);

    unsigned char *BitStream_GetData(BitStream *self);

    void BitStream_WriteBits(BitStream *self, const unsigned char *inByteArray, BitSize_t numberOfBitsToWrite, const bool rightAlignedBits);

    void BitStream_AlignWriteToByteBoundary(BitStream *self);
    void BitStream_AlignReadToByteBoundary(BitStream *self);

    bool BitStream_ReadBits(BitStream *self, unsigned char *inOutByteArray, BitSize_t numberOfBitsToRead, const bool alignBitsToRight);

    void BitStream_Write0(BitStream *self);
    void BitStream_Write1(BitStream *self);
    bool BitStream_ReadBit(BitStream *self);

#ifdef __cplusplus
}
#endif