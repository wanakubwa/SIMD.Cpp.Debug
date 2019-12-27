.DATA
; Vector that contains 16x8-bit const numbers.
shuffleMask db 15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0
shufflePackMask db 6, 6, 6, 6, 4, 4, 4, 4, 2, 2, 2, 2, 0, 0, 0, 0

; Constans for channels multiplication.
rFactor dw 9794, 9794, 9794, 9794, 9794, 9794, 9794, 9794
gFactor dw 19235, 19235, 19235, 19235, 19235, 19235, 19235, 19235
bFactor dw 3736, 3736, 3736, 3736, 3736, 3736, 3736, 3736

inputRegister oword ?
BBBB_GGGG_RRRR_AAAA oword ?
XXXX_BBBB oword ?
XXXX_RRRR oword ?
XXXX_GGGG oword ?

.CODE
; RCX - First value of function input.
; RDX - DRUGI PRZEKAZYWANY INT
; R8 - Trzeci Int
; RAX - RETURN conains EAX register (32bit).

testFunctionASM PROC

; Load input pointer to array to a register as values.
movdqu xmm1, [RCX]

; Save values of 128-bit register as variable.
movdqa [oword ptr inputRegister], xmm1

; Load variable from reserved memory data to 128-bit register.
movdqa xmm2, XMMWORD PTR [shuffleMask]

; Do the shuffle (Left register to shuffle - output, Right shuffle mask).
; Configuration on output A0A1A2A3_R0R1R2R3_G0G1G2G3_B0B1B2B3 stored in xmm1.
pshufb xmm1, xmm2
movdqa [oword ptr BBBB_GGGG_RRRR_AAAA], xmm1

; Getting blue channel at low.
psrldq xmm1, 12
movdqa [oword ptr XXXX_BBBB], xmm1

; Getting green channel at low.
movdqa xmm1, XMMWORD PTR [BBBB_GGGG_RRRR_AAAA]
pslldq xmm1, 4
psrldq xmm1, 12
movdqa [oword ptr XXXX_GGGG], xmm1

; Getting red channel at low.
movdqa xmm1, XMMWORD PTR [BBBB_GGGG_RRRR_AAAA]
pslldq xmm1, 8
psrldq xmm1, 12
movdqa [oword ptr XXXX_RRRR], xmm1

; Unpacking uInt8 to uInt16 XXXX_X_F_F_F_F_F: R: xmm0, G: xmm2, B: xmm3
pmovzxbw xmm0, xmm1
movdqa xmm1, XMMWORD PTR [XXXX_GGGG]
pmovzxbw xmm2, xmm1
movdqa xmm1, XMMWORD PTR [XXXX_BBBB]
pmovzxbw xmm3, xmm1

; Load R,G,B factors into xmm registers: RFactor: xmm4, gFactor: xmm5, bFactor xmm6
movdqa xmm4, XMMWORD PTR [rFactor]
movdqa xmm5, XMMWORD PTR [gFactor]
movdqa xmm6, XMMWORD PTR [bFactor]

; Multiply each channel by factor.
pmulhrsw xmm0, xmm4
pmulhrsw xmm2, xmm5
pmulhrsw xmm3, xmm6

; Adding all channels together to get one xmm7 register contains result.
paddw xmm0, xmm2
paddw xmm0, xmm3

; Add two 128-bit registers from right to left.
movdqa xmm8, XMMWORD PTR [shufflePackMask]
pshufb xmm0, xmm8

; Write xmm1 to rcx adress destination.
movdqu [RCX], xmm0

                ret
testFunctionASM ENDP

reciveCharPiotrek PROC

; Load input pointer to array to a register as values.
movdqu xmm1, [RCX]
movq xmm2, RDX

; Shifting to get only single char value at LO and get register in format XXX_VALUE
pslldq xmm2, 15
psrldq xmm2, 15


; Returning changed register at recived pointer adress.
movdqu [RCX], xmm2

ret
reciveCharPiotrek ENDP
END