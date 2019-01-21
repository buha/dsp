import struct
import numpy as np

def wav_write_float(data, rate, wsize, sf):
    N = len(data)
    byte_count = N * wsize
    wav = b''
    wav += struct.pack('<ccccIccccccccIHHIIHH',
        b'R', b'I', b'F', b'F',
        byte_count + 0x2c - 8,  # header size
        b'W', b'A', b'V', b'E', b'f', b'm', b't', b' ',
        0x10,  # size of 'fmt ' header
        3,  # format 3 = IEEE floating-point
        1,  # channels
        int(rate),  # samples / second
        int(rate) * wsize,  # bytes / second
        wsize,  # block alignment
        8 * wsize)  # bits / sample
    wav += struct.pack('<ccccI',
        b'd', b'a', b't', b'a', byte_count)
    for d in data:
        wav += struct.pack(sf, d)
    return wav

def wav_write_int(data, rate, wsize, sf):
    N = len(data)
    byte_count = N * wsize
    wav = b''
    wav += struct.pack('<ccccIccccccccIHHIIHH',
        b'R', b'I', b'F', b'F',
        byte_count + 0x2c - 8,  # header size
        b'W', b'A', b'V', b'E', b'f', b'm', b't', b' ',
        0x10,  # size of 'fmt ' header
        1,  # format 1 = PCM
        1,  # channels
        int(rate),  # samples / second
        int(rate) * wsize,  # bytes / second
        wsize,  # block alignment
        8 * wsize)  # bits / sample
    wav += struct.pack('<ccccI',
        b'd', b'a', b't', b'a', byte_count)
    for d in data:
        wav += struct.pack(sf, int(d))
    return wav

def wav_read(filename, verbose=False):
    fi=open(filename,'rb')
    data=fi.read()
    fi.close()
    A, B, C, D    = struct.unpack('4c', data[0:4])      # 'RIFF'
    ChunkSize     = struct.unpack('<l', data[4:8])[0]   #4+(8+SubChunk1Size)+8+SubChunk2Size)
    A, B, C, D    = struct.unpack('4c', data[8:12])     # 'WAVE'
    A, B, C, D    = struct.unpack('4c', data[12:16])    # 'fmt '
    Subchunk1Size = struct.unpack('<l', data[16:20])[0] # LITTLE ENDIAN, long, 16
    AudioFormat   = struct.unpack('<h', data[20:22])[0] # LITTLE ENDIAN, short, 1
    NumChannels   = struct.unpack('<h', data[22:24])[0] # LITTLE ENDIAN, short, Mono = 1, Stereo = 2
    SampleRate    = struct.unpack('<l', data[24:28])[0] # LITTLE ENDIAN, long,  sample rate in samples per second
    ByteRate      = struct.unpack('<l', data[28:32])[0] # self.SampleRate * self.NumChannels * self.BitsPerSample/8)) # (ByteRate) LITTLE ENDIAN, long
    BlockAlign    = struct.unpack('<h', data[32:34])[0] # self.NumChannels * self.BitsPerSample/8))  # (BlockAlign) LITTLE ENDIAN, short
    BitsPerSample = struct.unpack('<h', data[34:36])[0] # LITTLE ENDIAN, short
    A, B, C, D    = struct.unpack('4c', data[36:40])    # BIG ENDIAN, char*4
    SubChunk2Size = struct.unpack('<l', data[40:44])[0] # LITTLE ENDIAN, long
    waveData=data[44:]
    
    print("%s\n\tChunkSize     = %d\n\tSubchunk1Size = %d\n\tAudioFormat   = %d\n\tNumChannels   = %d\n\tSampleRate    = %d\n\tByteRate      = %d\n\tBlockAlign    = %d\n\tBitsPerSample = %d\n\tSubChunk2Size = %d" %
        (filename,
        ChunkSize,
        Subchunk1Size, 
        AudioFormat,
        NumChannels,
        SampleRate,
        ByteRate,
        BlockAlign,
        BitsPerSample,
        SubChunk2Size)) if verbose else 0

    if BitsPerSample == 8:
        print("{}: Unpacking {} bytes as 8 bits integer.".format(filename, len(waveData))) if verbose else 0
        d = np.frombuffer(waveData, np.uint8)
    elif BitsPerSample == 16:
        print("{}: Unpacking {} bytes as 16 bits integer.".format(filename, len(waveData))) if verbose else 0
        d = np.zeros(int(SubChunk2Size / 2), dtype=np.int16)
        j = 0
        for k in range(0, SubChunk2Size, 2):
            d[j] = struct.unpack('<h', waveData[k:k+2])[0]
            j = j+1
    elif BitsPerSample == 24:
        print("{}: Unpacking {} bytes as 24 bits integer.".format(filename, len(waveData))) if verbose else 0
        d = np.zeros(SubChunk2Size/3,  dtype=np.int32)
        j = 0
        for k in range(0, SubChunk2Size, 3):
            d[j] = struct.unpack('<l', struct.pack('c', waveData[k]) + waveData[k:k+3])[0]
            j = j+1
    else: # anything else will be considered 32 bits
        if AudioFormat == 1:
            print("{}: Unpacking {} bytes as 32 bits integer.".format(filename, len(waveData))) if verbose else 0
            d = np.frombuffer(waveData, np.int32)
        elif AudioFormat == 3:
            print("{}: Unpacking {} bytes as 32 bits float.".format(filename, len(waveData))) if verbose else 0
            d = np.frombuffer(waveData, np.float32)

    v = d[0::NumChannels]
    for i in range(1,NumChannels):
        v = np.vstack((v, floatdata[i::NumChannels]))
    return v, SampleRate, NumChannels, BitsPerSample
