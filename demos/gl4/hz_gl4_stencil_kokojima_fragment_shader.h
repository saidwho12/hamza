#if 0
// ; SPIR-V
// ; Version: 1.5
// ; Generator: Google Shaderc over Glslang; 10
// ; Bound: 23916
// ; Schema: 0
//                OpCapability Shader
//           %1 = OpExtInstImport "GLSL.std.450"
//                OpMemoryModel Logical GLSL450
//                OpEntryPoint Fragment %5663 "main" %4924 %3569
//                OpExecutionMode %5663 OriginLowerLeft
// 
//                ; Annotations
//                OpDecorate %4924 Location 0
//                OpDecorate %3569 Location 0
// 
//                ; Types, variables and constants
//        %void = OpTypeVoid
//        %1282 = OpTypeFunction %void
//       %float = OpTypeFloat 32
//     %v2float = OpTypeVector %float 2
// %_ptr_Input_v2float = OpTypePointer Input %v2float
//        %4924 = OpVariable %_ptr_Input_v2float Input
//        %uint = OpTypeInt 32 0
//      %uint_0 = OpConstant %uint 0
// %_ptr_Input_float = OpTypePointer Input %float
//      %uint_1 = OpConstant %uint 1
//     %float_0 = OpConstant %float 0
//        %bool = OpTypeBool
//     %v4float = OpTypeVector %float 4
// %_ptr_Output_v4float = OpTypePointer Output %v4float
//        %3569 = OpVariable %_ptr_Output_v4float Output
//     %float_1 = OpConstant %float 1
//        %1284 = OpConstantComposite %v4float %float_1 %float_1 %float_1 %float_1
// 
//                ; Function 5663
//        %5663 = OpFunction %void None %1282
//       %23915 = OpLabel
//        %8421 = OpAccessChain %_ptr_Input_float %4924 %uint_0
//       %23010 = OpLoad %float %8421
//        %8864 = OpFMul %float %23010 %23010
//       %11368 = OpAccessChain %_ptr_Input_float %4924 %uint_1
//        %7239 = OpLoad %float %11368
//        %7159 = OpFSub %float %8864 %7239
//        %6729 = OpFOrdGreaterThanEqual %bool %7159 %float_0
//                OpSelectionMerge %13376 None
//                OpBranchConditional %6729 %21473 %13376
//       %21473 = OpLabel
//                OpKill
//       %13376 = OpLabel
//                OpStore %3569 %1284
//                OpReturn
//                OpFunctionEnd
#endif

const uint8_t hz_gl4_stencil_kokojima_fragment_shader[] = 
{
       3,   2,  35,   7,   0,   5,
       1,   0,  10,   0,  13,   0,
     108,  93,   0,   0,   0,   0,
       0,   0,  17,   0,   2,   0,
       1,   0,   0,   0,  11,   0,
       6,   0,   1,   0,   0,   0,
      71,  76,  83,  76,  46, 115,
     116, 100,  46,  52,  53,  48,
       0,   0,   0,   0,  14,   0,
       3,   0,   0,   0,   0,   0,
       1,   0,   0,   0,  15,   0,
       7,   0,   4,   0,   0,   0,
      31,  22,   0,   0, 109,  97,
     105, 110,   0,   0,   0,   0,
      60,  19,   0,   0, 241,  13,
       0,   0,  16,   0,   3,   0,
      31,  22,   0,   0,   8,   0,
       0,   0,  71,   0,   4,   0,
      60,  19,   0,   0,  30,   0,
       0,   0,   0,   0,   0,   0,
      71,   0,   4,   0, 241,  13,
       0,   0,  30,   0,   0,   0,
       0,   0,   0,   0,  19,   0,
       2,   0,   8,   0,   0,   0,
      33,   0,   3,   0,   2,   5,
       0,   0,   8,   0,   0,   0,
      22,   0,   3,   0,  13,   0,
       0,   0,  32,   0,   0,   0,
      23,   0,   4,   0,  19,   0,
       0,   0,  13,   0,   0,   0,
       2,   0,   0,   0,  32,   0,
       4,   0, 144,   2,   0,   0,
       1,   0,   0,   0,  19,   0,
       0,   0,  59,   0,   4,   0,
     144,   2,   0,   0,  60,  19,
       0,   0,   1,   0,   0,   0,
      21,   0,   4,   0,  11,   0,
       0,   0,  32,   0,   0,   0,
       0,   0,   0,   0,  43,   0,
       4,   0,  11,   0,   0,   0,
      10,  10,   0,   0,   0,   0,
       0,   0,  32,   0,   4,   0,
     138,   2,   0,   0,   1,   0,
       0,   0,  13,   0,   0,   0,
      43,   0,   4,   0,  11,   0,
       0,   0,  13,  10,   0,   0,
       1,   0,   0,   0,  43,   0,
       4,   0,  13,   0,   0,   0,
      12,  10,   0,   0,   0,   0,
       0,   0,  20,   0,   2,   0,
       9,   0,   0,   0,  23,   0,
       4,   0,  29,   0,   0,   0,
      13,   0,   0,   0,   4,   0,
       0,   0,  32,   0,   4,   0,
     154,   2,   0,   0,   3,   0,
       0,   0,  29,   0,   0,   0,
      59,   0,   4,   0, 154,   2,
       0,   0, 241,  13,   0,   0,
       3,   0,   0,   0,  43,   0,
       4,   0,  13,   0,   0,   0,
     138,   0,   0,   0,   0,   0,
     128,  63,  44,   0,   7,   0,
      29,   0,   0,   0,   4,   5,
       0,   0, 138,   0,   0,   0,
     138,   0,   0,   0, 138,   0,
       0,   0, 138,   0,   0,   0,
      54,   0,   5,   0,   8,   0,
       0,   0,  31,  22,   0,   0,
       0,   0,   0,   0,   2,   5,
       0,   0, 248,   0,   2,   0,
     107,  93,   0,   0,  65,   0,
       5,   0, 138,   2,   0,   0,
     229,  32,   0,   0,  60,  19,
       0,   0,  10,  10,   0,   0,
      61,   0,   4,   0,  13,   0,
       0,   0, 226,  89,   0,   0,
     229,  32,   0,   0, 133,   0,
       5,   0,  13,   0,   0,   0,
     160,  34,   0,   0, 226,  89,
       0,   0, 226,  89,   0,   0,
      65,   0,   5,   0, 138,   2,
       0,   0, 104,  44,   0,   0,
      60,  19,   0,   0,  13,  10,
       0,   0,  61,   0,   4,   0,
      13,   0,   0,   0,  71,  28,
       0,   0, 104,  44,   0,   0,
     131,   0,   5,   0,  13,   0,
       0,   0, 247,  27,   0,   0,
     160,  34,   0,   0,  71,  28,
       0,   0, 190,   0,   5,   0,
       9,   0,   0,   0,  73,  26,
       0,   0, 247,  27,   0,   0,
      12,  10,   0,   0, 247,   0,
       3,   0,  64,  52,   0,   0,
       0,   0,   0,   0, 250,   0,
       4,   0,  73,  26,   0,   0,
     225,  83,   0,   0,  64,  52,
       0,   0, 248,   0,   2,   0,
     225,  83,   0,   0, 252,   0,
       1,   0, 248,   0,   2,   0,
      64,  52,   0,   0,  62,   0,
       3,   0, 241,  13,   0,   0,
       4,   5,   0,   0, 253,   0,
       1,   0,  56,   0,   1,   0,
};
