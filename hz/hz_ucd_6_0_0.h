#ifndef HZ_UCD_6_0_0_H
#define HZ_UCD_6_0_0_H

#include <stdint.h>

#define HZ_UCD_VERSION HZ_MAKE_VERSION(6,0,0)

typedef enum {
    HZ_JOINING_TYPE_C = 1 << 16,
    HZ_JOINING_TYPE_D = 1 << 17,
    HZ_JOINING_TYPE_L = 1 << 18,
    HZ_JOINING_TYPE_R = 1 << 19,
    HZ_JOINING_TYPE_T = 1 << 20,
    HZ_JOINING_TYPE_U = 1 << 21,
} hz_joining_type_t;

typedef enum {
    HZ_JOINING_GROUP_NONE,
    HZ_JOINING_GROUP_AIN,
    HZ_JOINING_GROUP_ALAPH,
    HZ_JOINING_GROUP_ALEF,
    HZ_JOINING_GROUP_BEH,
    HZ_JOINING_GROUP_BETH,
    HZ_JOINING_GROUP_BURUSHASKI_YEH_BARREE,
    HZ_JOINING_GROUP_DAL,
    HZ_JOINING_GROUP_DALATH_RISH,
    HZ_JOINING_GROUP_E,
    HZ_JOINING_GROUP_FARSI_YEH,
    HZ_JOINING_GROUP_FE,
    HZ_JOINING_GROUP_FEH,
    HZ_JOINING_GROUP_FINAL_SEMKATH,
    HZ_JOINING_GROUP_GAF,
    HZ_JOINING_GROUP_GAMAL,
    HZ_JOINING_GROUP_HAH,
    HZ_JOINING_GROUP_HE,
    HZ_JOINING_GROUP_HEH,
    HZ_JOINING_GROUP_HEH_GOAL,
    HZ_JOINING_GROUP_HETH,
    HZ_JOINING_GROUP_KAF,
    HZ_JOINING_GROUP_KAPH,
    HZ_JOINING_GROUP_KHAPH,
    HZ_JOINING_GROUP_KNOTTED_HEH,
    HZ_JOINING_GROUP_LAM,
    HZ_JOINING_GROUP_LAMADH,
    HZ_JOINING_GROUP_MEEM,
    HZ_JOINING_GROUP_MIM,
    HZ_JOINING_GROUP_NOON,
    HZ_JOINING_GROUP_NUN,
    HZ_JOINING_GROUP_NYA,
    HZ_JOINING_GROUP_PE,
    HZ_JOINING_GROUP_QAF,
    HZ_JOINING_GROUP_QAPH,
    HZ_JOINING_GROUP_REH,
    HZ_JOINING_GROUP_REVERSED_PE,
    HZ_JOINING_GROUP_SAD,
    HZ_JOINING_GROUP_SADHE,
    HZ_JOINING_GROUP_SEEN,
    HZ_JOINING_GROUP_SEMKATH,
    HZ_JOINING_GROUP_SHIN,
    HZ_JOINING_GROUP_SWASH_KAF,
    HZ_JOINING_GROUP_SYRIAC_WAW,
    HZ_JOINING_GROUP_TAH,
    HZ_JOINING_GROUP_TAW,
    HZ_JOINING_GROUP_TEH_MARBUTA,
    HZ_JOINING_GROUP_TEH_MARBUTA_GOAL,
    HZ_JOINING_GROUP_TETH,
    HZ_JOINING_GROUP_WAW,
    HZ_JOINING_GROUP_YEH,
    HZ_JOINING_GROUP_YEH_BARREE,
    HZ_JOINING_GROUP_YEH_WITH_TAIL,
    HZ_JOINING_GROUP_YUDH,
    HZ_JOINING_GROUP_YUDH_HE,
    HZ_JOINING_GROUP_ZAIN,
    HZ_JOINING_GROUP_ZHAIN,
} hz_joining_group_t;

