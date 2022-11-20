#if 0
// ; SPIR-V
// ; Version: 1.5
// ; Generator: Google Shaderc over Glslang; 10
// ; Bound: 24954
// ; Schema: 0
//                OpCapability Shader
//           %1 = OpExtInstImport "GLSL.std.450"
//                OpMemoryModel Logical GLSL450
//                OpEntryPoint Vertex %5663 "main" %4930 %gl_VertexID
// 
//                ; Annotations
//                OpMemberDecorate %_struct_2935 0 BuiltIn Position
//                OpMemberDecorate %_struct_2935 1 BuiltIn PointSize
//                OpMemberDecorate %_struct_2935 2 BuiltIn ClipDistance
//                OpMemberDecorate %_struct_2935 3 BuiltIn CullDistance
//                OpDecorate %_struct_2935 Block
//                OpDecorate %gl_VertexID BuiltIn VertexId
//                OpDecorate %5238 NonWritable
// 
//                ; Types, variables and constants
//        %void = OpTypeVoid
//        %1282 = OpTypeFunction %void
//       %float = OpTypeFloat 32
//     %v4float = OpTypeVector %float 4
//        %uint = OpTypeInt 32 0
//      %uint_1 = OpConstant %uint 1
// %_arr_float_uint_1 = OpTypeArray %float %uint_1
// %_struct_2935 = OpTypeStruct %v4float %float %_arr_float_uint_1 %_arr_float_uint_1
// %_ptr_Output__struct_2935 = OpTypePointer Output %_struct_2935
//        %4930 = OpVariable %_ptr_Output__struct_2935 Output
//         %int = OpTypeInt 32 1
//       %int_0 = OpConstant %int 0
//     %v2float = OpTypeVector %float 2
//      %uint_3 = OpConstant %uint 3
// %_arr_v2float_uint_3 = OpTypeArray %v2float %uint_3
//    %float_n1 = OpConstant %float -1
//          %73 = OpConstantComposite %v2float %float_n1 %float_n1
//     %float_3 = OpConstant %float 3
//         %255 = OpConstantComposite %v2float %float_3 %float_n1
//        %2400 = OpConstantComposite %v2float %float_n1 %float_3
//          %14 = OpConstantComposite %_arr_v2float_uint_3 %73 %255 %2400
// %_ptr_Input_int = OpTypePointer Input %int
// %gl_VertexID = OpVariable %_ptr_Input_int Input
// %_ptr_Function__arr_v2float_uint_3 = OpTypePointer Function %_arr_v2float_uint_3
// %_ptr_Function_v2float = OpTypePointer Function %v2float
//     %float_0 = OpConstant %float 0
//     %float_1 = OpConstant %float 1
// %_ptr_Output_v4float = OpTypePointer Output %v4float
// 
//                ; Function 5663
//        %5663 = OpFunction %void None %1282
//       %24953 = OpLabel
//        %5238 = OpVariable %_ptr_Function__arr_v2float_uint_3 Function %14
//       %23578 = OpLoad %int %gl_VertexID
//       %22076 = OpAccessChain %_ptr_Function_v2float %5238 %23578
//       %20020 = OpLoad %v2float %22076
//       %19388 = OpCompositeExtract %float %20020 0
//       %23384 = OpCompositeExtract %float %20020 1
//       %18260 = OpCompositeConstruct %v4float %19388 %23384 %float_0 %float_1
//       %12055 = OpAccessChain %_ptr_Output_v4float %4930 %int_0
//                OpStore %12055 %18260
//                OpReturn
//                OpFunctionEnd
#endif

