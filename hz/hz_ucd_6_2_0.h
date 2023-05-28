#ifndef HZ_UCD_6_2_0_H
#define HZ_UCD_6_2_0_H

#include <stdint.h>

#define HZ_UCD_VERSION HZ_MAKE_VERSION(6,2,0)

uint16_t hz_ucd_arabic_joining_k2[313] = {
       0,  48,   6,   6,  21,   3,
       6,   0,   4,  27,   0,   2,
       0,  16,  12,   9,   3,  21,
       6,   1,   6,   4,   1,   1,
       1,   0,   4,   0,  28,   1,
       0,   0,  20,   0,   3,   0,
       0,   0,   0,   3,   3,   6,
       1,   5,  12,   2,  18,   0,
       0,   4,   3,   1,   1,   0,
       2,   6,   3,   0,   0,   0,
       0,   3,   4,   0,   0,   1,
       2,   6,   3,   0,   0,   1,
       2,   2,   0,   2,   0,   5,
       0,   4,  28,  10,   0,   0,
       1,   0,   8,   3,   1,   2,
       0,   0,   2,   5,  10,  15,
       0,   0,   0,   0,   3,   0,
       0,   6,   4,   0,   0,   1,
       2,   0,   0,   6,   4,  12,
       1,   4,   2,   9,   1,   0,
       0,   2,   2,   1,   0,   3,
       3,  12,   0,   0,   5,   2,
       1,   0,   9,   0,   0,   0,
       1,   0,   1,   0,   1,   1,
       0,   1,   7,   7,   3,   0,
       2,   0,   0,   2,   5,   1,
       1,   6,   3,   1,   2,   1,
       1,   0,   0,   4,   5,   1,
       1,   0,   4,   8,   0,   0,
       0,   0,   0,   0,   5,   0,
       1,   0,   0,   0,   0,   0,
       1,  79,   7,   0,   0, 108,
      56,   9,  53,   7,   3,   4,
      19,   4,   0,   6,  11,   4,
       7,   2,   0,   0,   0,   2,
      13,   2,  17,  14,   0,   0,
       3,   1,   1,   1,   0,   1,
       6,   0,   0,   1,   0,   0,
       0,  19,   5,   0,   1,   0,
       1,   1,   2,   5,   4,   3,
       0,   0,   0,   1,   3,   7,
       0,   0,   1,   6,  16,   3,
       0,   0,   7,   0,   2,   1,
       0,   0,   0,   2,  10,   2,
       2,   3,   2,   0,   0,   1,
       0,   0,   0,   7,   2,   5,
       3,   2,   1,   0,   5,   0,
       2,   0,   0,   2,   1,   0,
       3,   2,   1,   2,   0,   0,
       0,   6,   1,   0,  17,   4,
       0,   0,   2,  13,   2,   0,
       7,   0,  19,   0,   0,   2,
       0,
};