static int32_t hz_ucd_arabic_joining_k2[275] = {
       0,   0,-271,-268,-256,-241,
    -239,   2,   2,-238,   0,-227,
       0,-225,-216,   2,   0,-195,
    -180,   0,-177,-170,-161,   1,
       0,   0,   0,   0,   0,-146,
       0,   0,   3,   0,   0,   2,
       0,-108,   0,   0,   0,-103,
    -100,   0, -86,   2,   0, -71,
       1,   1,   0, -60,   2,   8,
     -52, -50,   0,   9,   9,   1,
     -42, -40,   1,   0,   4,   0,
     -35,   1,   0,   0,   1, -26,
       0, -24,   0,   1,  -1,  -3,
       0,  -6,   6, -11, -15,   4,
       2,   1,   0,-274,   1,   6,
       1, -49,  20,   0, -57,   0,
     -61,   0,   0,   0,   2,   0,
       4, -72,   0, -79,   3,   2,
       0, -91, -95, -99,   0,   1,
    -106,   0,   0,   0,-117,-121,
       0,   0,-125,   0,-129,   2,
       0,   0,-140,   0,   1,   0,
       0,   1,   0,   1,   3,-164,
       0,   0,-169,   6,-171,-172,
    -175,   0,   2,-186,   4,-206,
       0,-210,   0,   0,   0,   0,
       0,   0,   4,   0,   0,   0,
    -247,   0,-251,-254,   2,-258,
    -264,   0,   0,   1,-273,   9,
    -270,   0,   0,   0,   0,   0,
       4,-249,   0,   0,-242,   2,
       2,-233,   5,   1,   0,   2,
       0,   0,   1,   0,-212,   0,
       0,   0,   0,-207,   0,   3,
       0,   0,-168,-167,   4,   1,
    -144,-141,   1,-131,   0,   0,
    -127,-124,   0,-123,-109,   0,
       2,   5,   2, -87, -74, -68,
     -66, -65, -63,   2,   0,   4,
     -30,  -5,  -8,  13, -27,   0,
       2, -39,   1,   3,   0, -67,
       0,   0,   0,   0, -82,   2,
       1,   6,   0,-107,-130,   0,
       2,   1,-135,-142,   2,   2,
    -154,-155,-157,  18,-214,   3,
       0,   2,   0,  13,   9,
};

static uint32_t hz_ucd_arabic_joining_ucs_codepoints[275] = {
        1602,    1914,    8205,    1735,    2007,    1606,
        1584,    1650,    2012,    1870,    1592,    1811,
        1720,    1999,    1888,    1659,    1835,    1654,
        1719,    2024,    1668,    1813,    1839,    2009,
        1692,    2017,    1580,    1737,    1734,    1572,
        1901,    1903,    1743,    1995,    1905,    1808,
        1821,    1818,    1881,    1604,    1569,    1705,
        1916,    1832,    1823,    1667,    1745,    1729,
        1900,    1837,    1715,    1882,    1685,    1717,
        1649,    1892,    1660,    2023,    1669,    1909,
        1710,    1601,    1997,    1726,    1871,    1725,
        1838,    1890,    1740,    1897,    1830,    1676,
        1869,    1819,    1586,    1671,    1727,    2042,
        1876,    1891,    1610,    1757,    1712,    1599,
        1658,    1744,    1547,    1537,    2015,    1686,
        1724,    1684,    1583,    1582,    1578,    1588,
        1694,    1706,    2005,    2011,    2020,    1573,
        2018,    1878,    1570,    1664,    1732,    2003,
        1884,    1646,    1699,    1597,    1877,    1810,
        1836,    1774,    1714,    2014,    1701,    1682,
        2004,    1675,    1747,    1598,    2000,    1733,
        1591,    1908,    1731,    1788,    1695,    1574,
        1647,    1738,    2013,    1787,    1700,    1666,
        1812,    1749,    1827,    1655,    2025,    1711,
        1681,    1665,    1893,    1605,    1896,    1594,
        1656,    1576,    2026,    1690,    1702,    1880,
        1833,    1663,    1539,    1872,    2006,    1589,
        1746,    1704,    1595,    1816,    1899,    1716,
        1581,    1662,    1814,    1889,    1673,    1883,
        1590,    1822,    1577,    1910,    1683,    1698,
        1913,    1600,    1703,    1815,    1536,    1728,
        1886,    1875,    1919,    1579,    1593,    1879,
        1603,    1906,    1915,    1918,    1661,    1693,
        1609,    1887,    1587,    1721,    1736,    1829,
        1824,    1722,    1672,    1688,    1894,    1687,
        1775,    1820,    1689,    1912,    1996,    1679,
        1723,    1874,    1994,    1651,    1608,    1708,
        1742,    1730,    1902,    1739,    2010,    1998,
        1652,    1911,    2016,    1898,    1680,    1885,
        1895,    2001,    1907,    1786,    1653,    2021,
        1607,    2019,    2008,    1713,    1657,    1828,
        1718,    1709,    1917,    1697,    2002,    1904,
        1596,    2022,    1817,    1568,    1538,    1571,
        1696,    1825,    1791,    1873,    1674,    1575,
        1691,    1707,    1741,    1677,    1544,    1670,
        1826,    1585,    1678,    1834,    1831,
};

