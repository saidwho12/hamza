// #define HZ_NOSTDLIB
/*
#include "hz_ucd_4_1_0.h"
#include "hz_ucd_5_0_0.h"
#include "hz_ucd_5_1_0.h"
#include "hz_ucd_5_2_0.h"
#include "hz_ucd_6_0_0.h"
#include "hz_ucd_6_1_0.h"
#include "hz_ucd_6_2_0.h"
#include "hz_ucd_6_3_0.h"
#include "hz_ucd_7_0_0.h"
#include "hz_ucd_8_0_0.h"
#include "hz_ucd_9_0_0.h"
#include "hz_ucd_10_0_0.h"
#include "hz_ucd_11_0_0.h"
#include "hz_ucd_12_0_0.h"
#include "hz_ucd_12_1_0.h"
#include "hz_ucd_13_0_0.h"
#include "hz_ucd_14_0_0.h"
*/
// #include "hz_ucd_15_0_0.h"
#include "./ucd/hz_ucd_5_2_0.h"

#define HZ_UCD_VERSIONS_LIST(X)\
    X(4,1,0) \
    X(5,0,0) \
    X(5,1,0) \
    X(5,2,0) \
    X(6,0,0) \
    X(6,1,0) \
    X(6,2,0) \
    X(6,3,0) \
    X(7,0,0) \
    X(8,0,0) \
    X(9,0,0) \
    X(10,0,0) \
    X(11,0,0) \
    X(12,0,0) \
    X(12,1,0) \
    X(13,0,0) \
    X(14,0,0) \
    X(15,0,0)

// #define HZ_INCLUDE_STBTT

#define HZ_INCLUDE_THREADS
#define HZ_MAX_RECURSE_DEPTH 16
#define HZ_CONFIG_ENABLE_VALIDATION 1