uint32_t hz_ucd_arabic_joining_ucs[313] = {
        1808,    1833,    1539,    1900,    2016,    1572,
        1595,    1606,    1885,    1711,    1590,    1683,
        1544,    1661,    1676,    1686,    2134,    1685,
        2022,    1652,    1745,    2118,    1882,    1787,
        2000,    2001,    1890,    1653,    1708,    1647,
        1738,    1671,    1649,    1908,    1669,    1675,
        1574,    1600,    1825,    1575,    1679,    1536,
        2024,    1836,    1912,    1662,    1596,    1579,
        1603,    1818,    1896,    1691,    1607,    1919,
        1578,    2120,    1581,    2125,    1568,    1698,
        1712,    2115,    1587,    1601,    1876,    1700,
        1571,    1569,    1740,    1547,    2009,    1746,
        1682,    1696,    1687,    1599,    2011,    1714,
        1883,    1570,    1594,    1871,    1718,    1663,
        1994,    1811,    1734,    1813,    1828,    2129,
        1819,    1703,    2208,    1591,    1697,    1695,
        1694,    1584,    2005,    2130,    1706,    1585,
        1656,    1888,    1592,    1911,    1660,    1582,
        2019,    1651,    1877,    1677,    1667,    1604,
        1914,    2212,    1576,    1878,    1702,    1681,
        1690,    1717,    1583,    1743,    1869,    1657,
        2116,    1832,    1903,    1672,    1729,    1719,
        1707,    1732,    1540,       0,    1699,    1668,
        1704,    1646,    1786,    1820,    1909,    1757,
        2006,    1693,    2007,    1735,    1873,    1726,
        1917,    1915,    1826,    1597,    1665,    1827,
        2023,    1701,    1689,    1588,    1710,    2213,
        1598,    2013,    1733,    1593,    1721,    1728,
        1666,    8205,    1996,    1664,    1586,    1692,
        2113,    2025,    1875,    1573,    1812,    1589,
        1709,    1537,    1736,    2042,    1817,    2132,
        1747,    1829,    1725,    2014,    1997,    1884,
        1742,    1680,    1723,    1916,    2215,    1580,
        1650,    2018,    1889,    2123,    1684,    2214,
        2126,    1892,    1577,    1886,    1907,    1822,
        1874,    1602,    1737,    1730,    1788,    1775,
        1749,    1839,    1895,    1658,    2017,    1880,
        1814,    1678,    1608,    2119,    2218,    1904,
        1816,    1774,    1720,    1727,    2008,    2122,
        1893,    2020,    2210,    2004,    1891,    1894,
        1838,    1741,    1910,    1821,    1906,    1831,
        2114,    1739,    1673,    2216,    1659,    2211,
        2127,    1918,    1610,    1881,    2135,    1724,
        1830,    1654,    1897,    1731,    1834,    1705,
        2136,    2003,    2002,    1887,    1913,    1898,
        1999,    2026,    1715,    1810,    2128,    2117,
        1609,    1716,    1722,    1837,    1815,    1879,
        2021,    1901,    1791,    1870,    2012,    2219,
        2015,    2010,    1713,    1605,    2220,    1744,
        1995,    1998,    1905,    2121,    2217,    1674,
        1670,    1899,    2133,    2131,    1823,    1655,
        1824,    2112,    1538,    1835,    1688,    1902,
        1872,
};

