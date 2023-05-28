#ifndef HZ_UCD_4_1_0_H
#define HZ_UCD_4_1_0_H

#include <stdint.h>

#define HZ_UCD_VERSION HZ_MAKE_VERSION(4,1,0)

uint16_t hz_ucd_arabic_joining_k2[216] = {
      12,  53,   0,  18,   0,   1,
       0,   0,  31,   0,  19,   1,
       0,  10,   0,   2,   0,   1,
       3,   0,  12,   2,   0,   0,
      15,   0,   2,   0,   1,   3,
       0,   0,   1,  18,   0,   3,
       2,   0,   1,   0,   0,   0,
       3,   0,   1,  19,   2,   1,
       3,   0,   4,   3,   0,   4,
       3,   0,   0,   0,   2,   0,
       1,   0,   0,   1,   2,   2,
      10,   0,  11,   0,   0,   3,
      12,   0,   3,   0,   0,  12,
       7,   1,   2,  10,   2,   0,
       4,  13,   2,   0,   0,   0,
       6,   0,   0,   1,   0,   4,
       0,   0,   3,   1,   2,   0,
       0,   0,   1,  10,  14,   4,
       6,   5,   0,   9,   1,  27,
       1,  15,   1,  21,   0,   0,
       3,   2,   3,   2,   2,   3,
      19,   1,   0,   4,   0,   1,
      19,   0,   6,   0,  19,  35,
       1,   4,   4,  13,   3,  16,
       0,   1,   2,   2,  12,   0,
       1,   0,   0,   0,   0,   4,
       7,   0,   3,   1,   3,   4,
       7,   5,   4,   0,   0,   4,
       0,   5,   6,   0,   0,   1,
       5,   2,   1,   3,   1,   3,
       1,   4,   1,   0,   0,   0,
       1,   2,   1,   0,   0,   2,
       4,   0,   1,   1,   2,   3,
       5,  17,   0,   1,   2,   4,
       0,   1,   0,   1,  11,   0,
       2,   0,   3,   1,   2,   0,
};

uint32_t hz_ucd_arabic_joining_ucs[216] = {
        1594,    1685,    1696,    1741,    1590,    1610,
        1877,    1711,    1811,    1879,    1606,    1579,
        1574,    1603,    1654,    1663,    1609,    1572,
        1675,    1725,    1570,    1587,    1569,    1589,
        1577,    1537,    1873,    1733,    1659,    1692,
        1687,    1608,    1820,    1704,    1749,    1699,
        1647,    1581,    1672,    1730,    1721,    1870,
        1889,    1788,    1712,    1836,    1578,    1833,
        1660,    1728,    1575,    1717,    1691,    1740,
        1705,    1650,    1901,    1683,    1815,    1605,
        1591,    1887,    1729,    1832,    1661,    1547,
        1747,    1817,    1707,    1710,    1895,    1677,
        1831,    1588,    1694,    1600,       0,    1819,
        1709,    1693,    1536,    1652,    1881,    1876,
        1580,    1878,    1604,    1662,    1734,    1656,
        1664,    1651,    1576,    1757,    1593,    1667,
        1882,    1839,    1824,    1880,    1714,    1716,
        1582,    1671,    1680,    1700,    1718,    1681,
        1838,    1665,    1739,    1676,    1592,    1538,
        1884,    1890,    1658,    1897,    1669,    1731,
        1818,    1690,    1869,    1678,    1583,    1539,
        1825,    1602,    1682,    1875,    1774,    1885,
        1698,    1787,    1701,    1679,    1823,    1573,
        1697,    1813,    1653,    1715,    1736,    1888,
        1695,    1713,    1607,    1724,    1684,    1601,
        1810,    1837,    1727,    1827,    1723,    1822,
        1826,    1737,    1872,    1722,    1668,    1688,
        8205,    1874,    1745,    1883,    1775,    1814,
        1900,    1899,    1898,    1732,    1830,    1703,
        1786,    1886,    1834,    1726,    1706,    1666,
        1674,    1835,    1720,    1816,    1746,    1571,
        1821,    1744,    1649,    1891,    1670,    1702,
        1657,    1812,    1735,    1646,    1738,    1893,
        1689,    1584,    1829,    1673,    1791,    1743,
        1742,    1892,    1585,    1828,    1808,    1708,
        1719,    1896,    1586,    1894,    1655,    1871,
};

