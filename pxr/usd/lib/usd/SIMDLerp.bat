ispc.exe SIMDLerp.ispc -o SIMDLerp.obj -h SIMDLerp.h --target=sse2,sse4,avx --arch=x86-64 --opt=fast-masked-vload --opt=fast-math --opt=force-aligned-memory