uint32_t hz_ucd_arabic_joining_data[313] = {
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALAPH << 6), /* ALAPH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_QAPH << 6), /* QAPH */
    HZ_JOINING_TYPE_U, /* ARABIC SIGN SAFHA */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH HAMZA ABOVE */
    HZ_JOINING_TYPE_D, /* NKO NA WOLOSO */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH HAMZA ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* KEHEH WITH 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NOON << 6), /* NOON */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_AIN << 6), /* AIN WITH 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* GAF */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SAD << 6), /* SAD WITH DOT ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH ATTACHED RING BELOW */
    HZ_JOINING_TYPE_U, /* ARABIC RAY */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH WITH INVERTED 3 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH 2 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH DOT BELOW AND DOT WITHIN */
    HZ_JOINING_TYPE_U, /* MANDAIC DUSHENNA */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH V BELOW */
    HZ_JOINING_TYPE_D, /* NKO YA */
    HZ_JOINING_TYPE_U, /* HIGH HAMZA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* DOTLESS YEH WITH 3 DOTS BELOW */
    HZ_JOINING_TYPE_R, /* MANDAIC AZ */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH INVERTED V BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SAD << 6), /* SAD WITH DOT BELOW AND DOT ABOVE */
    HZ_JOINING_TYPE_D, /* NKO O */
    HZ_JOINING_TYPE_D, /* NKO DAGBASINNA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* KEHEH WITH DOT ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* HIGH HAMZA ALEF */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_KAF << 6), /* KAF WITH DOT ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_QAF << 6), /* DOTLESS QAF */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH 4 DOTS BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* ALEF WITH WASLA ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* ALEF WITH DIGIT THREE ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH DOT BELOW AND TAH ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* DOTLESS YEH WITH HAMZA ABOVE */
    HZ_JOINING_TYPE_C, /* TATWEEL */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_MIM << 6), /* MIM */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* ALEF */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH INVERTED 3 DOTS ABOVE */
    HZ_JOINING_TYPE_U, /* ARABIC NUMBER SIGN */
    HZ_JOINING_TYPE_D, /* NKO JONA JA */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_TAW << 6), /* TAW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH DIGIT TWO ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH WITH 3 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* KEHEH WITH 3 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_KAF << 6), /* KAF */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HETH << 6), /* HETH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NOON << 6), /* NOON WITH TAH ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN WITH 3 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HEH << 6), /* HEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_KAF << 6), /* KAF WITH 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH WITH 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D, /* MANDAIC ATT */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH */
    HZ_JOINING_TYPE_D, /* MANDAIC AN */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* DOTLESS YEH WITH SEPARATE RING BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* DOTLESS FEH WITH DOT BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* GAF WITH ATTACHED RING BELOW */
    HZ_JOINING_TYPE_D, /* MANDAIC AD */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* FEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH WITH 2 DOTS BELOW AND DOT ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* DOTLESS FEH WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* ALEF WITH HAMZA ABOVE */
    HZ_JOINING_TYPE_U, /* HAMZA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FARSI << 6), /* FARSI YEH */
    HZ_JOINING_TYPE_U, /* AFGHANI SIGN */
    HZ_JOINING_TYPE_D, /* NKO RA */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_YEH << 6), /* YEH BARREE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH V ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_AIN << 6), /* AIN WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FARSI << 6), /* FARSI YEH WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D, /* NKO SA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* GAF WITH 2 DOTS BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH BAR */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* ALEF WITH MADDA ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_AIN << 6), /* AIN WITH DOT ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FE << 6), /* SOGDIAN FE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_LAM << 6), /* LAM WITH DOT ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH WITH 4 DOTS ABOVE */
    HZ_JOINING_TYPE_D, /* NKO A */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAMAL << 6), /* GAMAL */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH V ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DALATH << 6), /* DALATH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FINAL << 6), /* FINAL SEMKATH */
    HZ_JOINING_TYPE_D, /* MANDAIC ASZ */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_TETH << 6), /* TETH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_QAF << 6), /* DOTLESS QAF WITH DOT ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH WITH V BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_TAH << 6), /* TAH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* DOTLESS FEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_TAH << 6), /* TAH WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SAD << 6), /* SAD WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH DOT ABOVE */
    HZ_JOINING_TYPE_D, /* NKO TA */
    HZ_JOINING_TYPE_D, /* MANDAIC AQ */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SWASH << 6), /* SWASH KAF */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* HIGH HAMZA DOTLESS YEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* DOTLESS FEH WITH 2 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_TAH << 6), /* TAH WITH DOT ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* DOTLESS YEH WITH DIGIT FOUR BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH WITH ATTACHED RING BELOW AND 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH DOT ABOVE */
    HZ_JOINING_TYPE_D, /* NKO NA */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* ALEF WITH WAVY HAMZA BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH WITH INVERTED V BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH 2 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH 2 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_LAM << 6), /* LAM */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BURUSHASKI << 6), /* BURUSHASKI YEH BARREE WITH DIGIT TWO ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* DOTLESS FEH WITH DOT BELOW AND 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* BEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH WITH V ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* DOTLESS FEH WITH 4 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH TAH ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN WITH DOT BELOW AND DOT ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_LAM << 6), /* LAM WITH V ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH DOT ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ZHAIN << 6), /* SOGDIAN ZHAIN */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH WITH TAH ABOVE */
    HZ_JOINING_TYPE_D, /* MANDAIC AH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_SADHE << 6), /* SADHE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH TAH AND 2 DOTS BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH TAH ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HEH << 6), /* HEH GOAL */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_LAM << 6), /* LAM WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* KEHEH WITH ATTACHED RING BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH ATTACHED RING WITHIN */
    HZ_JOINING_TYPE_U, /* ARABIC SIGN SAMVAT */
    HZ_JOINING_TYPE_  | (HZ_JOINING_GROUP_ << 6),
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* FEH WITH DOT BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH VERTICAL 2 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_QAF << 6), /* DOTLESS QAF WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN WITH DOT BELOW AND 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_TETH << 6), /* TETH GARSHUNI */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FARSI << 6), /* FARSI YEH WITH DIGIT TWO ABOVE */
    HZ_JOINING_TYPE_U, /* ARABIC END OF AYAH */
    HZ_JOINING_TYPE_D, /* NKO JA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SAD << 6), /* SAD WITH 2 DOTS BELOW */
    HZ_JOINING_TYPE_D, /* NKO CHA */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH DAMMA ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* BEH WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_KNOTTED << 6), /* KNOTTED HEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN WITH DIGIT FOUR ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BURUSHASKI << 6), /* BURUSHASKI YEH BARREE WITH DIGIT THREE ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NUN << 6), /* NUN */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FARSI << 6), /* FARSI YEH WITH INVERTED V ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH HAMZA ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEMKATH << 6), /* SEMKATH */
    HZ_JOINING_TYPE_D, /* NKO NYA WOLOSO */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* DOTLESS FEH WITH 3 DOTS BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH 4 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_KAF << 6), /* KAF WITH 3 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_QAF << 6), /* QAF WITH DOT BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FARSI << 6), /* FARSI YEH WITH 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D, /* NKO FA */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH BAR */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_AIN << 6), /* AIN */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NOON << 6), /* NOON WITH DOT BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_TEH << 6), /* DOTLESS TEH MARBUTA WITH HAMZA ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH VERTICAL 2 DOTS ABOVE */
    HZ_JOINING_TYPE_C, /* ZERO WIDTH JOINER */
    HZ_JOINING_TYPE_D, /* NKO I */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH WITH 4 DOTS BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH DOT ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN WITH 3 DOTS BELOW AND 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D, /* MANDAIC AB */
    HZ_JOINING_TYPE_D, /* NKO JONA CHA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH WITH INVERTED 3 DOTS BELOW AND 2 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* ALEF WITH HAMZA BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAMAL << 6), /* GAMAL GARSHUNI */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SAD << 6), /* SAD */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_KAF << 6), /* KAF WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_U, /* ARABIC SIGN SANAH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH ALEF ABOVE */
    HZ_JOINING_TYPE_C, /* NKO LAJANYALAN */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ZAIN << 6), /* ZAIN */
    HZ_JOINING_TYPE_R, /* MANDAIC ASH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_YEH << 6), /* YEH BARREE WITH HAMZA ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_E << 6), /* E */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NYA << 6), /* NYA */
    HZ_JOINING_TYPE_D, /* NKO KA */
    HZ_JOINING_TYPE_D, /* NKO E */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN WITH 4 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FARSI << 6), /* FARSI YEH WITH V ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH 4 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NOON << 6), /* DOTLESS NOON WITH TAH ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH DIGIT FOUR BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_MEEM << 6), /* MEEM WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH DOT BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* ALEF WITH WAVY HAMZA ABOVE */
    HZ_JOINING_TYPE_D, /* NKO NYA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* DOTLESS FEH WITH INVERTED 3 DOTS BELOW */
    HZ_JOINING_TYPE_D, /* MANDAIC AL */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH DOT BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_LAM << 6), /* LAM WITH DOUBLE BAR */
    HZ_JOINING_TYPE_D, /* MANDAIC AS */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* KEHEH WITH INVERTED 3 DOTS BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_TEH << 6), /* TEH MARBUTA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_AIN << 6), /* AIN WITH INVERTED 3 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* ALEF WITH DIGIT TWO ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_YUDH << 6), /* YUDH HE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH WITH INVERTED 3 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_QAF << 6), /* QAF */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH INVERTED V ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HEH << 6), /* HEH GOAL WITH HAMZA ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_AIN << 6), /* AIN WITH DOT BELOW AND DOT ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH INVERTED V ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_TEH << 6), /* DOTLESS TEH MARBUTA */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DALATH << 6), /* PERSIAN DHALATH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NOON << 6), /* NOON WITH 2 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH WITH VERTICAL 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D, /* NKO MA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH INVERTED 3 DOTS BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DALATH << 6), /* DOTLESS DALATH RISH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW */
    HZ_JOINING_TYPE_D, /* MANDAIC IT */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH LOOP */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN WITH 2 DOTS AND TAH ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_SYRIAC << 6), /* WAW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH INVERTED V ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_LAM << 6), /* LAM WITH 3 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH 3 DOTS BELOW AND DOT ABOVE */
    HZ_JOINING_TYPE_D, /* NKO DA */
    HZ_JOINING_TYPE_D, /* MANDAIC AK */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_MEEM << 6), /* MEEM WITH DOT ABOVE */
    HZ_JOINING_TYPE_D, /* NKO HA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH DOT BELOW AND 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D, /* NKO PA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* KEHEH WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_MEEM << 6), /* MEEM WITH DOT BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAMAL << 6), /* PERSIAN GHAMAL */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_YEH << 6), /* YEH WITH TAIL */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FARSI << 6), /* FARSI YEH WITH DIGIT THREE ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YUDH << 6), /* YUDH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH TAH ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_REVERSED << 6), /* REVERSED PE */
    HZ_JOINING_TYPE_D, /* MANDAIC AG */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH ATTACHED RING BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* YEH WITH HAMZA ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH WITH VERTICAL 2 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_TAH << 6), /* TAH WITH 2 DOTS ABOVE */
    HZ_JOINING_TYPE_R, /* MANDAIC IN */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN WITH INVERTED V ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* YEH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH VERTICAL 2 DOTS BELOW AND TAH ABOVE */
    HZ_JOINING_TYPE_U, /* MANDAIC KAD */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NOON << 6), /* NOON WITH ATTACHED RING BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_PE << 6), /* PE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* HIGH HAMZA WAW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NOON << 6), /* NOON WITH V ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_TEH << 6), /* TEH MARBUTA GOAL */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DALATH << 6), /* RISH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* KEHEH */
    HZ_JOINING_TYPE_U, /* MANDAIC AIN */
    HZ_JOINING_TYPE_D, /* NKO BA */
    HZ_JOINING_TYPE_D, /* NKO N */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_AIN << 6), /* AIN WITH VERTICAL 2 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH DIGIT THREE ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_LAM << 6), /* LAM WITH BAR */
    HZ_JOINING_TYPE_D, /* NKO OO */
    HZ_JOINING_TYPE_D, /* NKO JONA RA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* GAF WITH VERTICAL 2 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BETH << 6), /* BETH */
    HZ_JOINING_TYPE_D, /* MANDAIC AP */
    HZ_JOINING_TYPE_D, /* MANDAIC USHENNA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* DOTLESS YEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* GAF WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NOON << 6), /* DOTLESS NOON */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BETH << 6), /* PERSIAN BHETH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_HE << 6), /* HE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D, /* NKO WA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN WITH VERTICAL 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_KNOTTED << 6), /* KNOTTED HEH WITH INVERTED V ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_KHAPH << 6), /* SOGDIAN KHAPH */
    HZ_JOINING_TYPE_D, /* NKO GBA */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH DOT WITHIN */
    HZ_JOINING_TYPE_D, /* NKO LA */
    HZ_JOINING_TYPE_D, /* NKO RRA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* GAF WITH 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_MEEM << 6), /* MEEM */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ROHINGYA << 6), /* ROHINGYA YEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* DOTLESS YEH WITH VERTICAL 2 DOTS BELOW */
    HZ_JOINING_TYPE_D, /* NKO EE */
    HZ_JOINING_TYPE_D, /* NKO U */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH 2 DOTS AND TAH ABOVE */
    HZ_JOINING_TYPE_R, /* MANDAIC AKSA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* YEH WITH DOT ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH DOT BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH 3 DOTS BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH VERTICAL 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D, /* MANDAIC AT */
    HZ_JOINING_TYPE_D, /* MANDAIC AR */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_KAPH << 6), /* KAPH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* HIGH HAMZA WAW WITH DAMMA ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_LAMADH << 6), /* LAMADH */
    HZ_JOINING_TYPE_R, /* MANDAIC HALQA */
    HZ_JOINING_TYPE_U, /* ARABIC FOOTNOTE MARKER */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SHIN << 6), /* SHIN */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH TAH BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH WITH HORIZONTAL 3 DOTS BELOW */
};

#endif /* HZ_UCD_6_2_0_H */