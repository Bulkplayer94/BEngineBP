#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer MatrixBuffer
// {
//
//   float4x4 worldMatrix;              // Offset:    0 Size:    64
//   float4x4 viewMatrix;               // Offset:   64 Size:    64
//   float4x4 perspectiveMatrix;        // Offset:  128 Size:    64
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// MatrixBuffer                      cbuffer      NA          NA            cb3      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// POSITION                 0   xy          0     NONE   float   xy  
// TEX                      0   xy          1     NONE   float   xy  
// SV_VertexID              0   x           2   VERTID    uint   x   
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_Position              0   xyzw        0      POS   float   xyzw
// TEXCOORD                 0   xy          1     NONE   float   xy  
// COLOR                    0   xyzw        2     NONE   float   xyzw
//
vs_5_0
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB3[12], immediateIndexed
dcl_input v0.xy
dcl_input v1.xy
dcl_input_sgv v2.x, vertex_id
dcl_output_siv o0.xyzw, position
dcl_output o1.xy
dcl_output o2.xyzw
dcl_temps 2
mov r0.xy, v0.xyxx
mov r0.z, l(1.000000)
dp3 r1.x, r0.xyzx, cb3[0].xywx
dp3 r1.y, r0.xyzx, cb3[1].xywx
dp3 r1.z, r0.xyzx, cb3[2].xywx
dp3 r1.w, r0.xyzx, cb3[3].xywx
dp4 r0.x, r1.xyzw, cb3[4].xyzw
dp4 r0.y, r1.xyzw, cb3[5].xyzw
dp4 r0.z, r1.xyzw, cb3[6].xyzw
dp4 r0.w, r1.xyzw, cb3[7].xyzw
dp4 o0.x, r0.xyzw, cb3[8].xyzw
dp4 o0.y, r0.xyzw, cb3[9].xyzw
dp4 o0.z, r0.xyzw, cb3[10].xyzw
dp4 o0.w, r0.xyzw, cb3[11].xyzw
switch v2.x
  case l(0)
  mov r0.xyzw, l(1.000000,0,0,1.000000)
  break 
  case l(1)
  mov r0.xyzw, l(0,1.000000,0,1.000000)
  break 
  case l(2)
  mov r0.xyzw, l(0,0,1.000000,1.000000)
  break 
  case l(3)
  mov r0.xyzw, l(1.000000,1.000000,1.000000,1.000000)
  break 
  default 
  break 
endswitch 
mov o2.xyzw, r0.xyzw
mov o1.xy, v1.xyxx
ret 
// Approximately 33 instruction slots used
#endif