const uint8_t hz_gl4_fs_triangle_vertex_shader[] = 
{
       3,   2,  35,   7,   0,   5,
       1,   0,  10,   0,  13,   0,
     122,  97,   0,   0,   0,   0,
       0,   0,  17,   0,   2,   0,
       1,   0,   0,   0,  11,   0,
       6,   0,   1,   0,   0,   0,
      71,  76,  83,  76,  46, 115,
     116, 100,  46,  52,  53,  48,
       0,   0,   0,   0,  14,   0,
       3,   0,   0,   0,   0,   0,
       1,   0,   0,   0,  15,   0,
       7,   0,   0,   0,   0,   0,
      31,  22,   0,   0, 109,  97,
     105, 110,   0,   0,   0,   0,
      66,  19,   0,   0, 142,  18,
       0,   0,  72,   0,   5,   0,
     119,  11,   0,   0,   0,   0,
       0,   0,  11,   0,   0,   0,
       0,   0,   0,   0,  72,   0,
       5,   0, 119,  11,   0,   0,
       1,   0,   0,   0,  11,   0,
       0,   0,   1,   0,   0,   0,
      72,   0,   5,   0, 119,  11,
       0,   0,   2,   0,   0,   0,
      11,   0,   0,   0,   3,   0,
       0,   0,  72,   0,   5,   0,
     119,  11,   0,   0,   3,   0,
       0,   0,  11,   0,   0,   0,
       4,   0,   0,   0,  71,   0,
       3,   0, 119,  11,   0,   0,
       2,   0,   0,   0,  71,   0,
       4,   0, 142,  18,   0,   0,
      11,   0,   0,   0,   5,   0,
       0,   0,  71,   0,   3,   0,
     118,  20,   0,   0,  24,   0,
       0,   0,  19,   0,   2,   0,
       8,   0,   0,   0,  33,   0,
       3,   0,   2,   5,   0,   0,
       8,   0,   0,   0,  22,   0,
       3,   0,  13,   0,   0,   0,
      32,   0,   0,   0,  23,   0,
       4,   0,  29,   0,   0,   0,
      13,   0,   0,   0,   4,   0,
       0,   0,  21,   0,   4,   0,
      11,   0,   0,   0,  32,   0,
       0,   0,   0,   0,   0,   0,
      43,   0,   4,   0,  11,   0,
       0,   0,  13,  10,   0,   0,
       1,   0,   0,   0,  28,   0,
       4,   0,  42,   2,   0,   0,
      13,   0,   0,   0,  13,  10,
       0,   0,  30,   0,   6,   0,
     119,  11,   0,   0,  29,   0,
       0,   0,  13,   0,   0,   0,
      42,   2,   0,   0,  42,   2,
       0,   0,  32,   0,   4,   0,
      49,   2,   0,   0,   3,   0,
       0,   0, 119,  11,   0,   0,
      59,   0,   4,   0,  49,   2,
       0,   0,  66,  19,   0,   0,
       3,   0,   0,   0,  21,   0,
       4,   0,  12,   0,   0,   0,
      32,   0,   0,   0,   1,   0,
       0,   0,  43,   0,   4,   0,
      12,   0,   0,   0,  11,  10,
       0,   0,   0,   0,   0,   0,
      23,   0,   4,   0,  19,   0,
       0,   0,  13,   0,   0,   0,
       2,   0,   0,   0,  43,   0,
       4,   0,  11,   0,   0,   0,
      19,  10,   0,   0,   3,   0,
       0,   0,  28,   0,   4,   0,
     184,   2,   0,   0,  19,   0,
       0,   0,  19,  10,   0,   0,
      43,   0,   4,   0,  13,   0,
       0,   0,  65,   3,   0,   0,
       0,   0, 128, 191,  44,   0,
       5,   0,  19,   0,   0,   0,
      73,   0,   0,   0,  65,   3,
       0,   0,  65,   3,   0,   0,
      43,   0,   4,   0,  13,   0,
       0,   0, 162,  11,   0,   0,
       0,   0,  64,  64,  44,   0,
       5,   0,  19,   0,   0,   0,
     255,   0,   0,   0, 162,  11,
       0,   0,  65,   3,   0,   0,
      44,   0,   5,   0,  19,   0,
       0,   0,  96,   9,   0,   0,
      65,   3,   0,   0, 162,  11,
       0,   0,  44,   0,   6,   0,
     184,   2,   0,   0,  14,   0,
       0,   0,  73,   0,   0,   0,
     255,   0,   0,   0,  96,   9,
       0,   0,  32,   0,   4,   0,
     137,   2,   0,   0,   1,   0,
       0,   0,  12,   0,   0,   0,
      59,   0,   4,   0, 137,   2,
       0,   0, 142,  18,   0,   0,
       1,   0,   0,   0,  32,   0,
       4,   0,  53,   5,   0,   0,
       7,   0,   0,   0, 184,   2,
       0,   0,  32,   0,   4,   0,
     144,   2,   0,   0,   7,   0,
       0,   0,  19,   0,   0,   0,
      43,   0,   4,   0,  13,   0,
       0,   0,  12,  10,   0,   0,
       0,   0,   0,   0,  43,   0,
       4,   0,  13,   0,   0,   0,
     138,   0,   0,   0,   0,   0,
     128,  63,  32,   0,   4,   0,
     154,   2,   0,   0,   3,   0,
       0,   0,  29,   0,   0,   0,
      54,   0,   5,   0,   8,   0,
       0,   0,  31,  22,   0,   0,
       0,   0,   0,   0,   2,   5,
       0,   0, 248,   0,   2,   0,
     121,  97,   0,   0,  59,   0,
       5,   0,  53,   5,   0,   0,
     118,  20,   0,   0,   7,   0,
       0,   0,  14,   0,   0,   0,
      61,   0,   4,   0,  12,   0,
       0,   0,  26,  92,   0,   0,
     142,  18,   0,   0,  65,   0,
       5,   0, 144,   2,   0,   0,
      60,  86,   0,   0, 118,  20,
       0,   0,  26,  92,   0,   0,
      61,   0,   4,   0,  19,   0,
       0,   0,  52,  78,   0,   0,
      60,  86,   0,   0,  81,   0,
       5,   0,  13,   0,   0,   0,
     188,  75,   0,   0,  52,  78,
       0,   0,   0,   0,   0,   0,
      81,   0,   5,   0,  13,   0,
       0,   0,  88,  91,   0,   0,
      52,  78,   0,   0,   1,   0,
       0,   0,  80,   0,   7,   0,
      29,   0,   0,   0,  84,  71,
       0,   0, 188,  75,   0,   0,
      88,  91,   0,   0,  12,  10,
       0,   0, 138,   0,   0,   0,
      65,   0,   5,   0, 154,   2,
       0,   0,  23,  47,   0,   0,
      66,  19,   0,   0,  11,  10,
       0,   0,  62,   0,   3,   0,
      23,  47,   0,   0,  84,  71,
       0,   0, 253,   0,   1,   0,
      56,   0,   1,   0,
};
