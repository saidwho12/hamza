#if 0
// ; SPIR-V
// ; Version: 1.5
// ; Generator: Google Shaderc over Glslang; 10
// ; Bound: 18261
// ; Schema: 0
//                OpCapability Shader
//           %1 = OpExtInstImport "GLSL.std.450"
//                OpMemoryModel Logical GLSL450
//                OpEntryPoint Vertex %5663 "main" %4990 %4930 %4924 %4136
// 
//                ; Annotations
//                OpDecorate %4990 Location 0
//                OpMemberDecorate %_struct_329 0 BuiltIn Position
//                OpMemberDecorate %_struct_329 1 BuiltIn PointSize
//                OpMemberDecorate %_struct_329 2 BuiltIn ClipDistance
//                OpMemberDecorate %_struct_329 3 BuiltIn CullDistance
//                OpDecorate %_struct_329 Block
//                OpDecorate %4924 Location 0
//                OpDecorate %4136 Location 1
// 
//                ; Types, variables and constants
//        %void = OpTypeVoid
//        %1282 = OpTypeFunction %void
//       %float = OpTypeFloat 32
//     %v2float = OpTypeVector %float 2
// %_ptr_Input_v2float = OpTypePointer Input %v2float
//        %4990 = OpVariable %_ptr_Input_v2float Input
//     %v4float = OpTypeVector %float 4
//        %uint = OpTypeInt 32 0
//      %uint_1 = OpConstant %uint 1
// %_arr_float_uint_1 = OpTypeArray %float %uint_1
// %_struct_329 = OpTypeStruct %v4float %float %_arr_float_uint_1 %_arr_float_uint_1
// %_ptr_Output__struct_329 = OpTypePointer Output %_struct_329
//        %4930 = OpVariable %_ptr_Output__struct_329 Output
//         %int = OpTypeInt 32 1
//       %int_0 = OpConstant %int 0
//     %float_0 = OpConstant %float 0
//     %float_1 = OpConstant %float 1
// %_ptr_Output_v4float = OpTypePointer Output %v4float
// %_ptr_Output_v2float = OpTypePointer Output %v2float
//        %4924 = OpVariable %_ptr_Output_v2float Output
//        %4136 = OpVariable %_ptr_Input_v2float Input
// %float_0_000500000024 = OpConstant %float 0.000500000024
//        %2557 = OpConstantComposite %v2float %float_0_000500000024 %float_0_000500000024
// 
//                ; Function 5663
//        %5663 = OpFunction %void None %1282
//        %6904 = OpLabel
//        %8678 = OpLoad %v2float %4990
//       %15540 = OpFMul %v2float %8678 %2557
//        %9348 = OpCompositeExtract %float %15540 0
//       %10725 = OpCompositeExtract %float %15540 1
//       %18260 = OpCompositeConstruct %v4float %9348 %10725 %float_0 %float_1
//        %8407 = OpAccessChain %_ptr_Output_v4float %4930 %int_0
//                OpStore %8407 %18260
//       %17934 = OpLoad %v2float %4136
//                OpStore %4924 %17934
//                OpReturn
//                OpFunctionEnd
#endif