static uint32_t hz_ucd_arabic_joining_data[275] = {
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_QAF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BURUSHASKI_YEH_BARREE,
    HZ_JOINING_TYPE_C | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_WAW,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NOON,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DAL,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_ALEF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_KHAPH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_TAH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_GAMAL,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_LAM,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SHIN,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_WAW,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_LAM,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HAH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DALATH_RISH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DALATH_RISH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SEEN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HAH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_WAW,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_WAW,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_WAW,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SEEN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HAH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_WAW,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_REH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_ALAPH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_YUDH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HETH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DAL,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_LAM,
    HZ_JOINING_TYPE_U | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_GAF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HAH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_SADHE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_KAPH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HAH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_YEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HEH_GOAL,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_REH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BETH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_GAF,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DAL,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_REH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_LAM,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_ALEF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_GAF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HAH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FARSI_YEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_KAF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_KNOTTED_HEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NYA,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_GAMAL,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_GAF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FARSI_YEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NOON,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_PE,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DAL,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_ZHAIN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_TETH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_REH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HAH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HAH,
    HZ_JOINING_TYPE_C | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_GAF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_YEH,
    HZ_JOINING_TYPE_U | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_GAF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FARSI_YEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_YEH,
    HZ_JOINING_TYPE_U | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_U | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_REH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NOON,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_REH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DAL,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HAH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SEEN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SAD,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SWASH_KAF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_ALEF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_ALEF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_WAW,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SEEN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FARSI_YEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BETH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_TAW,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DAL,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_GAF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FEH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_REH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DAL,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_YEH_BARREE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FARSI_YEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_WAW,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_TAH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_ALEF,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_TEH_MARBUTA_GOAL,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_AIN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_TAH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_YEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_QAF,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_WAW,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SAD,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HAH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_GAMAL,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_TEH_MARBUTA,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SEMKATH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_WAW,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_GAF,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_REH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HAH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_MEEM,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_MEEM,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NOON,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_AIN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_YEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SEEN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HAH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_QAPH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_U | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SAD,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_YEH_BARREE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_QAF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_GAF,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_SYRIAC_WAW,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_REH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_GAF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HAH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DALATH_RISH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FEH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DAL,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_REH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SAD,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_YUDH_HE,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_TEH_MARBUTA,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FARSI_YEH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_REH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FEH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_WAW,
    HZ_JOINING_TYPE_C | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_QAF,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_HE,
    HZ_JOINING_TYPE_U | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_TEH_MARBUTA,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_AIN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_KAF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_AIN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HAH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_KAF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HAH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BURUSHASKI_YEH_BARREE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SEEN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SAD,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_YEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_AIN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SEEN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NOON,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_WAW,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_E,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_LAMADH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NOON,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DAL,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_REH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_MEEM,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_REH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_REH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_TETH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_REH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_WAW,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DAL,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NOON,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_ALEF,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_WAW,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_KAF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FARSI_YEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HEH_GOAL,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HAH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_WAW,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_U | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_YEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_LAM,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DAL,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_AIN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NOON,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_ALEF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SEEN,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_ALEF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_GAF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FINAL_SEMKATH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_LAM,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_KAF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SEEN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_FEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SEEN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_GAF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_ZAIN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_YEH,
    HZ_JOINING_TYPE_U | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_ALEF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_AIN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_MIM,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_KNOTTED_HEH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_BEH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DAL,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_ALEF,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_SEEN,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_GAF,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_YEH_WITH_TAIL,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DAL,
    HZ_JOINING_TYPE_U | HZ_JOINING_GROUP_NONE,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_HAH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_NUN,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_REH,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DAL,
    HZ_JOINING_TYPE_R | HZ_JOINING_GROUP_DALATH_RISH,
    HZ_JOINING_TYPE_D | HZ_JOINING_GROUP_REVERSED_PE,
};

#endif /* HZ_UCD_6_0_0_H */