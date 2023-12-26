#include <RakNet/BitStream.h>

using namespace RakNet;

BitStream *BitStream_new_empty()
{
    return new BitStream();
}

BitStream *BitStream_new_data()
{
}

void BitStream_drop(BitStream *self)
{
    delete self;
}

void BitStream_Reset(BitStream *self)
{
    self->Reset();
}

void BitStream_IgnoreBits(BitStream *self, const BitSize_t numberOfBits)
{
    self->IgnoreBits(numberOfBits);
}

void BitStream_SetWriteOffset(BitStream *self, const BitSize_t offset)
{
    self->SetWriteOffset(offset);
}

BitSize_t BitStream_GetNumberOfBitsUsed(BitStream *self)
{
    return self->GetNumberOfBitsUsed();
}
BitSize_t BitStream_GetWriteOffset(BitStream *self)
{
    return self->GetWriteOffset();
}
BitSize_t BitStream_GetReadOffset(BitStream *self)
{
    return self->GetReadOffset();
}

void BitStream_SetReadOffset(BitStream *self, const BitSize_t newReadOffset)
{
    self->SetReadOffset(newReadOffset);
}
BitSize_t BitStream_GetNumberOfUnreadBits(BitStream *self)
{
    return self->GetNumberOfUnreadBits();
}

unsigned char *BitStream_GetData(BitStream *self)
{
    return self->GetData();
}

void BitStream_WriteBits(BitStream *self, const unsigned char *inByteArray, BitSize_t numberOfBitsToWrite, const bool rightAlignedBits)
{
    return self->WriteBits(inByteArray, numberOfBitsToWrite, rightAlignedBits);
}

void BitStream_AlignWriteToByteBoundary(BitStream *self)
{
    self->AlignWriteToByteBoundary();
}
void BitStream_AlignReadToByteBoundary(BitStream *self)
{
    self->AlignReadToByteBoundary();
}

bool BitStream_ReadBits(BitStream *self, unsigned char *inOutByteArray, BitSize_t numberOfBitsToRead, const bool alignBitsToRight)
{
    return self->ReadBits(inOutByteArray, numberOfBitsToRead, alignBitsToRight);
}

void BitStream_Write0(BitStream *self)
{
    self->Write0();
}
void BitStream_Write1(BitStream *self)
{
    self->Write1();
}
bool BitStream_ReadBit(BitStream *self)
{
    self->ReadBit();
}