const uint8_t hz_gl4_stencil_kokojima_vertex_shader[] = 
{
       3,   2,  35,   7,   0,   5,
       1,   0,  10,   0,  13,   0,
      85,  71,   0,   0,   0,   0,
       0,   0,  17,   0,   2,   0,
       1,   0,   0,   0,  11,   0,
       6,   0,   1,   0,   0,   0,
      71,  76,  83,  76,  46, 115,
     116, 100,  46,  52,  53,  48,
       0,   0,   0,   0,  14,   0,
       3,   0,   0,   0,   0,   0,
       1,   0,   0,   0,  15,   0,
       9,   0,   0,   0,   0,   0,
      31,  22,   0,   0, 109,  97,
     105, 110,   0,   0,   0,   0,
     126,  19,   0,   0,  66,  19,
       0,   0,  60,  19,   0,   0,
      40,  16,   0,   0,  71,   0,
       4,   0, 126,  19,   0,   0,
      30,   0,   0,   0,   0,   0,
       0,   0,  72,   0,   5,   0,
      73,   1,   0,   0,   0,   0,
       0,   0,  11,   0,   0,   0,
       0,   0,   0,   0,  72,   0,
       5,   0,  73,   1,   0,   0,
       1,   0,   0,   0,  11,   0,
       0,   0,   1,   0,   0,   0,
      72,   0,   5,   0,  73,   1,
       0,   0,   2,   0,   0,   0,
      11,   0,   0,   0,   3,   0,
       0,   0,  72,   0,   5,   0,
      73,   1,   0,   0,   3,   0,
       0,   0,  11,   0,   0,   0,
       4,   0,   0,   0,  71,   0,
       3,   0,  73,   1,   0,   0,
       2,   0,   0,   0,  71,   0,
       4,   0,  60,  19,   0,   0,
      30,   0,   0,   0,   0,   0,
       0,   0,  71,   0,   4,   0,
      40,  16,   0,   0,  30,   0,
       0,   0,   1,   0,   0,   0,
      19,   0,   2,   0,   8,   0,
       0,   0,  33,   0,   3,   0,
       2,   5,   0,   0,   8,   0,
       0,   0,  22,   0,   3,   0,
      13,   0,   0,   0,  32,   0,
       0,   0,  23,   0,   4,   0,
      19,   0,   0,   0,  13,   0,
       0,   0,   2,   0,   0,   0,
      32,   0,   4,   0, 144,   2,
       0,   0,   1,   0,   0,   0,
      19,   0,   0,   0,  59,   0,
       4,   0, 144,   2,   0,   0,
     126,  19,   0,   0,   1,   0,
       0,   0,  23,   0,   4,   0,
      29,   0,   0,   0,  13,   0,
       0,   0,   4,   0,   0,   0,
      21,   0,   4,   0,  11,   0,
       0,   0,  32,   0,   0,   0,
       0,   0,   0,   0,  43,   0,
       4,   0,  11,   0,   0,   0,
      13,  10,   0,   0,   1,   0,
       0,   0,  28,   0,   4,   0,
      87,   2,   0,   0,  13,   0,
       0,   0,  13,  10,   0,   0,
      30,   0,   6,   0,  73,   1,
       0,   0,  29,   0,   0,   0,
      13,   0,   0,   0,  87,   2,
       0,   0,  87,   2,   0,   0,
      32,   0,   4,   0, 198,   3,
       0,   0,   3,   0,   0,   0,
      73,   1,   0,   0,  59,   0,
       4,   0, 198,   3,   0,   0,
      66,  19,   0,   0,   3,   0,
       0,   0,  21,   0,   4,   0,
      12,   0,   0,   0,  32,   0,
       0,   0,   1,   0,   0,   0,
      43,   0,   4,   0,  12,   0,
       0,   0,  11,  10,   0,   0,
       0,   0,   0,   0,  43,   0,
       4,   0,  13,   0,   0,   0,
      12,  10,   0,   0,   0,   0,
       0,   0,  43,   0,   4,   0,
      13,   0,   0,   0, 138,   0,
       0,   0,   0,   0, 128,  63,
      32,   0,   4,   0, 154,   2,
       0,   0,   3,   0,   0,   0,
      29,   0,   0,   0,  32,   0,
       4,   0, 145,   2,   0,   0,
       3,   0,   0,   0,  19,   0,
       0,   0,  59,   0,   4,   0,
     145,   2,   0,   0,  60,  19,
       0,   0,   3,   0,   0,   0,
      59,   0,   4,   0, 144,   2,
       0,   0,  40,  16,   0,   0,
       1,   0,   0,   0,  43,   0,
       4,   0,  13,   0,   0,   0,
     162,   0,   0,   0, 111,  18,
       3,  58,  44,   0,   5,   0,
      19,   0,   0,   0, 253,   9,
       0,   0, 162,   0,   0,   0,
     162,   0,   0,   0,  54,   0,
       5,   0,   8,   0,   0,   0,
      31,  22,   0,   0,   0,   0,
       0,   0,   2,   5,   0,   0,
     248,   0,   2,   0, 248,  26,
       0,   0,  61,   0,   4,   0,
      19,   0,   0,   0, 230,  33,
       0,   0, 126,  19,   0,   0,
     133,   0,   5,   0,  19,   0,
       0,   0, 180,  60,   0,   0,
     230,  33,   0,   0, 253,   9,
       0,   0,  81,   0,   5,   0,
      13,   0,   0,   0, 132,  36,
       0,   0, 180,  60,   0,   0,
       0,   0,   0,   0,  81,   0,
       5,   0,  13,   0,   0,   0,
     229,  41,   0,   0, 180,  60,
       0,   0,   1,   0,   0,   0,
      80,   0,   7,   0,  29,   0,
       0,   0,  84,  71,   0,   0,
     132,  36,   0,   0, 229,  41,
       0,   0,  12,  10,   0,   0,
     138,   0,   0,   0,  65,   0,
       5,   0, 154,   2,   0,   0,
     215,  32,   0,   0,  66,  19,
       0,   0,  11,  10,   0,   0,
      62,   0,   3,   0, 215,  32,
       0,   0,  84,  71,   0,   0,
      61,   0,   4,   0,  19,   0,
       0,   0,  14,  70,   0,   0,
      40,  16,   0,   0,  62,   0,
       3,   0,  60,  19,   0,   0,
      14,  70,   0,   0, 253,   0,
       1,   0,  56,   0,   1,   0,
};