const BYTE SmokeVS[] =
{
     68,  88,  66,  67, 197,  35, 
     94, 254, 239,  37, 152,  67, 
    160, 202,  94, 169,  84,  55, 
    168,  26,   1,   0,   0,   0, 
    104,   6,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    188,   1,   0,   0,  48,   2, 
      0,   0, 164,   2,   0,   0, 
    204,   5,   0,   0,  82,  68, 
     69,  70, 128,   1,   0,   0, 
      1,   0,   0,   0, 108,   0, 
      0,   0,   1,   0,   0,   0, 
     60,   0,   0,   0,   0,   5, 
    254, 255,   0,   1,   0,   0, 
     85,   1,   0,   0,  82,  68, 
     49,  49,  60,   0,   0,   0, 
     24,   0,   0,   0,  32,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
     92,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  77,  97, 116, 114, 
    105, 120,  66, 117, 102, 102, 
    101, 114,   0, 171, 171, 171, 
     92,   0,   0,   0,   3,   0, 
      0,   0, 132,   0,   0,   0, 
    192,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    252,   0,   0,   0,   0,   0, 
      0,   0,  64,   0,   0,   0, 
      2,   0,   0,   0,  20,   1, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  56,   1, 
      0,   0,  64,   0,   0,   0, 
     64,   0,   0,   0,   2,   0, 
      0,   0,  20,   1,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  67,   1,   0,   0, 
    128,   0,   0,   0,  64,   0, 
      0,   0,   2,   0,   0,   0, 
     20,   1,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    119, 111, 114, 108, 100,  77, 
     97, 116, 114, 105, 120,   0, 
    102, 108, 111,  97, 116,  52, 
    120,  52,   0, 171, 171, 171, 
      3,   0,   3,   0,   4,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   8,   1,   0,   0, 
    118, 105, 101, 119,  77,  97, 
    116, 114, 105, 120,   0, 112, 
    101, 114, 115, 112, 101,  99, 
    116, 105, 118, 101,  77,  97, 
    116, 114, 105, 120,   0,  77, 
    105,  99, 114, 111, 115, 111, 
    102, 116,  32,  40,  82,  41, 
     32,  72,  76,  83,  76,  32, 
     83, 104,  97, 100, 101, 114, 
     32,  67, 111, 109, 112, 105, 
    108, 101, 114,  32,  49,  48, 
     46,  49,   0, 171, 171, 171, 
     73,  83,  71,  78, 108,   0, 
      0,   0,   3,   0,   0,   0, 
      8,   0,   0,   0,  80,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   3,   0,   0,  89,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   3,   0,   0,  93,   0, 
      0,   0,   0,   0,   0,   0, 
      6,   0,   0,   0,   1,   0, 
      0,   0,   2,   0,   0,   0, 
      1,   1,   0,   0,  80,  79, 
     83,  73,  84,  73,  79,  78, 
      0,  84,  69,  88,   0,  83, 
     86,  95,  86, 101, 114, 116, 
    101, 120,  73,  68,   0, 171, 
    171, 171,  79,  83,  71,  78, 
    108,   0,   0,   0,   3,   0, 
      0,   0,   8,   0,   0,   0, 
     80,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     92,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,   3,  12,   0,   0, 
    101,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,  15,   0,   0,   0, 
     83,  86,  95,  80, 111, 115, 
    105, 116, 105, 111, 110,   0, 
     84,  69,  88,  67,  79,  79, 
     82,  68,   0,  67,  79,  76, 
     79,  82,   0, 171,  83,  72, 
     69,  88,  32,   3,   0,   0, 
     80,   0,   1,   0, 200,   0, 
      0,   0, 106,   8,   0,   1, 
     89,   0,   0,   4,  70, 142, 
     32,   0,   3,   0,   0,   0, 
     12,   0,   0,   0,  95,   0, 
      0,   3,  50,  16,  16,   0, 
      0,   0,   0,   0,  95,   0, 
      0,   3,  50,  16,  16,   0, 
      1,   0,   0,   0,  96,   0, 
      0,   4,  18,  16,  16,   0, 
      2,   0,   0,   0,   6,   0, 
      0,   0, 103,   0,   0,   4, 
    242,  32,  16,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
    101,   0,   0,   3,  50,  32, 
     16,   0,   1,   0,   0,   0, 
    101,   0,   0,   3, 242,  32, 
     16,   0,   2,   0,   0,   0, 
    104,   0,   0,   2,   2,   0, 
      0,   0,  54,   0,   0,   5, 
     50,   0,  16,   0,   0,   0, 
      0,   0,  70,  16,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5,  66,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     16,   0,   0,   8,  18,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  70, 131,  32,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  16,   0,   0,   8, 
     34,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,  70, 131, 
     32,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,  16,   0, 
      0,   8,  66,   0,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
     70, 131,  32,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
     16,   0,   0,   8, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  70, 131,  32,   0, 
      3,   0,   0,   0,   3,   0, 
      0,   0,  17,   0,   0,   8, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  70, 142, 
     32,   0,   3,   0,   0,   0, 
      4,   0,   0,   0,  17,   0, 
      0,   8,  34,   0,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     70, 142,  32,   0,   3,   0, 
      0,   0,   5,   0,   0,   0, 
     17,   0,   0,   8,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  70, 142,  32,   0, 
      3,   0,   0,   0,   6,   0, 
      0,   0,  17,   0,   0,   8, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  70, 142, 
     32,   0,   3,   0,   0,   0, 
      7,   0,   0,   0,  17,   0, 
      0,   8,  18,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  32,   0,   3,   0, 
      0,   0,   8,   0,   0,   0, 
     17,   0,   0,   8,  34,  32, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      3,   0,   0,   0,   9,   0, 
      0,   0,  17,   0,   0,   8, 
     66,  32,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   3,   0,   0,   0, 
     10,   0,   0,   0,  17,   0, 
      0,   8, 130,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  32,   0,   3,   0, 
      0,   0,  11,   0,   0,   0, 
     76,   0,   0,   3,  10,  16, 
     16,   0,   2,   0,   0,   0, 
      6,   0,   0,   3,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     54,   0,   0,   8, 242,   0, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
    128,  63,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    128,  63,   2,   0,   0,   1, 
      6,   0,   0,   3,   1,  64, 
      0,   0,   1,   0,   0,   0, 
     54,   0,   0,   8, 242,   0, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0, 128,  63, 
      0,   0,   0,   0,   0,   0, 
    128,  63,   2,   0,   0,   1, 
      6,   0,   0,   3,   1,  64, 
      0,   0,   2,   0,   0,   0, 
     54,   0,   0,   8, 242,   0, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   2,   0,   0,   1, 
      6,   0,   0,   3,   1,  64, 
      0,   0,   3,   0,   0,   0, 
     54,   0,   0,   8, 242,   0, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
    128,  63,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   2,   0,   0,   1, 
     10,   0,   0,   1,   2,   0, 
      0,   1,  23,   0,   0,   1, 
     54,   0,   0,   5, 242,  32, 
     16,   0,   2,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
     50,  32,  16,   0,   1,   0, 
      0,   0,  70,  16,  16,   0, 
      1,   0,   0,   0,  62,   0, 
      0,   1,  83,  84,  65,  84, 
    148,   0,   0,   0,  33,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   6,   0, 
      0,   0,  12,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0
};