uint32_t hz_ucd_arabic_joining_data[216] = {
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_AIN << 6), /* GHAIN */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH SMALL V BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_AIN << 6), /* AIN WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_YEH << 6), /* YEH WITH TAIL */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SAD << 6), /* DAD */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* YEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* BEH WITH INVERTED SMALL V BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* GAF */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAMAL << 6), /* GAMAL */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NOON << 6), /* NOON */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* THEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* HAMZA ON YEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_KAF << 6), /* KAF */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* HIGH HAMZA WAW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* TEH WITH 4 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* ALEF MAKSURA */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* HAMZA ON WAW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH DOT BELOW AND SMALL TAH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NOON << 6), /* NOON WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* MADDA ON ALEF */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN */
    HZ_JOINING_TYPE_U, /* HAMZA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SAD << 6), /* SAD */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_TEH << 6), /* TEH MARBUTA */
    HZ_JOINING_TYPE_U, /* ARABIC SIGN SANAH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* BEH WITH DOT BELOW AND 3 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH BAR */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* BEH WITH 2 DOTS VERTICAL BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN WITH 3 DOTS BELOW AND 3 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH 2 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_TETH << 6), /* TETH GARSHUNI */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_QAF << 6), /* QAF WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_TEH << 6), /* AE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* FEH WITH DOT BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_QAF << 6), /* DOTLESS QAF */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH SMALL TAH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HEH << 6), /* HAMZA ON HEH GOAL */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NOON << 6), /* NOON WITH DOT BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_KHAPH << 6), /* SOGDIAN KHAPH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* FEH WITH 3 DOTS POINTING UPWARDS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_AIN << 6), /* GHAIN WITH DOT BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* GAF WITH RING */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_TAW << 6), /* TAW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* TEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_QAPH << 6), /* QAPH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* TEH WITH RING */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_TEH << 6), /* HAMZA ON HEH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* ALEF */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_LAM << 6), /* LAM WITH SMALL V */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN WITH 3 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* DOTLESS YEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* KEHEH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* WAVY HAMZA ON ALEF */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN WITH 2 DOTS VERTICALLY ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH RING */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_HE << 6), /* HE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_MEEM << 6), /* MEEM */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_TAH << 6), /* TAH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_AIN << 6), /* AIN WITH 2 DOTS VERTICALLY ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HEH << 6), /* HEH GOAL */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_SADHE << 6), /* SADHE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* TEH WITH 3 DOTS ABOVE DOWNWARD */
    HZ_JOINING_TYPE_U, /* AFGHANI SIGN */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_YEH << 6), /* HAMZA ON YEH BARREE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ZAIN << 6), /* ZAIN */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* KAF WITH RING */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_KAF << 6), /* KAF WITH 3 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NOON << 6), /* NOON WITH 2 DOTS BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH 2 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_REVERSED << 6), /* REVERSED PE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SHEEN */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SAD << 6), /* SAD WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_C, /* TATWEEL */
    HZ_JOINING_TYPE_  | (HZ_JOINING_GROUP_ << 6),
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_TETH << 6), /* TETH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_KAF << 6), /* KAF WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SAD << 6), /* SAD WITH 2 DOTS BELOW */
    HZ_JOINING_TYPE_U, /* ARABIC NUMBER SIGN */
    HZ_JOINING_TYPE_U, /* HIGH HAMZA */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH 2 DOTS VERTICALLY BELOW AND SMALL TAH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* BEH WITH 2 DOTS BELOW AND DOT ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* JEEM */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* BEH WITH SMALL V */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_LAM << 6), /* LAM */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* TEH WITH 3 DOTS BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH SMALL V */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* HIGH HAMZA YEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* BEH WITH 4 DOTS BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* WAVY HAMZA UNDER ALEF */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* BEH */
    HZ_JOINING_TYPE_U, /* ARABIC END OF AYAH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_AIN << 6), /* AIN */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH MIDDLE 2 DOTS */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH INVERTED SMALL V BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DALATH << 6), /* PERSIAN DHALATH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_LAMADH << 6), /* LAMADH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH 3 DOTS POINTING UPWARDS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* GAF WITH 2 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* GAF WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* KHAH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH MIDDLE 4 DOTS */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH 4 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* FEH WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_LAM << 6), /* LAM WITH DOT ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH SMALL TAH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAMAL << 6), /* PERSIAN GHAMAL */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAMZA ON HAH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_TAH << 6), /* ZAH */
    HZ_JOINING_TYPE_U, /* ARABIC FOOTNOTE MARKER */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN WITH 4 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* KEHEH WITH DOT ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* TEH WITH 2 DOTS VERTICAL ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NOON << 6), /* NOON WITH SMALL V */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_HAMZA << 6), /* TEH MARBUTA GOAL */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HETH << 6), /* HETH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN WITH DOT BELOW AND DOT ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ZHAIN << 6), /* SOGDIAN ZHAIN */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL */
    HZ_JOINING_TYPE_U, /* ARABIC SIGN SAFHA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_MIM << 6), /* MIM */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_QAF << 6), /* QAF */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH SMALL V */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* BEH WITH 3 DOTS POINTING UPWARDS BELOW AND 2 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH INVERTED V */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_AIN << 6), /* AIN WITH 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* FEH WITH DOT MOVED BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SAD << 6), /* DAD WITH DOT BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* FEH WITH 3 DOTS BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH 3 DOTS ABOVE DOWNWARD */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_KAPH << 6), /* KAPH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* HAMZA UNDER ALEF */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* DOTLESS FEH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DALATH << 6), /* DALATH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* HIGH HAMZA ALEF */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* GAF WITH 2 DOTS VERTICAL BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH ALEF ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* FEH WITH 2 DOTS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_TAH << 6), /* TAH WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* GAF WITH 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HEH << 6), /* HEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NOON << 6), /* NOON WITH RING */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH DOT BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* FEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BETH << 6), /* BETH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BETH << 6), /* PERSIAN BHETH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH MIDDLE 3 DOTS DOWNWARD AND DOT ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEMKATH << 6), /* SEMKATH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NOON << 6), /* DOTLESS NOON WITH SMALL TAH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_YUDH << 6), /* YUDH HE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NUN << 6), /* NUN */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH INVERTED SMALL V */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* BEH WITH 3 DOTS HORIZONTALLY BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NOON << 6), /* DOTLESS NOON */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH MIDDLE 2 DOTS VERTICAL */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_C, /* ZERO WIDTH JOINER */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* BEH WITH 3 DOTS POINTING UPWARDS BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* YEH WITH 3 DOTS BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH STROKE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH INVERTED V */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DALATH << 6), /* DOTLESS DALATH RISH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH HAMZA ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH 2 DOTS VERTICALLY ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_LAM << 6), /* LAM WITH BAR */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH RING */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_PE << 6), /* PE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_QAF << 6), /* QAF WITH DOT ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SEEN << 6), /* SEEN WITH DOT BELOW AND 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_AIN << 6), /* AIN WITH 3 DOTS POINTING DOWNWARDS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DALATH << 6), /* RISH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_KNOTTED << 6), /* KNOTTED HEH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SWASH << 6), /* SWASH KAF */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH 2 DOTS VERTICAL ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH DOT BELOW */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_SHIN << 6), /* SHIN */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_LAM << 6), /* LAM WITH 3 DOTS BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_SYRIAC << 6), /* WAW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_YEH << 6), /* YEH BARREE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* HAMZA ON ALEF */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YUDH << 6), /* YUDH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* YEH WITH 2 DOTS VERTICAL BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALEF << 6), /* HAMZAT WASL ON ALEF */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* KEHEH WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_HAH << 6), /* HAH WITH MIDDLE 3 DOTS DOWNWARD */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FEH << 6), /* FEH WITH 4 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* TEH WITH SMALL TAH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAMAL << 6), /* GAMAL GARSHUNI */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH DAMMA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_BEH << 6), /* DOTLESS BEH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH 2 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_MEEM << 6), /* MEEM WITH DOT ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH WITH 4 DOTS ABOVE */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* THAL */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_E << 6), /* E */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_DAL << 6), /* DAL WITH RING */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_KNOTTED << 6), /* HEH WITH INVERTED V */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* WAW WITH DOT ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_YEH << 6), /* YEH WITH SMALL V */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_GAF << 6), /* KEHEH WITH 3 DOTS POINTING UPWARDS BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* REH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FINAL << 6), /* FINAL SEMKATH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_ALAPH << 6), /* ALAPH */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_KAF << 6), /* KAF WITH DOT ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_LAM << 6), /* LAM WITH 3 DOTS ABOVE */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_NOON << 6), /* NOON WITH SMALL TAH */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_REH << 6), /* ZAIN */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_MEEM << 6), /* MEEM WITH DOT BELOW */
    HZ_JOINING_TYPE_R | (HZ_JOINING_GROUP_WAW << 6), /* HIGH HAMZA WAW WITH DAMMA */
    HZ_JOINING_TYPE_D | (HZ_JOINING_GROUP_FE << 6), /* SOGDIAN FE */
};

#endif /* HZ_UCD_4_1_0_H */