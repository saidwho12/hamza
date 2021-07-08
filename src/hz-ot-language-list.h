#ifndef HZ_OT_LANGUAGE_LIST_H
#define HZ_OT_LANGUAGE_LIST_H

#include "hz-base.h"

typedef enum hz_language_t {
    HZ_LANGUAGE_DFLT,
    HZ_LANGUAGE_ABAZA, /* Abaza */ 
    HZ_LANGUAGE_ABKHAZIAN, /* Abkhazian */ 
    HZ_LANGUAGE_ACHOLI, /* Acholi */ 
    HZ_LANGUAGE_ACHI, /* Achi */ 
    HZ_LANGUAGE_ADYGHE, /* Adyghe */ 
    HZ_LANGUAGE_AFRIKAANS, /* Afrikaans */ 
    HZ_LANGUAGE_AFAR, /* Afar */ 
    HZ_LANGUAGE_AGAW, /* Agaw */ 
    HZ_LANGUAGE_AITON, /* Aiton */ 
    HZ_LANGUAGE_AKAN, /* Akan */ 
    HZ_LANGUAGE_BATAK_ANGKOLA, /* Batak Angkola */ 
    HZ_LANGUAGE_ALSATIAN, /* Alsatian */ 
    HZ_LANGUAGE_ALTAI, /* Altai */ 
    HZ_LANGUAGE_AMHARIC, /* Amharic */ 
    HZ_LANGUAGE_ANGLO_SAXON, /* Anglo-Saxon */ 
    HZ_LANGUAGE_PHONETIC_TRANSCRIPTION_AMERICANIST_CONVENTIONS, /* Phonetic transcription-Americanist conventions */ 
    HZ_LANGUAGE_ARABIC, /* Arabic */ 
    HZ_LANGUAGE_ARAGONESE, /* Aragonese */ 
    HZ_LANGUAGE_AARI, /* Aari */ 
    HZ_LANGUAGE_RAKHINE, /* Rakhine */ 
    HZ_LANGUAGE_ASSAMESE, /* Assamese */ 
    HZ_LANGUAGE_ASTURIAN, /* Asturian */ 
    HZ_LANGUAGE_ATHAPASKAN_LANGUAGES, /* Athapaskan languages */ 
    HZ_LANGUAGE_AVAR, /* Avar */ 
    HZ_LANGUAGE_AWADHI, /* Awadhi */ 
    HZ_LANGUAGE_AYMARA, /* Aymara */ 
    HZ_LANGUAGE_TORKI, /* Torki */ 
    HZ_LANGUAGE_AZERBAIJANI, /* Azerbaijani */ 
    HZ_LANGUAGE_BADAGA, /* Badaga */ 
    HZ_LANGUAGE_BANDA, /* Banda */ 
    HZ_LANGUAGE_BAGHELKHANDI, /* Baghelkhandi */ 
    HZ_LANGUAGE_BALKAR, /* Balkar */ 
    HZ_LANGUAGE_BALINESE, /* Balinese */ 
    HZ_LANGUAGE_BAVARIAN, /* Bavarian */ 
    HZ_LANGUAGE_BAULE, /* Baule */ 
    HZ_LANGUAGE_BATAK_TOBA, /* Batak Toba */ 
    HZ_LANGUAGE_BERBER, /* Berber */ 
    HZ_LANGUAGE_BENCH, /* Bench */ 
    HZ_LANGUAGE_BIBLE_CREE, /* Bible Cree */ 
    HZ_LANGUAGE_BANDJALANG, /* Bandjalang */ 
    HZ_LANGUAGE_BELARUSSIAN, /* Belarussian */ 
    HZ_LANGUAGE_BEMBA, /* Bemba */ 
    HZ_LANGUAGE_BENGALI, /* Bengali */ 
    HZ_LANGUAGE_HARYANVI, /* Haryanvi */ 
    HZ_LANGUAGE_BAGRI, /* Bagri */ 
    HZ_LANGUAGE_BULGARIAN, /* Bulgarian */ 
    HZ_LANGUAGE_BHILI, /* Bhili */ 
    HZ_LANGUAGE_BHOJPURI, /* Bhojpuri */ 
    HZ_LANGUAGE_BIKOL, /* Bikol */ 
    HZ_LANGUAGE_BILEN, /* Bilen */ 
    HZ_LANGUAGE_BISLAMA, /* Bislama */ 
    HZ_LANGUAGE_KANAUJI, /* Kanauji */ 
    HZ_LANGUAGE_BLACKFOOT, /* Blackfoot */ 
    HZ_LANGUAGE_BALUCHI, /* Baluchi */ 
    HZ_LANGUAGE_PAO_KAREN, /* Pao Karen */ 
    HZ_LANGUAGE_BALANTE, /* Balante */ 
    HZ_LANGUAGE_BALTI, /* Balti */ 
    HZ_LANGUAGE_BAMBARA_BAMANANKAN, /* Bambara (Bamanankan) */ 
    HZ_LANGUAGE_BAMILEKE, /* Bamileke */ 
    HZ_LANGUAGE_BOSNIAN, /* Bosnian */ 
    HZ_LANGUAGE_BISHNUPRIYA_MANIPURI, /* Bishnupriya Manipuri */ 
    HZ_LANGUAGE_BRETON, /* Breton */ 
    HZ_LANGUAGE_BRAHUI, /* Brahui */ 
    HZ_LANGUAGE_BRAJ_BHASHA, /* Braj Bhasha */ 
    HZ_LANGUAGE_BURMESE, /* Burmese */ 
    HZ_LANGUAGE_BODO, /* Bodo */ 
    HZ_LANGUAGE_BASHKIR, /* Bashkir */ 
    HZ_LANGUAGE_BURUSHASKI, /* Burushaski */ 
    HZ_LANGUAGE_BATAK_DAIRI_PAKPAK, /* Batak Dairi (Pakpak) */ 
    HZ_LANGUAGE_BETI, /* Beti */ 
    HZ_LANGUAGE_BATAK_LANGUAGES, /* Batak languages */ 
    HZ_LANGUAGE_BATAK_MANDAILING, /* Batak Mandailing */ 
    HZ_LANGUAGE_BATAK_SIMALUNGUN, /* Batak Simalungun */ 
    HZ_LANGUAGE_BATAK_KARO, /* Batak Karo */ 
    HZ_LANGUAGE_BATAK_ALAS_KLUET, /* Batak Alas-Kluet */ 
    HZ_LANGUAGE_BUGIS, /* Bugis */ 
    HZ_LANGUAGE_MEDUMBA, /* Medumba */ 
    HZ_LANGUAGE_KAQCHIKEL, /* Kaqchikel */ 
    HZ_LANGUAGE_CATALAN, /* Catalan */ 
    HZ_LANGUAGE_ZAMBOANGA_CHAVACANO, /* Zamboanga Chavacano */ 
    HZ_LANGUAGE_CHINANTEC, /* Chinantec */ 
    HZ_LANGUAGE_CEBUANO, /* Cebuano */ 
    HZ_LANGUAGE_CHIGA, /* Chiga */ 
    HZ_LANGUAGE_CHAMORRO, /* Chamorro */ 
    HZ_LANGUAGE_CHECHEN, /* Chechen */ 
    HZ_LANGUAGE_CHAHA_GURAGE, /* Chaha Gurage */ 
    HZ_LANGUAGE_CHATTISGARHI, /* Chattisgarhi */ 
    HZ_LANGUAGE_CHICHEWA_CHEWA_NYANJA, /* Chichewa (Chewa, Nyanja) */ 
    HZ_LANGUAGE_CHUKCHI, /* Chukchi */ 
    HZ_LANGUAGE_CHUUKESE, /* Chuukese */ 
    HZ_LANGUAGE_CHOCTAW, /* Choctaw */ 
    HZ_LANGUAGE_CHIPEWYAN, /* Chipewyan */ 
    HZ_LANGUAGE_CHEROKEE, /* Cherokee */ 
    HZ_LANGUAGE_CHUVASH, /* Chuvash */ 
    HZ_LANGUAGE_CHEYENNE, /* Cheyenne */ 
    HZ_LANGUAGE_WESTERN_CHAM, /* Western Cham */ 
    HZ_LANGUAGE_EASTERN_CHAM, /* Eastern Cham */ 
    HZ_LANGUAGE_COMORIAN, /* Comorian */ 
    HZ_LANGUAGE_COPTIC, /* Coptic */ 
    HZ_LANGUAGE_CORNISH, /* Cornish */ 
    HZ_LANGUAGE_CORSICAN, /* Corsican */ 
    HZ_LANGUAGE_CREOLES, /* Creoles */ 
    HZ_LANGUAGE_CREE, /* Cree */ 
    HZ_LANGUAGE_CARRIER, /* Carrier */ 
    HZ_LANGUAGE_CRIMEAN_TATAR, /* Crimean Tatar */ 
    HZ_LANGUAGE_KASHUBIAN, /* Kashubian */ 
    HZ_LANGUAGE_CHURCH_SLAVONIC, /* Church Slavonic */ 
    HZ_LANGUAGE_CZECH, /* Czech */ 
    HZ_LANGUAGE_CHITTAGONIAN, /* Chittagonian */ 
    HZ_LANGUAGE_SAN_BLAS_KUNA, /* San Blas Kuna */ 
    HZ_LANGUAGE_DAGBANI, /* Dagbani */ 
    HZ_LANGUAGE_DANISH, /* Danish */ 
    HZ_LANGUAGE_DARGWA, /* Dargwa */ 
    HZ_LANGUAGE_DAYI, /* Dayi */ 
    HZ_LANGUAGE_WOODS_CREE, /* Woods Cree */ 
    HZ_LANGUAGE_GERMAN, /* German */ 
    HZ_LANGUAGE_DOGRI_INDIVIDUAL_LANGUAGE, /* Dogri (individual language) */ 
    HZ_LANGUAGE_DOGRI_MACROLANGUAGE, /* Dogri (macrolanguage) */ 
    HZ_LANGUAGE_DHANGU, /* Dhangu */ 
    HZ_LANGUAGE_DIVEHI_DHIVEHI_MALDIVIAN, /* Divehi (Dhivehi, Maldivian) */ 
    HZ_LANGUAGE_DIMLI, /* Dimli */ 
    HZ_LANGUAGE_ZARMA, /* Zarma */ 
    HZ_LANGUAGE_DJAMBARRPUYNGU, /* Djambarrpuyngu */ 
    HZ_LANGUAGE_DANGME, /* Dangme */ 
    HZ_LANGUAGE_DAN, /* Dan */ 
    HZ_LANGUAGE_DINKA, /* Dinka */ 
    HZ_LANGUAGE_DARI, /* Dari */ 
    HZ_LANGUAGE_DHUWAL, /* Dhuwal */ 
    HZ_LANGUAGE_DUNGAN, /* Dungan */ 
    HZ_LANGUAGE_DZONGKHA, /* Dzongkha */ 
    HZ_LANGUAGE_EBIRA, /* Ebira */ 
    HZ_LANGUAGE_EASTERN_CREE, /* Eastern Cree */ 
    HZ_LANGUAGE_EDO, /* Edo */ 
    HZ_LANGUAGE_EFIK, /* Efik */ 
    HZ_LANGUAGE_GREEK, /* Greek */ 
    HZ_LANGUAGE_EASTERN_MANINKAKAN, /* Eastern Maninkakan */ 
    HZ_LANGUAGE_ENGLISH, /* English */ 
    HZ_LANGUAGE_ERZYA, /* Erzya */ 
    HZ_LANGUAGE_SPANISH, /* Spanish */ 
    HZ_LANGUAGE_CENTRAL_YUPIK, /* Central Yupik */ 
    HZ_LANGUAGE_ESTONIAN, /* Estonian */ 
    HZ_LANGUAGE_BASQUE, /* Basque */ 
    HZ_LANGUAGE_EVENKI, /* Evenki */ 
    HZ_LANGUAGE_EVEN, /* Even */ 
    HZ_LANGUAGE_EWE, /* Ewe */ 
    HZ_LANGUAGE_FRENCH_ANTILLEAN, /* French Antillean */ 
    HZ_LANGUAGE_FANG, /* Fang */ 
    HZ_LANGUAGE_PERSIAN, /* Persian */ 
    HZ_LANGUAGE_FANTI, /* Fanti */ 
    HZ_LANGUAGE_FINNISH, /* Finnish */ 
    HZ_LANGUAGE_FIJIAN, /* Fijian */ 
    HZ_LANGUAGE_DUTCH_FLEMISH, /* Dutch (Flemish) */ 
    HZ_LANGUAGE_FEFE, /* Fefe */ 
    HZ_LANGUAGE_FOREST_ENETS, /* Forest Enets */ 
    HZ_LANGUAGE_FON, /* Fon */ 
    HZ_LANGUAGE_FAROESE, /* Faroese */ 
    HZ_LANGUAGE_FRENCH, /* French */ 
    HZ_LANGUAGE_CAJUN_FRENCH, /* Cajun French */ 
    HZ_LANGUAGE_FRISIAN, /* Frisian */ 
    HZ_LANGUAGE_FRIULIAN, /* Friulian */ 
    HZ_LANGUAGE_ARPITAN, /* Arpitan */ 
    HZ_LANGUAGE_FUTA, /* Futa */ 
    HZ_LANGUAGE_FULAH, /* Fulah */ 
    HZ_LANGUAGE_NIGERIAN_FULFULDE, /* Nigerian Fulfulde */ 
    HZ_LANGUAGE_GA, /* Ga */ 
    HZ_LANGUAGE_SCOTTISH_GAELIC_GAELIC, /* Scottish Gaelic (Gaelic) */ 
    HZ_LANGUAGE_GAGAUZ, /* Gagauz */ 
    HZ_LANGUAGE_GALICIAN, /* Galician */ 
    HZ_LANGUAGE_GARSHUNI, /* Garshuni */ 
    HZ_LANGUAGE_GARHWALI, /* Garhwali */ 
    HZ_LANGUAGE_GEEZ, /* Geez */ 
    HZ_LANGUAGE_GITHABUL, /* Githabul */ 
    HZ_LANGUAGE_GILYAK, /* Gilyak */ 
    HZ_LANGUAGE_KIRIBATI_GILBERTESE, /* Kiribati (Gilbertese) */ 
    HZ_LANGUAGE_KPELLE_GUINEA, /* Kpelle (Guinea) */ 
    HZ_LANGUAGE_GILAKI, /* Gilaki */ 
    HZ_LANGUAGE_GUMUZ, /* Gumuz */ 
    HZ_LANGUAGE_GUMATJ, /* Gumatj */ 
    HZ_LANGUAGE_GOGO, /* Gogo */ 
    HZ_LANGUAGE_GONDI, /* Gondi */ 
    HZ_LANGUAGE_GREENLANDIC, /* Greenlandic */ 
    HZ_LANGUAGE_GARO, /* Garo */ 
    HZ_LANGUAGE_GUARANI, /* Guarani */ 
    HZ_LANGUAGE_WAYUU, /* Wayuu */ 
    HZ_LANGUAGE_GUPAPUYNGU, /* Gupapuyngu */ 
    HZ_LANGUAGE_GUJARATI, /* Gujarati */ 
    HZ_LANGUAGE_GUSII, /* Gusii */ 
    HZ_LANGUAGE_HAITIAN_HAITIAN_CREOLE, /* Haitian (Haitian Creole) */ 
    HZ_LANGUAGE_HALAM_FALAM_CHIN, /* Halam (Falam Chin) */ 
    HZ_LANGUAGE_HARAUTI, /* Harauti */ 
    HZ_LANGUAGE_HAUSA, /* Hausa */ 
    HZ_LANGUAGE_HAWAIIAN, /* Hawaiian */ 
    HZ_LANGUAGE_HAYA, /* Haya */ 
    HZ_LANGUAGE_HAZARAGI, /* Hazaragi */ 
    HZ_LANGUAGE_HAMMER_BANNA, /* Hammer-Banna */ 
    HZ_LANGUAGE_HERERO, /* Herero */ 
    HZ_LANGUAGE_HILIGAYNON, /* Hiligaynon */ 
    HZ_LANGUAGE_HINDI, /* Hindi */ 
    HZ_LANGUAGE_HIGH_MARI, /* High Mari */ 
    HZ_LANGUAGE_HMONG, /* Hmong */ 
    HZ_LANGUAGE_HIRI_MOTU, /* Hiri Motu */ 
    HZ_LANGUAGE_HINDKO, /* Hindko */ 
    HZ_LANGUAGE_HO, /* Ho */ 
    HZ_LANGUAGE_HARARI, /* Harari */ 
    HZ_LANGUAGE_CROATIAN, /* Croatian */ 
    HZ_LANGUAGE_HUNGARIAN, /* Hungarian */ 
    HZ_LANGUAGE_ARMENIAN, /* Armenian */ 
    HZ_LANGUAGE_ARMENIAN_EAST, /* Armenian East */ 
    HZ_LANGUAGE_IBAN, /* Iban */ 
    HZ_LANGUAGE_IBIBIO, /* Ibibio */ 
    HZ_LANGUAGE_IGBO, /* Igbo */ 
    HZ_LANGUAGE_IDO, /* Ido */ 
    HZ_LANGUAGE_IJO_LANGUAGES, /* Ijo languages */ 
    HZ_LANGUAGE_INTERLINGUE, /* Interlingue */ 
    HZ_LANGUAGE_ILOKANO, /* Ilokano */ 
    HZ_LANGUAGE_INTERLINGUA, /* Interlingua */ 
    HZ_LANGUAGE_INDONESIAN, /* Indonesian */ 
    HZ_LANGUAGE_INGUSH, /* Ingush */ 
    HZ_LANGUAGE_INUKTITUT, /* Inuktitut */ 
    HZ_LANGUAGE_INUPIAT, /* Inupiat */ 
    HZ_LANGUAGE_PHONETIC_TRANSCRIPTION_IPA_CONVENTIONS, /* Phonetic transcription-IPA conventions */ 
    HZ_LANGUAGE_IRISH, /* Irish */ 
    HZ_LANGUAGE_IRISH_TRADITIONAL, /* Irish Traditional */ 
    HZ_LANGUAGE_ICELANDIC, /* Icelandic */ 
    HZ_LANGUAGE_INARI_SAMI, /* Inari Sami */ 
    HZ_LANGUAGE_ITALIAN, /* Italian */ 
    HZ_LANGUAGE_HEBREW, /* Hebrew */ 
    HZ_LANGUAGE_JAMAICAN_CREOLE, /* Jamaican Creole */ 
    HZ_LANGUAGE_JAPANESE, /* Japanese */ 
    HZ_LANGUAGE_JAVANESE, /* Javanese */ 
    HZ_LANGUAGE_LOJBAN, /* Lojban */ 
    HZ_LANGUAGE_KRYMCHAK, /* Krymchak */ 
    HZ_LANGUAGE_YIDDISH, /* Yiddish */ 
    HZ_LANGUAGE_LADINO, /* Ladino */ 
    HZ_LANGUAGE_JULA, /* Jula */ 
    HZ_LANGUAGE_KABARDIAN, /* Kabardian */ 
    HZ_LANGUAGE_KABYLE, /* Kabyle */ 
    HZ_LANGUAGE_KACHCHI, /* Kachchi */ 
    HZ_LANGUAGE_KALENJIN, /* Kalenjin */ 
    HZ_LANGUAGE_KANNADA, /* Kannada */ 
    HZ_LANGUAGE_KARACHAY, /* Karachay */ 
    HZ_LANGUAGE_GEORGIAN, /* Georgian */ 
    HZ_LANGUAGE_KAZAKH, /* Kazakh */ 
    HZ_LANGUAGE_MAKONDE, /* Makonde */ 
    HZ_LANGUAGE_KABUVERDIANU_CRIOULO, /* Kabuverdianu (Crioulo) */ 
    HZ_LANGUAGE_KEBENA, /* Kebena */ 
    HZ_LANGUAGE_KEKCHI, /* Kekchi */ 
    HZ_LANGUAGE_KHUTSURI_GEORGIAN, /* Khutsuri Georgian */ 
    HZ_LANGUAGE_KHAKASS, /* Khakass */ 
    HZ_LANGUAGE_KHANTY_KAZIM, /* Khanty-Kazim */ 
    HZ_LANGUAGE_KHMER, /* Khmer */ 
    HZ_LANGUAGE_KHANTY_SHURISHKAR, /* Khanty-Shurishkar */ 
    HZ_LANGUAGE_KHAMTI_SHAN, /* Khamti Shan */ 
    HZ_LANGUAGE_KHANTY_VAKHI, /* Khanty-Vakhi */ 
    HZ_LANGUAGE_KHOWAR, /* Khowar */ 
    HZ_LANGUAGE_KIKUYU_GIKUYU, /* Kikuyu (Gikuyu) */ 
    HZ_LANGUAGE_KIRGHIZ_KYRGYZ, /* Kirghiz (Kyrgyz) */ 
    HZ_LANGUAGE_KISII, /* Kisii */ 
    HZ_LANGUAGE_KIRMANJKI, /* Kirmanjki */ 
    HZ_LANGUAGE_SOUTHERN_KIWAI, /* Southern Kiwai */ 
    HZ_LANGUAGE_EASTERN_PWO_KAREN, /* Eastern Pwo Karen */ 
    HZ_LANGUAGE_BUMTHANGKHA, /* Bumthangkha */ 
    HZ_LANGUAGE_KOKNI, /* Kokni */ 
    HZ_LANGUAGE_KALMYK, /* Kalmyk */ 
    HZ_LANGUAGE_KAMBA, /* Kamba */ 
    HZ_LANGUAGE_KUMAONI, /* Kumaoni */ 
    HZ_LANGUAGE_KOMO, /* Komo */ 
    HZ_LANGUAGE_KOMSO, /* Komso */ 
    HZ_LANGUAGE_KHORASANI_TURKIC, /* Khorasani Turkic */ 
    HZ_LANGUAGE_KANURI, /* Kanuri */ 
    HZ_LANGUAGE_KODAGU, /* Kodagu */ 
    HZ_LANGUAGE_KOREAN_OLD_HANGUL, /* Korean Old Hangul */ 
    HZ_LANGUAGE_KONKANI, /* Konkani */ 
    HZ_LANGUAGE_KOMI, /* Komi */ 
    HZ_LANGUAGE_KIKONGO, /* Kikongo */ 
    HZ_LANGUAGE_KONGO, /* Kongo */ 
    HZ_LANGUAGE_KOMI_PERMYAK, /* Komi-Permyak */ 
    HZ_LANGUAGE_KOREAN, /* Korean */ 
    HZ_LANGUAGE_KOSRAEAN, /* Kosraean */ 
    HZ_LANGUAGE_KOMI_ZYRIAN, /* Komi-Zyrian */ 
    HZ_LANGUAGE_KPELLE, /* Kpelle */ 
    HZ_LANGUAGE_KRIO, /* Krio */ 
    HZ_LANGUAGE_KARAKALPAK, /* Karakalpak */ 
    HZ_LANGUAGE_KARELIAN, /* Karelian */ 
    HZ_LANGUAGE_KARAIM, /* Karaim */ 
    HZ_LANGUAGE_KAREN, /* Karen */ 
    HZ_LANGUAGE_KOORETE, /* Koorete */ 
    HZ_LANGUAGE_KASHMIRI, /* Kashmiri */ 
    HZ_LANGUAGE_RIPUARIAN, /* Ripuarian */ 
    HZ_LANGUAGE_KHASI, /* Khasi */ 
    HZ_LANGUAGE_KILDIN_SAMI, /* Kildin Sami */ 
    HZ_LANGUAGE_SGAW_KAREN, /* Sgaw Karen */ 
    HZ_LANGUAGE_KUANYAMA, /* Kuanyama */ 
    HZ_LANGUAGE_KUI, /* Kui */ 
    HZ_LANGUAGE_KULVI, /* Kulvi */ 
    HZ_LANGUAGE_KUMYK, /* Kumyk */ 
    HZ_LANGUAGE_KURDISH, /* Kurdish */ 
    HZ_LANGUAGE_KURUKH, /* Kurukh */ 
    HZ_LANGUAGE_KUY, /* Kuy */ 
    HZ_LANGUAGE_KORYAK, /* Koryak */ 
    HZ_LANGUAGE_WESTERN_KAYAH, /* Western Kayah */ 
    HZ_LANGUAGE_LADIN, /* Ladin */ 
    HZ_LANGUAGE_LAHULI, /* Lahuli */ 
    HZ_LANGUAGE_LAK, /* Lak */ 
    HZ_LANGUAGE_LAMBANI, /* Lambani */ 
    HZ_LANGUAGE_LAO, /* Lao */ 
    HZ_LANGUAGE_LATIN, /* Latin */ 
    HZ_LANGUAGE_LAZ, /* Laz */ 
    HZ_LANGUAGE_L_CREE, /* L-Cree */ 
    HZ_LANGUAGE_LADAKHI, /* Ladakhi */ 
    HZ_LANGUAGE_LEZGI, /* Lezgi */ 
    HZ_LANGUAGE_LIGURIAN, /* Ligurian */ 
    HZ_LANGUAGE_LIMBURGISH, /* Limburgish */ 
    HZ_LANGUAGE_LINGALA, /* Lingala */ 
    HZ_LANGUAGE_LISU, /* Lisu */ 
    HZ_LANGUAGE_LAMPUNG, /* Lampung */ 
    HZ_LANGUAGE_LAKI, /* Laki */ 
    HZ_LANGUAGE_LOW_MARI, /* Low Mari */ 
    HZ_LANGUAGE_LIMBU, /* Limbu */ 
    HZ_LANGUAGE_LOMBARD, /* Lombard */ 
    HZ_LANGUAGE_LOMWE, /* Lomwe */ 
    HZ_LANGUAGE_LOMA, /* Loma */ 
    HZ_LANGUAGE_LURI, /* Luri */ 
    HZ_LANGUAGE_LOWER_SORBIAN, /* Lower Sorbian */ 
    HZ_LANGUAGE_LULE_SAMI, /* Lule Sami */ 
    HZ_LANGUAGE_LITHUANIAN, /* Lithuanian */ 
    HZ_LANGUAGE_LUXEMBOURGISH, /* Luxembourgish */ 
    HZ_LANGUAGE_LUBA_LULUA, /* Luba-Lulua */ 
    HZ_LANGUAGE_LUBA_KATANGA, /* Luba-Katanga */ 
    HZ_LANGUAGE_GANDA, /* Ganda */ 
    HZ_LANGUAGE_LUYIA, /* Luyia */ 
    HZ_LANGUAGE_LUO, /* Luo */ 
    HZ_LANGUAGE_LATVIAN, /* Latvian */ 
    HZ_LANGUAGE_MADURA, /* Madura */ 
    HZ_LANGUAGE_MAGAHI, /* Magahi */ 
    HZ_LANGUAGE_MARSHALLESE, /* Marshallese */ 
    HZ_LANGUAGE_MAJANG, /* Majang */ 
    HZ_LANGUAGE_MAKHUWA, /* Makhuwa */ 
    HZ_LANGUAGE_MALAYALAM, /* Malayalam */ 
    HZ_LANGUAGE_MAM, /* Mam */ 
    HZ_LANGUAGE_MANSI, /* Mansi */ 
    HZ_LANGUAGE_MAPUDUNGUN, /* Mapudungun */ 
    HZ_LANGUAGE_MARATHI, /* Marathi */ 
    HZ_LANGUAGE_MARWARI, /* Marwari */ 
    HZ_LANGUAGE_MBUNDU, /* Mbundu */ 
    HZ_LANGUAGE_MBO, /* Mbo */ 
    HZ_LANGUAGE_MANCHU, /* Manchu */ 
    HZ_LANGUAGE_MOOSE_CREE, /* Moose Cree */ 
    HZ_LANGUAGE_MENDE, /* Mende */ 
    HZ_LANGUAGE_MANDAR, /* Mandar */ 
    HZ_LANGUAGE_MEEN, /* Meen */ 
    HZ_LANGUAGE_MERU, /* Meru */ 
    HZ_LANGUAGE_PATTANI_MALAY, /* Pattani Malay */ 
    HZ_LANGUAGE_MORISYEN, /* Morisyen */ 
    HZ_LANGUAGE_MINANGKABAU, /* Minangkabau */ 
    HZ_LANGUAGE_MIZO, /* Mizo */ 
    HZ_LANGUAGE_MACEDONIAN, /* Macedonian */ 
    HZ_LANGUAGE_MAKASAR, /* Makasar */ 
    HZ_LANGUAGE_KITUBA, /* Kituba */ 
    HZ_LANGUAGE_MALE, /* Male */ 
    HZ_LANGUAGE_MALAGASY, /* Malagasy */ 
    HZ_LANGUAGE_MALINKE, /* Malinke */ 
    HZ_LANGUAGE_MALAYALAM_REFORMED, /* Malayalam Reformed */ 
    HZ_LANGUAGE_MALAY, /* Malay */ 
    HZ_LANGUAGE_MANDINKA, /* Mandinka */ 
    HZ_LANGUAGE_MONGOLIAN, /* Mongolian */ 
    HZ_LANGUAGE_MANIPURI, /* Manipuri */ 
    HZ_LANGUAGE_MANINKA, /* Maninka */ 
    HZ_LANGUAGE_MANX, /* Manx */ 
    HZ_LANGUAGE_MOHAWK, /* Mohawk */ 
    HZ_LANGUAGE_MOKSHA, /* Moksha */ 
    HZ_LANGUAGE_MOLDAVIAN, /* Moldavian */ 
    HZ_LANGUAGE_MON, /* Mon */ 
    HZ_LANGUAGE_MOROCCAN, /* Moroccan */ 
    HZ_LANGUAGE_MOSSI, /* Mossi */ 
    HZ_LANGUAGE_MAORI, /* Maori */ 
    HZ_LANGUAGE_MAITHILI, /* Maithili */ 
    HZ_LANGUAGE_MALTESE, /* Maltese */ 
    HZ_LANGUAGE_MUNDARI, /* Mundari */ 
    HZ_LANGUAGE_MUSCOGEE, /* Muscogee */ 
    HZ_LANGUAGE_MIRANDESE, /* Mirandese */ 
    HZ_LANGUAGE_HMONG_DAW, /* Hmong Daw */ 
    HZ_LANGUAGE_MAYAN, /* Mayan */ 
    HZ_LANGUAGE_MAZANDERANI, /* Mazanderani */ 
    HZ_LANGUAGE_NAGA_ASSAMESE, /* Naga-Assamese */ 
    HZ_LANGUAGE_NAHUATL, /* Nahuatl */ 
    HZ_LANGUAGE_NANAI, /* Nanai */ 
    HZ_LANGUAGE_NEAPOLITAN, /* Neapolitan */ 
    HZ_LANGUAGE_NASKAPI, /* Naskapi */ 
    HZ_LANGUAGE_NAURUAN, /* Nauruan */ 
    HZ_LANGUAGE_NAVAJO, /* Navajo */ 
    HZ_LANGUAGE_N_CREE, /* N-Cree */ 
    HZ_LANGUAGE_NDEBELE, /* Ndebele */ 
    HZ_LANGUAGE_NDAU, /* Ndau */ 
    HZ_LANGUAGE_NDONGA, /* Ndonga */ 
    HZ_LANGUAGE_LOW_SAXON, /* Low Saxon */ 
    HZ_LANGUAGE_NEPALI, /* Nepali */ 
    HZ_LANGUAGE_NEWARI, /* Newari */ 
    HZ_LANGUAGE_NGBAKA, /* Ngbaka */ 
    HZ_LANGUAGE_NAGARI, /* Nagari */ 
    HZ_LANGUAGE_NORWAY_HOUSE_CREE, /* Norway House Cree */ 
    HZ_LANGUAGE_NISI, /* Nisi */ 
    HZ_LANGUAGE_NIUEAN, /* Niuean */ 
    HZ_LANGUAGE_NYANKOLE, /* Nyankole */ 
    HZ_LANGUAGE_NKO, /* NKo */ 
    HZ_LANGUAGE_DUTCH, /* Dutch */ 
    HZ_LANGUAGE_NIMADI, /* Nimadi */ 
    HZ_LANGUAGE_NOGAI, /* Nogai */ 
    HZ_LANGUAGE_NORWEGIAN, /* Norwegian */ 
    HZ_LANGUAGE_NOVIAL, /* Novial */ 
    HZ_LANGUAGE_NORTHERN_SAMI, /* Northern Sami */ 
    HZ_LANGUAGE_NORTHERN_SOTHO, /* Northern Sotho */ 
    HZ_LANGUAGE_NORTHERN_TAI, /* Northern Tai */ 
    HZ_LANGUAGE_ESPERANTO, /* Esperanto */ 
    HZ_LANGUAGE_NYAMWEZI, /* Nyamwezi */ 
    HZ_LANGUAGE_NORWEGIAN_NYNORSK_NYNORSK_NORWEGIAN, /* Norwegian Nynorsk (Nynorsk, Norwegian) */ 
    HZ_LANGUAGE_MBEMBE_TIGON, /* Mbembe Tigon */ 
    HZ_LANGUAGE_OCCITAN, /* Occitan */ 
    HZ_LANGUAGE_OJI_CREE, /* Oji-Cree */ 
    HZ_LANGUAGE_OJIBWAY, /* Ojibway */ 
    HZ_LANGUAGE_ODIA_FORMERLY_ORIYA, /* Odia (formerly Oriya) */ 
    HZ_LANGUAGE_OROMO, /* Oromo */ 
    HZ_LANGUAGE_OSSETIAN, /* Ossetian */ 
    HZ_LANGUAGE_PALESTINIAN_ARAMAIC, /* Palestinian Aramaic */ 
    HZ_LANGUAGE_PANGASINAN, /* Pangasinan */ 
    HZ_LANGUAGE_PALI, /* Pali */ 
    HZ_LANGUAGE_PAMPANGAN, /* Pampangan */ 
    HZ_LANGUAGE_PUNJABI, /* Punjabi */ 
    HZ_LANGUAGE_PALPA, /* Palpa */ 
    HZ_LANGUAGE_PAPIAMENTU, /* Papiamentu */ 
    HZ_LANGUAGE_PASHTO, /* Pashto */ 
    HZ_LANGUAGE_PALAUAN, /* Palauan */ 
    HZ_LANGUAGE_BOUYEI, /* Bouyei */ 
    HZ_LANGUAGE_PICARD, /* Picard */ 
    HZ_LANGUAGE_PENNSYLVANIA_GERMAN, /* Pennsylvania German */ 
    HZ_LANGUAGE_POLYTONIC_GREEK, /* Polytonic Greek */ 
    HZ_LANGUAGE_PHAKE, /* Phake */ 
    HZ_LANGUAGE_NORFOLK, /* Norfolk */ 
    HZ_LANGUAGE_FILIPINO, /* Filipino */ 
    HZ_LANGUAGE_PALAUNG, /* Palaung */ 
    HZ_LANGUAGE_POLISH, /* Polish */ 
    HZ_LANGUAGE_PIEMONTESE, /* Piemontese */ 
    HZ_LANGUAGE_WESTERN_PANJABI, /* Western Panjabi */ 
    HZ_LANGUAGE_POCOMCHI, /* Pocomchi */ 
    HZ_LANGUAGE_POHNPEIAN, /* Pohnpeian */ 
    HZ_LANGUAGE_PROVENCAL_OLD_PROVENCAL, /* Provencal / Old Provencal */ 
    HZ_LANGUAGE_PORTUGUESE, /* Portuguese */ 
    HZ_LANGUAGE_WESTERN_PWO_KAREN, /* Western Pwo Karen */ 
    HZ_LANGUAGE_CHIN, /* Chin */ 
    HZ_LANGUAGE_KICHE, /* Kiche */ 
    HZ_LANGUAGE_QUECHUA_BOLIVIA, /* Quechua (Bolivia) */ 
    HZ_LANGUAGE_QUECHUA, /* Quechua */ 
    HZ_LANGUAGE_QUECHUA_ECUADOR, /* Quechua (Ecuador) */ 
    HZ_LANGUAGE_QUECHUA_PERU, /* Quechua (Peru) */ 
    HZ_LANGUAGE_RAJASTHANI, /* Rajasthani */ 
    HZ_LANGUAGE_RAROTONGAN, /* Rarotongan */ 
    HZ_LANGUAGE_RUSSIAN_BURIAT, /* Russian Buriat */ 
    HZ_LANGUAGE_R_CREE, /* R-Cree */ 
    HZ_LANGUAGE_REJANG, /* Rejang */ 
    HZ_LANGUAGE_RIANG, /* Riang */ 
    HZ_LANGUAGE_TARIFIT, /* Tarifit */ 
    HZ_LANGUAGE_RITARUNGO, /* Ritarungo */ 
    HZ_LANGUAGE_ARAKWAL, /* Arakwal */ 
    HZ_LANGUAGE_ROMANSH, /* Romansh */ 
    HZ_LANGUAGE_VLAX_ROMANI, /* Vlax Romani */ 
    HZ_LANGUAGE_ROMANIAN, /* Romanian */ 
    HZ_LANGUAGE_ROMANY, /* Romany */ 
    HZ_LANGUAGE_RUSYN, /* Rusyn */ 
    HZ_LANGUAGE_ROTUMAN, /* Rotuman */ 
    HZ_LANGUAGE_KINYARWANDA, /* Kinyarwanda */ 
    HZ_LANGUAGE_RUNDI, /* Rundi */ 
    HZ_LANGUAGE_AROMANIAN, /* Aromanian */ 
    HZ_LANGUAGE_RUSSIAN, /* Russian */ 
    HZ_LANGUAGE_SADRI, /* Sadri */ 
    HZ_LANGUAGE_SANSKRIT, /* Sanskrit */ 
    HZ_LANGUAGE_SASAK, /* Sasak */ 
    HZ_LANGUAGE_SANTALI, /* Santali */ 
    HZ_LANGUAGE_SAYISI, /* Sayisi */ 
    HZ_LANGUAGE_SICILIAN, /* Sicilian */ 
    HZ_LANGUAGE_SCOTS, /* Scots */ 
    HZ_LANGUAGE_NORTH_SLAVEY, /* North Slavey */ 
    HZ_LANGUAGE_SEKOTA, /* Sekota */ 
    HZ_LANGUAGE_SELKUP, /* Selkup */ 
    HZ_LANGUAGE_OLD_IRISH, /* Old Irish */ 
    HZ_LANGUAGE_SANGO, /* Sango */ 
    HZ_LANGUAGE_SAMOGITIAN, /* Samogitian */ 
    HZ_LANGUAGE_TACHELHIT, /* Tachelhit */ 
    HZ_LANGUAGE_SHAN, /* Shan */ 
    HZ_LANGUAGE_SIBE, /* Sibe */ 
    HZ_LANGUAGE_SIDAMO, /* Sidamo */ 
    HZ_LANGUAGE_SILTE_GURAGE, /* Silte Gurage */ 
    HZ_LANGUAGE_SKOLT_SAMI, /* Skolt Sami */ 
    HZ_LANGUAGE_SLOVAK, /* Slovak */ 
    HZ_LANGUAGE_SLAVEY, /* Slavey */ 
    HZ_LANGUAGE_SLOVENIAN, /* Slovenian */ 
    HZ_LANGUAGE_SOMALI, /* Somali */ 
    HZ_LANGUAGE_SAMOAN, /* Samoan */ 
    HZ_LANGUAGE_SENA, /* Sena */ 
    HZ_LANGUAGE_SHONA, /* Shona */ 
    HZ_LANGUAGE_SINDHI, /* Sindhi */ 
    HZ_LANGUAGE_SINHALA_SINHALESE, /* Sinhala (Sinhalese) */ 
    HZ_LANGUAGE_SONINKE, /* Soninke */ 
    HZ_LANGUAGE_SODO_GURAGE, /* Sodo Gurage */ 
    HZ_LANGUAGE_SONGE, /* Songe */ 
    HZ_LANGUAGE_SOUTHERN_SOTHO, /* Southern Sotho */ 
    HZ_LANGUAGE_ALBANIAN, /* Albanian */ 
    HZ_LANGUAGE_SERBIAN, /* Serbian */ 
    HZ_LANGUAGE_SARDINIAN, /* Sardinian */ 
    HZ_LANGUAGE_SARAIKI, /* Saraiki */ 
    HZ_LANGUAGE_SERER, /* Serer */ 
    HZ_LANGUAGE_SOUTH_SLAVEY, /* South Slavey */ 
    HZ_LANGUAGE_SOUTHERN_SAMI, /* Southern Sami */ 
    HZ_LANGUAGE_SATERLAND_FRISIAN, /* Saterland Frisian */ 
    HZ_LANGUAGE_SUKUMA, /* Sukuma */ 
    HZ_LANGUAGE_SUNDANESE, /* Sundanese */ 
    HZ_LANGUAGE_SURI, /* Suri */ 
    HZ_LANGUAGE_SVAN, /* Svan */ 
    HZ_LANGUAGE_SWEDISH, /* Swedish */ 
    HZ_LANGUAGE_SWADAYA_ARAMAIC, /* Swadaya Aramaic */ 
    HZ_LANGUAGE_SWAHILI, /* Swahili */ 
    HZ_LANGUAGE_SWATI, /* Swati */ 
    HZ_LANGUAGE_SUTU, /* Sutu */ 
    HZ_LANGUAGE_UPPER_SAXON, /* Upper Saxon */ 
    HZ_LANGUAGE_SYLHETI, /* Sylheti */ 
    HZ_LANGUAGE_SYRIAC, /* Syriac */ 
    HZ_LANGUAGE_SYRIAC_ESTRANGELA_SCRIPT_VARIANT_EQUIVALENT_TO_ISO_15924_SYRE, /* Syriac, Estrangela script-variant (equivalent to ISO 15924 'Syre') */ 
    HZ_LANGUAGE_SYRIAC_WESTERN_SCRIPT_VARIANT_EQUIVALENT_TO_ISO_15924_SYRJ, /* Syriac, Western script-variant (equivalent to ISO 15924 'Syrj') */ 
    HZ_LANGUAGE_SYRIAC_EASTERN_SCRIPT_VARIANT_EQUIVALENT_TO_ISO_15924_SYRN, /* Syriac, Eastern script-variant (equivalent to ISO 15924 'Syrn') */ 
    HZ_LANGUAGE_SILESIAN, /* Silesian */ 
    HZ_LANGUAGE_TABASARAN, /* Tabasaran */ 
    HZ_LANGUAGE_TAJIKI, /* Tajiki */ 
    HZ_LANGUAGE_TAMIL, /* Tamil */ 
    HZ_LANGUAGE_TATAR, /* Tatar */ 
    HZ_LANGUAGE_TH_CREE, /* TH-Cree */ 
    HZ_LANGUAGE_DEHONG_DAI, /* Dehong Dai */ 
    HZ_LANGUAGE_TELUGU, /* Telugu */ 
    HZ_LANGUAGE_TETUM, /* Tetum */ 
    HZ_LANGUAGE_TAGALOG, /* Tagalog */ 
    HZ_LANGUAGE_TONGAN, /* Tongan */ 
    HZ_LANGUAGE_TIGRE, /* Tigre */ 
    HZ_LANGUAGE_TIGRINYA, /* Tigrinya */ 
    HZ_LANGUAGE_THAI, /* Thai */ 
    HZ_LANGUAGE_TAHITIAN, /* Tahitian */ 
    HZ_LANGUAGE_TIBETAN, /* Tibetan */ 
    HZ_LANGUAGE_TIV, /* Tiv */ 
    HZ_LANGUAGE_TURKMEN, /* Turkmen */ 
    HZ_LANGUAGE_TAMASHEK, /* Tamashek */ 
    HZ_LANGUAGE_TEMNE, /* Temne */ 
    HZ_LANGUAGE_TSWANA, /* Tswana */ 
    HZ_LANGUAGE_TUNDRA_ENETS, /* Tundra Enets */ 
    HZ_LANGUAGE_TONGA, /* Tonga */ 
    HZ_LANGUAGE_TODO, /* Todo */ 
    HZ_LANGUAGE_TOMA, /* Toma */ 
    HZ_LANGUAGE_TOK_PISIN, /* Tok Pisin */ 
    HZ_LANGUAGE_TURKISH, /* Turkish */ 
    HZ_LANGUAGE_TSONGA, /* Tsonga */ 
    HZ_LANGUAGE_TSHANGLA, /* Tshangla */ 
    HZ_LANGUAGE_TUROYO_ARAMAIC, /* Turoyo Aramaic */ 
    HZ_LANGUAGE_TUMBUKA, /* Tumbuka */ 
    HZ_LANGUAGE_TULU, /* Tulu */ 
    HZ_LANGUAGE_TUVIN, /* Tuvin */ 
    HZ_LANGUAGE_TUVALU, /* Tuvalu */ 
    HZ_LANGUAGE_TWI, /* Twi */ 
    HZ_LANGUAGE_TAY, /* Tay */ 
    HZ_LANGUAGE_TAMAZIGHT, /* Tamazight */ 
    HZ_LANGUAGE_TZOTZIL, /* Tzotzil */ 
    HZ_LANGUAGE_UDMURT, /* Udmurt */ 
    HZ_LANGUAGE_UKRAINIAN, /* Ukrainian */ 
    HZ_LANGUAGE_UMBUNDU, /* Umbundu */ 
    HZ_LANGUAGE_URDU, /* Urdu */ 
    HZ_LANGUAGE_UPPER_SORBIAN, /* Upper Sorbian */ 
    HZ_LANGUAGE_UYGHUR, /* Uyghur */ 
    HZ_LANGUAGE_UZBEK, /* Uzbek */ 
    HZ_LANGUAGE_VENETIAN, /* Venetian */ 
    HZ_LANGUAGE_VENDA, /* Venda */ 
    HZ_LANGUAGE_VIETNAMESE, /* Vietnamese */ 
    HZ_LANGUAGE_VOLAPUK, /* Volapuk */ 
    HZ_LANGUAGE_VORO, /* Voro */ 
    HZ_LANGUAGE_WA, /* Wa */ 
    HZ_LANGUAGE_WAGDI, /* Wagdi */ 
    HZ_LANGUAGE_WARAY_WARAY, /* Waray-Waray */ 
    HZ_LANGUAGE_WEST_CREE, /* West-Cree */ 
    HZ_LANGUAGE_WELSH, /* Welsh */ 
    HZ_LANGUAGE_WOLOF, /* Wolof */ 
    HZ_LANGUAGE_WALLOON, /* Walloon */ 
    HZ_LANGUAGE_MEWATI, /* Mewati */ 
    HZ_LANGUAGE_LU, /* Lu */ 
    HZ_LANGUAGE_XHOSA, /* Xhosa */ 
    HZ_LANGUAGE_MINJANGBAL, /* Minjangbal */ 
    HZ_LANGUAGE_KHENGKHA, /* Khengkha */ 
    HZ_LANGUAGE_SOGA, /* Soga */ 
    HZ_LANGUAGE_KPELLE_LIBERIA, /* Kpelle (Liberia) */ 
    HZ_LANGUAGE_SAKHA, /* Sakha */ 
    HZ_LANGUAGE_YAO, /* Yao */ 
    HZ_LANGUAGE_YAPESE, /* Yapese */ 
    HZ_LANGUAGE_YORUBA, /* Yoruba */ 
    HZ_LANGUAGE_Y_CREE, /* Y-Cree */ 
    HZ_LANGUAGE_YI_CLASSIC, /* Yi Classic */ 
    HZ_LANGUAGE_YI_MODERN, /* Yi Modern */ 
    HZ_LANGUAGE_ZEALANDIC, /* Zealandic */ 
    HZ_LANGUAGE_STANDARD_MOROCCAN_TAMAZIGHT, /* Standard Moroccan Tamazight */ 
    HZ_LANGUAGE_ZHUANG, /* Zhuang */ 
    HZ_LANGUAGE_CHINESE_TRADITIONAL_HONG_KONG_SAR, /* Chinese, Traditional, Hong Kong SAR */ 
    HZ_LANGUAGE_CHINESE_PHONETIC, /* Chinese, Phonetic */ 
    HZ_LANGUAGE_CHINESE_SIMPLIFIED, /* Chinese, Simplified */ 
    HZ_LANGUAGE_CHINESE_TRADITIONAL, /* Chinese, Traditional */ 
    HZ_LANGUAGE_CHINESE_TRADITIONAL_MACAO_SAR, /* Chinese, Traditional, Macao SAR */ 
    HZ_LANGUAGE_ZANDE, /* Zande */ 
    HZ_LANGUAGE_ZULU, /* Zulu */ 
    HZ_LANGUAGE_ZAZAKI, /* Zazaki */ 
} hz_language_t;

typedef struct hz_language_map_t {
    hz_language_t language;
    const char *language_name;
    hz_tag_t tag;
    const char *codes;
} hz_language_map_t;

static const hz_language_map_t language_map_list[] = {
    {HZ_LANGUAGE_ABAZA, "Abaza", HZ_TAG('A','B','A',' '), "abq"}, /* Abaza */
    {HZ_LANGUAGE_ABKHAZIAN, "Abkhazian", HZ_TAG('A','B','K',' '), "abk"}, /* Abkhazian */
    {HZ_LANGUAGE_ACHOLI, "Acholi", HZ_TAG('A','C','H',' '), "ach"}, /* Acholi */
    {HZ_LANGUAGE_ACHI, "Achi", HZ_TAG('A','C','R',' '), "acr"}, /* Achi */
    {HZ_LANGUAGE_ADYGHE, "Adyghe", HZ_TAG('A','D','Y',' '), "ady"}, /* Adyghe */
    {HZ_LANGUAGE_AFRIKAANS, "Afrikaans", HZ_TAG('A','F','K',' '), "afr"}, /* Afrikaans */
    {HZ_LANGUAGE_AFAR, "Afar", HZ_TAG('A','F','R',' '), "aar"}, /* Afar */
    {HZ_LANGUAGE_AGAW, "Agaw", HZ_TAG('A','G','W',' '), "ahg"}, /* Agaw */
    {HZ_LANGUAGE_AITON, "Aiton", HZ_TAG('A','I','O',' '), "aio"}, /* Aiton */
    {HZ_LANGUAGE_AKAN, "Akan", HZ_TAG('A','K','A',' '), "aka:fat:twi"}, /* Akan */
    {HZ_LANGUAGE_BATAK_ANGKOLA, "Batak Angkola", HZ_TAG('A','K','B',' '), "akb"}, /* Batak Angkola */
    {HZ_LANGUAGE_ALSATIAN, "Alsatian", HZ_TAG('A','L','S',' '), "gsw"}, /* Alsatian */
    {HZ_LANGUAGE_ALTAI, "Altai", HZ_TAG('A','L','T',' '), "atv:alt"}, /* Altai */
    {HZ_LANGUAGE_AMHARIC, "Amharic", HZ_TAG('A','M','H',' '), "amh"}, /* Amharic */
    {HZ_LANGUAGE_ANGLO_SAXON, "Anglo-Saxon", HZ_TAG('A','N','G',' '), "ang"}, /* Anglo-Saxon */
    {HZ_LANGUAGE_PHONETIC_TRANSCRIPTION_AMERICANIST_CONVENTIONS, "Phonetic transcription-Americanist conventions", HZ_TAG('A','P','P','H'), NULL}, /* Phonetic transcription-Americanist conventions */
    {HZ_LANGUAGE_ARABIC, "Arabic", HZ_TAG('A','R','A',' '), "ara"}, /* Arabic */
    {HZ_LANGUAGE_ARAGONESE, "Aragonese", HZ_TAG('A','R','G',' '), "arg"}, /* Aragonese */
    {HZ_LANGUAGE_AARI, "Aari", HZ_TAG('A','R','I',' '), "aiw"}, /* Aari */
    {HZ_LANGUAGE_RAKHINE, "Rakhine", HZ_TAG('A','R','K',' '), "mhv:rmz:rki"}, /* Rakhine */
    {HZ_LANGUAGE_ASSAMESE, "Assamese", HZ_TAG('A','S','M',' '), "asm"}, /* Assamese */
    {HZ_LANGUAGE_ASTURIAN, "Asturian", HZ_TAG('A','S','T',' '), "ast"}, /* Asturian */
    {HZ_LANGUAGE_ATHAPASKAN_LANGUAGES, "Athapaskan languages", HZ_TAG('A','T','H',' '), "aht:apa:apk:apj:apl:apm:apw:ath:bea:sek:bcr:caf:chp:clc:coq:crx:ctc:den:dgr:gce:gwi:haa:hoi:hup:ing:kkz:koy:ktw:kuu:mvb:nav:qwt:scs:srs:taa:tau:tcb:tce:tfn:tgx:tht:tol:ttm:tuu:txc:wlk:xup:xsl"}, /* Athapaskan languages */
    {HZ_LANGUAGE_AVAR, "Avar", HZ_TAG('A','V','R',' '), "ava"}, /* Avar */
    {HZ_LANGUAGE_AWADHI, "Awadhi", HZ_TAG('A','W','A',' '), "awa"}, /* Awadhi */
    {HZ_LANGUAGE_AYMARA, "Aymara", HZ_TAG('A','Y','M',' '), "aym"}, /* Aymara */
    {HZ_LANGUAGE_TORKI, "Torki", HZ_TAG('A','Z','B',' '), "azb"}, /* Torki */
    {HZ_LANGUAGE_AZERBAIJANI, "Azerbaijani", HZ_TAG('A','Z','E',' '), "aze"}, /* Azerbaijani */
    {HZ_LANGUAGE_BADAGA, "Badaga", HZ_TAG('B','A','D',' '), "bfq"}, /* Badaga */
    {HZ_LANGUAGE_BANDA, "Banda", HZ_TAG('B','A','D','0'), "bad:bbp:bfl:bjo:bpd:bqk:gox:kuw:liy:lna:lnl:mnh:nue:nuu:tor:yaj:zmz"}, /* Banda */
    {HZ_LANGUAGE_BAGHELKHANDI, "Baghelkhandi", HZ_TAG('B','A','G',' '), "bfy"}, /* Baghelkhandi */
    {HZ_LANGUAGE_BALKAR, "Balkar", HZ_TAG('B','A','L',' '), "krc"}, /* Balkar */
    {HZ_LANGUAGE_BALINESE, "Balinese", HZ_TAG('B','A','N',' '), "ban"}, /* Balinese */
    {HZ_LANGUAGE_BAVARIAN, "Bavarian", HZ_TAG('B','A','R',' '), "bar"}, /* Bavarian */
    {HZ_LANGUAGE_BAULE, "Baule", HZ_TAG('B','A','U',' '), "bci"}, /* Baule */
    {HZ_LANGUAGE_BATAK_TOBA, "Batak Toba", HZ_TAG('B','B','C',' '), "bbc"}, /* Batak Toba */
    {HZ_LANGUAGE_BERBER, "Berber", HZ_TAG('B','B','R',' '), "auj:ber:cnu:gha:gho:grr:jbe:jbn:kab:mzb:oua:rif:sds:shi:shy:siz:sjs:swn:taq:tez:thv:thz:tia:tjo:tmh:ttq:tzm:zen:zgh"}, /* Berber */
    {HZ_LANGUAGE_BENCH, "Bench", HZ_TAG('B','C','H',' '), "bcq"}, /* Bench */
    {HZ_LANGUAGE_BIBLE_CREE, "Bible Cree", HZ_TAG('B','C','R',' '), NULL}, /* Bible Cree */
    {HZ_LANGUAGE_BANDJALANG, "Bandjalang", HZ_TAG('B','D','Y',' '), "bdy"}, /* Bandjalang */
    {HZ_LANGUAGE_BELARUSSIAN, "Belarussian", HZ_TAG('B','E','L',' '), "bel"}, /* Belarussian */
    {HZ_LANGUAGE_BEMBA, "Bemba", HZ_TAG('B','E','M',' '), "bem"}, /* Bemba */
    {HZ_LANGUAGE_BENGALI, "Bengali", HZ_TAG('B','E','N',' '), "ben"}, /* Bengali */
    {HZ_LANGUAGE_HARYANVI, "Haryanvi", HZ_TAG('B','G','C',' '), "bgc"}, /* Haryanvi */
    {HZ_LANGUAGE_BAGRI, "Bagri", HZ_TAG('B','G','Q',' '), "bgq"}, /* Bagri */
    {HZ_LANGUAGE_BULGARIAN, "Bulgarian", HZ_TAG('B','G','R',' '), "bul"}, /* Bulgarian */
    {HZ_LANGUAGE_BHILI, "Bhili", HZ_TAG('B','H','I',' '), "bhi:bhb"}, /* Bhili */
    {HZ_LANGUAGE_BHOJPURI, "Bhojpuri", HZ_TAG('B','H','O',' '), "bho"}, /* Bhojpuri */
    {HZ_LANGUAGE_BIKOL, "Bikol", HZ_TAG('B','I','K',' '), "bik:bhk:bcl:bto:cts:bln:fbl:lbl:rbl:ubl"}, /* Bikol */
    {HZ_LANGUAGE_BILEN, "Bilen", HZ_TAG('B','I','L',' '), "byn"}, /* Bilen */
    {HZ_LANGUAGE_BISLAMA, "Bislama", HZ_TAG('B','I','S',' '), "bis"}, /* Bislama */
    {HZ_LANGUAGE_KANAUJI, "Kanauji", HZ_TAG('B','J','J',' '), "bjj"}, /* Kanauji */
    {HZ_LANGUAGE_BLACKFOOT, "Blackfoot", HZ_TAG('B','K','F',' '), "bla"}, /* Blackfoot */
    {HZ_LANGUAGE_BALUCHI, "Baluchi", HZ_TAG('B','L','I',' '), "bal"}, /* Baluchi */
    {HZ_LANGUAGE_PAO_KAREN, "Pao Karen", HZ_TAG('B','L','K',' '), "blk"}, /* Pao Karen */
    {HZ_LANGUAGE_BALANTE, "Balante", HZ_TAG('B','L','N',' '), "bjt:ble"}, /* Balante */
    {HZ_LANGUAGE_BALTI, "Balti", HZ_TAG('B','L','T',' '), "bft"}, /* Balti */
    {HZ_LANGUAGE_BAMBARA_BAMANANKAN, "Bambara (Bamanankan)", HZ_TAG('B','M','B',' '), "bam"}, /* Bambara (Bamanankan) */
    {HZ_LANGUAGE_BAMILEKE, "Bamileke", HZ_TAG('B','M','L',' '), "bai:bbj:bko:byv:fmp:jgo:nla:nnh:nnz:nwe:xmg:ybb"}, /* Bamileke */
    {HZ_LANGUAGE_BOSNIAN, "Bosnian", HZ_TAG('B','O','S',' '), "bos"}, /* Bosnian */
    {HZ_LANGUAGE_BISHNUPRIYA_MANIPURI, "Bishnupriya Manipuri", HZ_TAG('B','P','Y',' '), "bpy"}, /* Bishnupriya Manipuri */
    {HZ_LANGUAGE_BRETON, "Breton", HZ_TAG('B','R','E',' '), "bre"}, /* Breton */
    {HZ_LANGUAGE_BRAHUI, "Brahui", HZ_TAG('B','R','H',' '), "brh"}, /* Brahui */
    {HZ_LANGUAGE_BRAJ_BHASHA, "Braj Bhasha", HZ_TAG('B','R','I',' '), "bra"}, /* Braj Bhasha */
    {HZ_LANGUAGE_BURMESE, "Burmese", HZ_TAG('B','R','M',' '), "mya"}, /* Burmese */
    {HZ_LANGUAGE_BODO, "Bodo", HZ_TAG('B','R','X',' '), "brx"}, /* Bodo */
    {HZ_LANGUAGE_BASHKIR, "Bashkir", HZ_TAG('B','S','H',' '), "bak"}, /* Bashkir */
    {HZ_LANGUAGE_BURUSHASKI, "Burushaski", HZ_TAG('B','S','K',' '), "bsk"}, /* Burushaski */
    {HZ_LANGUAGE_BATAK_DAIRI_PAKPAK, "Batak Dairi (Pakpak)", HZ_TAG('B','T','D',' '), "btd"}, /* Batak Dairi (Pakpak) */
    {HZ_LANGUAGE_BETI, "Beti", HZ_TAG('B','T','I',' '), "btb:beb:bum:bxp:eto:ewo:mct"}, /* Beti */
    {HZ_LANGUAGE_BATAK_LANGUAGES, "Batak languages", HZ_TAG('B','T','K',' '), "akb:bbc:btd:btk:btm:bts:btx:btz"}, /* Batak languages */
    {HZ_LANGUAGE_BATAK_MANDAILING, "Batak Mandailing", HZ_TAG('B','T','M',' '), "btm"}, /* Batak Mandailing */
    {HZ_LANGUAGE_BATAK_SIMALUNGUN, "Batak Simalungun", HZ_TAG('B','T','S',' '), "bts"}, /* Batak Simalungun */
    {HZ_LANGUAGE_BATAK_KARO, "Batak Karo", HZ_TAG('B','T','X',' '), "btx"}, /* Batak Karo */
    {HZ_LANGUAGE_BATAK_ALAS_KLUET, "Batak Alas-Kluet", HZ_TAG('B','T','Z',' '), "btz"}, /* Batak Alas-Kluet */
    {HZ_LANGUAGE_BUGIS, "Bugis", HZ_TAG('B','U','G',' '), "bug"}, /* Bugis */
    {HZ_LANGUAGE_MEDUMBA, "Medumba", HZ_TAG('B','Y','V',' '), "byv"}, /* Medumba */
    {HZ_LANGUAGE_KAQCHIKEL, "Kaqchikel", HZ_TAG('C','A','K',' '), "cak"}, /* Kaqchikel */
    {HZ_LANGUAGE_CATALAN, "Catalan", HZ_TAG('C','A','T',' '), "cat"}, /* Catalan */
    {HZ_LANGUAGE_ZAMBOANGA_CHAVACANO, "Zamboanga Chavacano", HZ_TAG('C','B','K',' '), "cbk"}, /* Zamboanga Chavacano */
    {HZ_LANGUAGE_CHINANTEC, "Chinantec", HZ_TAG('C','C','H','N'), "cco:chj:chq:chz:cle:cnl:cnt:cpa:csa:cso:cte:ctl:cuc:cvn"}, /* Chinantec */
    {HZ_LANGUAGE_CEBUANO, "Cebuano", HZ_TAG('C','E','B',' '), "ceb"}, /* Cebuano */
    {HZ_LANGUAGE_CHIGA, "Chiga", HZ_TAG('C','G','G',' '), "cgg"}, /* Chiga */
    {HZ_LANGUAGE_CHAMORRO, "Chamorro", HZ_TAG('C','H','A',' '), "cha"}, /* Chamorro */
    {HZ_LANGUAGE_CHECHEN, "Chechen", HZ_TAG('C','H','E',' '), "che"}, /* Chechen */
    {HZ_LANGUAGE_CHAHA_GURAGE, "Chaha Gurage", HZ_TAG('C','H','G',' '), "sgw"}, /* Chaha Gurage */
    {HZ_LANGUAGE_CHATTISGARHI, "Chattisgarhi", HZ_TAG('C','H','H',' '), "hne"}, /* Chattisgarhi */
    {HZ_LANGUAGE_CHICHEWA_CHEWA_NYANJA, "Chichewa (Chewa, Nyanja)", HZ_TAG('C','H','I',' '), "nya"}, /* Chichewa (Chewa, Nyanja) */
    {HZ_LANGUAGE_CHUKCHI, "Chukchi", HZ_TAG('C','H','K',' '), "ckt"}, /* Chukchi */
    {HZ_LANGUAGE_CHUUKESE, "Chuukese", HZ_TAG('C','H','K','0'), "chk"}, /* Chuukese */
    {HZ_LANGUAGE_CHOCTAW, "Choctaw", HZ_TAG('C','H','O',' '), "cho"}, /* Choctaw */
    {HZ_LANGUAGE_CHIPEWYAN, "Chipewyan", HZ_TAG('C','H','P',' '), "chp"}, /* Chipewyan */
    {HZ_LANGUAGE_CHEROKEE, "Cherokee", HZ_TAG('C','H','R',' '), "chr"}, /* Cherokee */
    {HZ_LANGUAGE_CHUVASH, "Chuvash", HZ_TAG('C','H','U',' '), "chv"}, /* Chuvash */
    {HZ_LANGUAGE_CHEYENNE, "Cheyenne", HZ_TAG('C','H','Y',' '), "chy"}, /* Cheyenne */
    {HZ_LANGUAGE_WESTERN_CHAM, "Western Cham", HZ_TAG('C','J','A',' '), "cja"}, /* Western Cham */
    {HZ_LANGUAGE_EASTERN_CHAM, "Eastern Cham", HZ_TAG('C','J','M',' '), "cjm"}, /* Eastern Cham */
    {HZ_LANGUAGE_COMORIAN, "Comorian", HZ_TAG('C','M','R',' '), "swb:wlc:wni:zdj"}, /* Comorian */
    {HZ_LANGUAGE_COPTIC, "Coptic", HZ_TAG('C','O','P',' '), "cop"}, /* Coptic */
    {HZ_LANGUAGE_CORNISH, "Cornish", HZ_TAG('C','O','R',' '), "cor"}, /* Cornish */
    {HZ_LANGUAGE_CORSICAN, "Corsican", HZ_TAG('C','O','S',' '), "cos"}, /* Corsican */
    {HZ_LANGUAGE_CREOLES, "Creoles", HZ_TAG('C','P','P',' '), "abs:acf:afs:aig:aoa:bah:bew:bis:bjs:bpl:bpq:brc:bxo:bzj:bzk:cbk:ccl:ccm:chn:cks:cpe:cpf:cpi:cpp:cri:crp:crs:dcr:dep:djk:fab:fng:fpe:gac:gcf:gcl:gcr:gib:goq:gpe:gul:gyn:hat:hca:hmo:hwc:icr:idb:ihb:jam:jvd:kcn:kea:kmv:kri:kww:lir:lou:lrt:max:mbf:mcm:mfe:mfp:mkn:mod:msi:mud:mzs:nag:nef:ngm:njt:onx:oor:pap:pcm:pea:pey:pga:pih:pis:pln:pml:pmy:pov:pre:rcf:rop:scf:sci:skw:srm:srn:sta:svc:tas:tch:tcs:tgh:tmg:tpi:trf:tvy:uln:vic:vkp:wes:xmm"}, /* Creoles */
    {HZ_LANGUAGE_CREE, "Cree", HZ_TAG('C','R','E',' '), "cre"}, /* Cree */
    {HZ_LANGUAGE_CARRIER, "Carrier", HZ_TAG('C','R','R',' '), "crx:caf"}, /* Carrier */
    {HZ_LANGUAGE_CRIMEAN_TATAR, "Crimean Tatar", HZ_TAG('C','R','T',' '), "crh"}, /* Crimean Tatar */
    {HZ_LANGUAGE_KASHUBIAN, "Kashubian", HZ_TAG('C','S','B',' '), "csb"}, /* Kashubian */
    {HZ_LANGUAGE_CHURCH_SLAVONIC, "Church Slavonic", HZ_TAG('C','S','L',' '), "chu"}, /* Church Slavonic */
    {HZ_LANGUAGE_CZECH, "Czech", HZ_TAG('C','S','Y',' '), "ces"}, /* Czech */
    {HZ_LANGUAGE_CHITTAGONIAN, "Chittagonian", HZ_TAG('C','T','G',' '), "ctg"}, /* Chittagonian */
    {HZ_LANGUAGE_SAN_BLAS_KUNA, "San Blas Kuna", HZ_TAG('C','U','K',' '), "cuk"}, /* San Blas Kuna */
    {HZ_LANGUAGE_DAGBANI, "Dagbani", HZ_TAG('D','A','G',' '), "dag"}, /* Dagbani */
    {HZ_LANGUAGE_DANISH, "Danish", HZ_TAG('D','A','N',' '), "dan"}, /* Danish */
    {HZ_LANGUAGE_DARGWA, "Dargwa", HZ_TAG('D','A','R',' '), "dar"}, /* Dargwa */
    {HZ_LANGUAGE_DAYI, "Dayi", HZ_TAG('D','A','X',' '), "dax"}, /* Dayi */
    {HZ_LANGUAGE_WOODS_CREE, "Woods Cree", HZ_TAG('D','C','R',' '), "cwd"}, /* Woods Cree */
    {HZ_LANGUAGE_GERMAN, "German", HZ_TAG('D','E','U',' '), "deu"}, /* German */
    {HZ_LANGUAGE_DOGRI_INDIVIDUAL_LANGUAGE, "Dogri (individual language)", HZ_TAG('D','G','O',' '), "dgo"}, /* Dogri (individual language) */
    {HZ_LANGUAGE_DOGRI_MACROLANGUAGE, "Dogri (macrolanguage)", HZ_TAG('D','G','R',' '), "doi"}, /* Dogri (macrolanguage) */
    {HZ_LANGUAGE_DHANGU, "Dhangu", HZ_TAG('D','H','G',' '), "dhg"}, /* Dhangu */
    {HZ_LANGUAGE_DIVEHI_DHIVEHI_MALDIVIAN, "Divehi (Dhivehi, Maldivian)", HZ_TAG('D','H','V',' '), "div"}, /* Divehi (Dhivehi, Maldivian) */
    {HZ_LANGUAGE_DIMLI, "Dimli", HZ_TAG('D','I','Q',' '), "diq"}, /* Dimli */
    {HZ_LANGUAGE_DIVEHI_DHIVEHI_MALDIVIAN, "Divehi (Dhivehi, Maldivian)", HZ_TAG('D','I','V',' '), "div"}, /* Divehi (Dhivehi, Maldivian) */
    {HZ_LANGUAGE_ZARMA, "Zarma", HZ_TAG('D','J','R',' '), "dje"}, /* Zarma */
    {HZ_LANGUAGE_DJAMBARRPUYNGU, "Djambarrpuyngu", HZ_TAG('D','J','R','0'), "djr"}, /* Djambarrpuyngu */
    {HZ_LANGUAGE_DANGME, "Dangme", HZ_TAG('D','N','G',' '), "ada"}, /* Dangme */
    {HZ_LANGUAGE_DAN, "Dan", HZ_TAG('D','N','J',' '), "dnj"}, /* Dan */
    {HZ_LANGUAGE_DINKA, "Dinka", HZ_TAG('D','N','K',' '), "din"}, /* Dinka */
    {HZ_LANGUAGE_DARI, "Dari", HZ_TAG('D','R','I',' '), "prs"}, /* Dari */
    {HZ_LANGUAGE_DHUWAL, "Dhuwal", HZ_TAG('D','U','J',' '), "duj:dwu:dwy"}, /* Dhuwal */
    {HZ_LANGUAGE_DUNGAN, "Dungan", HZ_TAG('D','U','N',' '), "dng"}, /* Dungan */
    {HZ_LANGUAGE_DZONGKHA, "Dzongkha", HZ_TAG('D','Z','N',' '), "dzo"}, /* Dzongkha */
    {HZ_LANGUAGE_EBIRA, "Ebira", HZ_TAG('E','B','I',' '), "igb"}, /* Ebira */
    {HZ_LANGUAGE_EASTERN_CREE, "Eastern Cree", HZ_TAG('E','C','R',' '), "crj:crl"}, /* Eastern Cree */
    {HZ_LANGUAGE_EDO, "Edo", HZ_TAG('E','D','O',' '), "bin"}, /* Edo */
    {HZ_LANGUAGE_EFIK, "Efik", HZ_TAG('E','F','I',' '), "efi"}, /* Efik */
    {HZ_LANGUAGE_GREEK, "Greek", HZ_TAG('E','L','L',' '), "ell"}, /* Greek */
    {HZ_LANGUAGE_EASTERN_MANINKAKAN, "Eastern Maninkakan", HZ_TAG('E','M','K',' '), "emk"}, /* Eastern Maninkakan */
    {HZ_LANGUAGE_ENGLISH, "English", HZ_TAG('E','N','G',' '), "eng"}, /* English */
    {HZ_LANGUAGE_ERZYA, "Erzya", HZ_TAG('E','R','Z',' '), "myv"}, /* Erzya */
    {HZ_LANGUAGE_SPANISH, "Spanish", HZ_TAG('E','S','P',' '), "spa"}, /* Spanish */
    {HZ_LANGUAGE_CENTRAL_YUPIK, "Central Yupik", HZ_TAG('E','S','U',' '), "esu"}, /* Central Yupik */
    {HZ_LANGUAGE_ESTONIAN, "Estonian", HZ_TAG('E','T','I',' '), "est"}, /* Estonian */
    {HZ_LANGUAGE_BASQUE, "Basque", HZ_TAG('E','U','Q',' '), "eus"}, /* Basque */
    {HZ_LANGUAGE_EVENKI, "Evenki", HZ_TAG('E','V','K',' '), "evn"}, /* Evenki */
    {HZ_LANGUAGE_EVEN, "Even", HZ_TAG('E','V','N',' '), "eve"}, /* Even */
    {HZ_LANGUAGE_EWE, "Ewe", HZ_TAG('E','W','E',' '), "ewe"}, /* Ewe */
    {HZ_LANGUAGE_FRENCH_ANTILLEAN, "French Antillean", HZ_TAG('F','A','N',' '), "acf"}, /* French Antillean */
    {HZ_LANGUAGE_FANG, "Fang", HZ_TAG('F','A','N','0'), "fan"}, /* Fang */
    {HZ_LANGUAGE_PERSIAN, "Persian", HZ_TAG('F','A','R',' '), "fas"}, /* Persian */
    {HZ_LANGUAGE_FANTI, "Fanti", HZ_TAG('F','A','T',' '), "fat"}, /* Fanti */
    {HZ_LANGUAGE_FINNISH, "Finnish", HZ_TAG('F','I','N',' '), "fin"}, /* Finnish */
    {HZ_LANGUAGE_FIJIAN, "Fijian", HZ_TAG('F','J','I',' '), "fij"}, /* Fijian */
    {HZ_LANGUAGE_DUTCH_FLEMISH, "Dutch (Flemish)", HZ_TAG('F','L','E',' '), "vls"}, /* Dutch (Flemish) */
    {HZ_LANGUAGE_FEFE, "Fefe", HZ_TAG('F','M','P',' '), "fmp"}, /* Fefe */
    {HZ_LANGUAGE_FOREST_ENETS, "Forest Enets", HZ_TAG('F','N','E',' '), "enf"}, /* Forest Enets */
    {HZ_LANGUAGE_FON, "Fon", HZ_TAG('F','O','N',' '), "fon"}, /* Fon */
    {HZ_LANGUAGE_FAROESE, "Faroese", HZ_TAG('F','O','S',' '), "fao"}, /* Faroese */
    {HZ_LANGUAGE_FRENCH, "French", HZ_TAG('F','R','A',' '), "fra"}, /* French */
    {HZ_LANGUAGE_CAJUN_FRENCH, "Cajun French", HZ_TAG('F','R','C',' '), "frc"}, /* Cajun French */
    {HZ_LANGUAGE_FRISIAN, "Frisian", HZ_TAG('F','R','I',' '), "fry"}, /* Frisian */
    {HZ_LANGUAGE_FRIULIAN, "Friulian", HZ_TAG('F','R','L',' '), "fur"}, /* Friulian */
    {HZ_LANGUAGE_ARPITAN, "Arpitan", HZ_TAG('F','R','P',' '), "frp"}, /* Arpitan */
    {HZ_LANGUAGE_FUTA, "Futa", HZ_TAG('F','T','A',' '), "fuf"}, /* Futa */
    {HZ_LANGUAGE_FULAH, "Fulah", HZ_TAG('F','U','L',' '), "ful"}, /* Fulah */
    {HZ_LANGUAGE_NIGERIAN_FULFULDE, "Nigerian Fulfulde", HZ_TAG('F','U','V',' '), "fuv"}, /* Nigerian Fulfulde */
    {HZ_LANGUAGE_GA, "Ga", HZ_TAG('G','A','D',' '), "gaa"}, /* Ga */
    {HZ_LANGUAGE_SCOTTISH_GAELIC_GAELIC, "Scottish Gaelic (Gaelic)", HZ_TAG('G','A','E',' '), "gla"}, /* Scottish Gaelic (Gaelic) */
    {HZ_LANGUAGE_GAGAUZ, "Gagauz", HZ_TAG('G','A','G',' '), "gag"}, /* Gagauz */
    {HZ_LANGUAGE_GALICIAN, "Galician", HZ_TAG('G','A','L',' '), "glg"}, /* Galician */
    {HZ_LANGUAGE_GARSHUNI, "Garshuni", HZ_TAG('G','A','R',' '), NULL}, /* Garshuni */
    {HZ_LANGUAGE_GARHWALI, "Garhwali", HZ_TAG('G','A','W',' '), "gbm"}, /* Garhwali */
    {HZ_LANGUAGE_GEEZ, "Geez", HZ_TAG('G','E','Z',' '), "gez"}, /* Geez */
    {HZ_LANGUAGE_GITHABUL, "Githabul", HZ_TAG('G','I','H',' '), "gih"}, /* Githabul */
    {HZ_LANGUAGE_GILYAK, "Gilyak", HZ_TAG('G','I','L',' '), "niv"}, /* Gilyak */
    {HZ_LANGUAGE_KIRIBATI_GILBERTESE, "Kiribati (Gilbertese)", HZ_TAG('G','I','L','0'), "gil"}, /* Kiribati (Gilbertese) */
    {HZ_LANGUAGE_KPELLE_GUINEA, "Kpelle (Guinea)", HZ_TAG('G','K','P',' '), "gkp"}, /* Kpelle (Guinea) */
    {HZ_LANGUAGE_GILAKI, "Gilaki", HZ_TAG('G','L','K',' '), "glk"}, /* Gilaki */
    {HZ_LANGUAGE_GUMUZ, "Gumuz", HZ_TAG('G','M','Z',' '), "guk"}, /* Gumuz */
    {HZ_LANGUAGE_GUMATJ, "Gumatj", HZ_TAG('G','N','N',' '), "gnn"}, /* Gumatj */
    {HZ_LANGUAGE_GOGO, "Gogo", HZ_TAG('G','O','G',' '), "gog"}, /* Gogo */
    {HZ_LANGUAGE_GONDI, "Gondi", HZ_TAG('G','O','N',' '), "gon"}, /* Gondi */
    {HZ_LANGUAGE_GREENLANDIC, "Greenlandic", HZ_TAG('G','R','N',' '), "kal"}, /* Greenlandic */
    {HZ_LANGUAGE_GARO, "Garo", HZ_TAG('G','R','O',' '), "grt"}, /* Garo */
    {HZ_LANGUAGE_GUARANI, "Guarani", HZ_TAG('G','U','A',' '), "grn"}, /* Guarani */
    {HZ_LANGUAGE_WAYUU, "Wayuu", HZ_TAG('G','U','C',' '), "guc"}, /* Wayuu */
    {HZ_LANGUAGE_GUPAPUYNGU, "Gupapuyngu", HZ_TAG('G','U','F',' '), "guf"}, /* Gupapuyngu */
    {HZ_LANGUAGE_GUJARATI, "Gujarati", HZ_TAG('G','U','J',' '), "guj"}, /* Gujarati */
    {HZ_LANGUAGE_GUSII, "Gusii", HZ_TAG('G','U','Z',' '), "guz"}, /* Gusii */
    {HZ_LANGUAGE_HAITIAN_HAITIAN_CREOLE, "Haitian (Haitian Creole)", HZ_TAG('H','A','I',' '), "hat"}, /* Haitian (Haitian Creole) */
    {HZ_LANGUAGE_HALAM_FALAM_CHIN, "Halam (Falam Chin)", HZ_TAG('H','A','L',' '), "cfm"}, /* Halam (Falam Chin) */
    {HZ_LANGUAGE_HARAUTI, "Harauti", HZ_TAG('H','A','R',' '), "hoj"}, /* Harauti */
    {HZ_LANGUAGE_HAUSA, "Hausa", HZ_TAG('H','A','U',' '), "hau"}, /* Hausa */
    {HZ_LANGUAGE_HAWAIIAN, "Hawaiian", HZ_TAG('H','A','W',' '), "haw"}, /* Hawaiian */
    {HZ_LANGUAGE_HAYA, "Haya", HZ_TAG('H','A','Y',' '), "hay"}, /* Haya */
    {HZ_LANGUAGE_HAZARAGI, "Hazaragi", HZ_TAG('H','A','Z',' '), "haz"}, /* Hazaragi */
    {HZ_LANGUAGE_HAMMER_BANNA, "Hammer-Banna", HZ_TAG('H','B','N',' '), "amf"}, /* Hammer-Banna */
    {HZ_LANGUAGE_HERERO, "Herero", HZ_TAG('H','E','R',' '), "her"}, /* Herero */
    {HZ_LANGUAGE_HILIGAYNON, "Hiligaynon", HZ_TAG('H','I','L',' '), "hil"}, /* Hiligaynon */
    {HZ_LANGUAGE_HINDI, "Hindi", HZ_TAG('H','I','N',' '), "hin"}, /* Hindi */
    {HZ_LANGUAGE_HIGH_MARI, "High Mari", HZ_TAG('H','M','A',' '), "mrj"}, /* High Mari */
    {HZ_LANGUAGE_HMONG, "Hmong", HZ_TAG('H','M','N',' '), "hmn"}, /* Hmong */
    {HZ_LANGUAGE_HIRI_MOTU, "Hiri Motu", HZ_TAG('H','M','O',' '), "hmo"}, /* Hiri Motu */
    {HZ_LANGUAGE_HINDKO, "Hindko", HZ_TAG('H','N','D',' '), "hno:hnd"}, /* Hindko */
    {HZ_LANGUAGE_HO, "Ho", HZ_TAG('H','O',' ',' '), "hoc"}, /* Ho */
    {HZ_LANGUAGE_HARARI, "Harari", HZ_TAG('H','R','I',' '), "har"}, /* Harari */
    {HZ_LANGUAGE_CROATIAN, "Croatian", HZ_TAG('H','R','V',' '), "hrv"}, /* Croatian */
    {HZ_LANGUAGE_HUNGARIAN, "Hungarian", HZ_TAG('H','U','N',' '), "hun"}, /* Hungarian */
    {HZ_LANGUAGE_ARMENIAN, "Armenian", HZ_TAG('H','Y','E',' '), "hye:hyw"}, /* Armenian */
    {HZ_LANGUAGE_ARMENIAN_EAST, "Armenian East", HZ_TAG('H','Y','E','0'), "hye"}, /* Armenian East */
    {HZ_LANGUAGE_IBAN, "Iban", HZ_TAG('I','B','A',' '), "iba"}, /* Iban */
    {HZ_LANGUAGE_IBIBIO, "Ibibio", HZ_TAG('I','B','B',' '), "ibb"}, /* Ibibio */
    {HZ_LANGUAGE_IGBO, "Igbo", HZ_TAG('I','B','O',' '), "ibo"}, /* Igbo */
    {HZ_LANGUAGE_IDO, "Ido", HZ_TAG('I','D','O',' '), "ido"}, /* Ido */
    {HZ_LANGUAGE_IJO_LANGUAGES, "Ijo languages", HZ_TAG('I','J','O',' '), "iby:ijc:ije:ijn:ijo:ijs:nkx:okd:okr:orr"}, /* Ijo languages */
    {HZ_LANGUAGE_INTERLINGUE, "Interlingue", HZ_TAG('I','L','E',' '), "ile"}, /* Interlingue */
    {HZ_LANGUAGE_ILOKANO, "Ilokano", HZ_TAG('I','L','O',' '), "ilo"}, /* Ilokano */
    {HZ_LANGUAGE_INTERLINGUA, "Interlingua", HZ_TAG('I','N','A',' '), "ina"}, /* Interlingua */
    {HZ_LANGUAGE_INDONESIAN, "Indonesian", HZ_TAG('I','N','D',' '), "ind"}, /* Indonesian */
    {HZ_LANGUAGE_INGUSH, "Ingush", HZ_TAG('I','N','G',' '), "inh"}, /* Ingush */
    {HZ_LANGUAGE_INUKTITUT, "Inuktitut", HZ_TAG('I','N','U',' '), "iku"}, /* Inuktitut */
    {HZ_LANGUAGE_INUPIAT, "Inupiat", HZ_TAG('I','P','K',' '), "ipk"}, /* Inupiat */
    {HZ_LANGUAGE_PHONETIC_TRANSCRIPTION_IPA_CONVENTIONS, "Phonetic transcription-IPA conventions", HZ_TAG('I','P','P','H'), NULL}, /* Phonetic transcription-IPA conventions */
    {HZ_LANGUAGE_IRISH, "Irish", HZ_TAG('I','R','I',' '), "gle"}, /* Irish */
    {HZ_LANGUAGE_IRISH_TRADITIONAL, "Irish Traditional", HZ_TAG('I','R','T',' '), "gle"}, /* Irish Traditional */
    {HZ_LANGUAGE_ICELANDIC, "Icelandic", HZ_TAG('I','S','L',' '), "isl"}, /* Icelandic */
    {HZ_LANGUAGE_INARI_SAMI, "Inari Sami", HZ_TAG('I','S','M',' '), "smn"}, /* Inari Sami */
    {HZ_LANGUAGE_ITALIAN, "Italian", HZ_TAG('I','T','A',' '), "ita"}, /* Italian */
    {HZ_LANGUAGE_HEBREW, "Hebrew", HZ_TAG('I','W','R',' '), "heb"}, /* Hebrew */
    {HZ_LANGUAGE_JAMAICAN_CREOLE, "Jamaican Creole", HZ_TAG('J','A','M',' '), "jam"}, /* Jamaican Creole */
    {HZ_LANGUAGE_JAPANESE, "Japanese", HZ_TAG('J','A','N',' '), "jpn"}, /* Japanese */
    {HZ_LANGUAGE_JAVANESE, "Javanese", HZ_TAG('J','A','V',' '), "jav"}, /* Javanese */
    {HZ_LANGUAGE_LOJBAN, "Lojban", HZ_TAG('J','B','O',' '), "jbo"}, /* Lojban */
    {HZ_LANGUAGE_KRYMCHAK, "Krymchak", HZ_TAG('J','C','T',' '), "jct"}, /* Krymchak */
    {HZ_LANGUAGE_YIDDISH, "Yiddish", HZ_TAG('J','I','I',' '), "yid"}, /* Yiddish */
    {HZ_LANGUAGE_LADINO, "Ladino", HZ_TAG('J','U','D',' '), "lad"}, /* Ladino */
    {HZ_LANGUAGE_JULA, "Jula", HZ_TAG('J','U','L',' '), "dyu"}, /* Jula */
    {HZ_LANGUAGE_KABARDIAN, "Kabardian", HZ_TAG('K','A','B',' '), "kbd"}, /* Kabardian */
    {HZ_LANGUAGE_KABYLE, "Kabyle", HZ_TAG('K','A','B','0'), "kab"}, /* Kabyle */
    {HZ_LANGUAGE_KACHCHI, "Kachchi", HZ_TAG('K','A','C',' '), "kfr"}, /* Kachchi */
    {HZ_LANGUAGE_KALENJIN, "Kalenjin", HZ_TAG('K','A','L',' '), "kln"}, /* Kalenjin */
    {HZ_LANGUAGE_KANNADA, "Kannada", HZ_TAG('K','A','N',' '), "kan"}, /* Kannada */
    {HZ_LANGUAGE_KARACHAY, "Karachay", HZ_TAG('K','A','R',' '), "krc"}, /* Karachay */
    {HZ_LANGUAGE_GEORGIAN, "Georgian", HZ_TAG('K','A','T',' '), "kat"}, /* Georgian */
    {HZ_LANGUAGE_KAZAKH, "Kazakh", HZ_TAG('K','A','Z',' '), "kaz"}, /* Kazakh */
    {HZ_LANGUAGE_MAKONDE, "Makonde", HZ_TAG('K','D','E',' '), "kde"}, /* Makonde */
    {HZ_LANGUAGE_KABUVERDIANU_CRIOULO, "Kabuverdianu (Crioulo)", HZ_TAG('K','E','A',' '), "kea"}, /* Kabuverdianu (Crioulo) */
    {HZ_LANGUAGE_KEBENA, "Kebena", HZ_TAG('K','E','B',' '), "ktb"}, /* Kebena */
    {HZ_LANGUAGE_KEKCHI, "Kekchi", HZ_TAG('K','E','K',' '), "kek"}, /* Kekchi */
    {HZ_LANGUAGE_KHUTSURI_GEORGIAN, "Khutsuri Georgian", HZ_TAG('K','G','E',' '), "kat"}, /* Khutsuri Georgian */
    {HZ_LANGUAGE_KHAKASS, "Khakass", HZ_TAG('K','H','A',' '), "kjh"}, /* Khakass */
    {HZ_LANGUAGE_KHANTY_KAZIM, "Khanty-Kazim", HZ_TAG('K','H','K',' '), "kca"}, /* Khanty-Kazim */
    {HZ_LANGUAGE_KHMER, "Khmer", HZ_TAG('K','H','M',' '), "khm"}, /* Khmer */
    {HZ_LANGUAGE_KHANTY_SHURISHKAR, "Khanty-Shurishkar", HZ_TAG('K','H','S',' '), "kca"}, /* Khanty-Shurishkar */
    {HZ_LANGUAGE_KHAMTI_SHAN, "Khamti Shan", HZ_TAG('K','H','T',' '), "kht"}, /* Khamti Shan */
    {HZ_LANGUAGE_KHANTY_VAKHI, "Khanty-Vakhi", HZ_TAG('K','H','V',' '), "kca"}, /* Khanty-Vakhi */
    {HZ_LANGUAGE_KHOWAR, "Khowar", HZ_TAG('K','H','W',' '), "khw"}, /* Khowar */
    {HZ_LANGUAGE_KIKUYU_GIKUYU, "Kikuyu (Gikuyu)", HZ_TAG('K','I','K',' '), "kik"}, /* Kikuyu (Gikuyu) */
    {HZ_LANGUAGE_KIRGHIZ_KYRGYZ, "Kirghiz (Kyrgyz)", HZ_TAG('K','I','R',' '), "kir"}, /* Kirghiz (Kyrgyz) */
    {HZ_LANGUAGE_KISII, "Kisii", HZ_TAG('K','I','S',' '), "kqs:kss"}, /* Kisii */
    {HZ_LANGUAGE_KIRMANJKI, "Kirmanjki", HZ_TAG('K','I','U',' '), "kiu"}, /* Kirmanjki */
    {HZ_LANGUAGE_SOUTHERN_KIWAI, "Southern Kiwai", HZ_TAG('K','J','D',' '), "kjd"}, /* Southern Kiwai */
    {HZ_LANGUAGE_EASTERN_PWO_KAREN, "Eastern Pwo Karen", HZ_TAG('K','J','P',' '), "kjp"}, /* Eastern Pwo Karen */
    {HZ_LANGUAGE_BUMTHANGKHA, "Bumthangkha", HZ_TAG('K','J','Z',' '), "kjz"}, /* Bumthangkha */
    {HZ_LANGUAGE_KOKNI, "Kokni", HZ_TAG('K','K','N',' '), "kex"}, /* Kokni */
    {HZ_LANGUAGE_KALMYK, "Kalmyk", HZ_TAG('K','L','M',' '), "xal"}, /* Kalmyk */
    {HZ_LANGUAGE_KAMBA, "Kamba", HZ_TAG('K','M','B',' '), "kam"}, /* Kamba */
    {HZ_LANGUAGE_KUMAONI, "Kumaoni", HZ_TAG('K','M','N',' '), "kfy"}, /* Kumaoni */
    {HZ_LANGUAGE_KOMO, "Komo", HZ_TAG('K','M','O',' '), "kmw"}, /* Komo */
    {HZ_LANGUAGE_KOMSO, "Komso", HZ_TAG('K','M','S',' '), "kxc"}, /* Komso */
    {HZ_LANGUAGE_KHORASANI_TURKIC, "Khorasani Turkic", HZ_TAG('K','M','Z',' '), "kmz"}, /* Khorasani Turkic */
    {HZ_LANGUAGE_KANURI, "Kanuri", HZ_TAG('K','N','R',' '), "kau"}, /* Kanuri */
    {HZ_LANGUAGE_KODAGU, "Kodagu", HZ_TAG('K','O','D',' '), "kfa"}, /* Kodagu */
    {HZ_LANGUAGE_KOREAN_OLD_HANGUL, "Korean Old Hangul", HZ_TAG('K','O','H',' '), "kor:okm"}, /* Korean Old Hangul */
    {HZ_LANGUAGE_KONKANI, "Konkani", HZ_TAG('K','O','K',' '), "kok"}, /* Konkani */
    {HZ_LANGUAGE_KOMI, "Komi", HZ_TAG('K','O','M',' '), "kom"}, /* Komi */
    {HZ_LANGUAGE_KIKONGO, "Kikongo", HZ_TAG('K','O','N',' '), "ktu"}, /* Kikongo */
    {HZ_LANGUAGE_KONGO, "Kongo", HZ_TAG('K','O','N','0'), "kon"}, /* Kongo */
    {HZ_LANGUAGE_KOMI_PERMYAK, "Komi-Permyak", HZ_TAG('K','O','P',' '), "koi"}, /* Komi-Permyak */
    {HZ_LANGUAGE_KOREAN, "Korean", HZ_TAG('K','O','R',' '), "kor"}, /* Korean */
    {HZ_LANGUAGE_KOSRAEAN, "Kosraean", HZ_TAG('K','O','S',' '), "kos"}, /* Kosraean */
    {HZ_LANGUAGE_KOMI_ZYRIAN, "Komi-Zyrian", HZ_TAG('K','O','Z',' '), "kpv"}, /* Komi-Zyrian */
    {HZ_LANGUAGE_KPELLE, "Kpelle", HZ_TAG('K','P','L',' '), "kpe"}, /* Kpelle */
    {HZ_LANGUAGE_KRIO, "Krio", HZ_TAG('K','R','I',' '), "kri"}, /* Krio */
    {HZ_LANGUAGE_KARAKALPAK, "Karakalpak", HZ_TAG('K','R','K',' '), "kaa"}, /* Karakalpak */
    {HZ_LANGUAGE_KARELIAN, "Karelian", HZ_TAG('K','R','L',' '), "krl"}, /* Karelian */
    {HZ_LANGUAGE_KARAIM, "Karaim", HZ_TAG('K','R','M',' '), "kdr"}, /* Karaim */
    {HZ_LANGUAGE_KAREN, "Karen", HZ_TAG('K','R','N',' '), "blk:bwe:eky:ghk:jkm:jkp:kar:kjp:kjt:ksw:kvl:kvq:kvt:kvu:kvy:kxf:kxk:kyu:pdu:pwo:pww:wea"}, /* Karen */
    {HZ_LANGUAGE_KOORETE, "Koorete", HZ_TAG('K','R','T',' '), "kqy"}, /* Koorete */
    {HZ_LANGUAGE_KASHMIRI, "Kashmiri", HZ_TAG('K','S','H',' '), "kas"}, /* Kashmiri */
    {HZ_LANGUAGE_RIPUARIAN, "Ripuarian", HZ_TAG('K','S','H','0'), "ksh"}, /* Ripuarian */
    {HZ_LANGUAGE_KHASI, "Khasi", HZ_TAG('K','S','I',' '), "kha"}, /* Khasi */
    {HZ_LANGUAGE_KILDIN_SAMI, "Kildin Sami", HZ_TAG('K','S','M',' '), "sjd"}, /* Kildin Sami */
    {HZ_LANGUAGE_SGAW_KAREN, "Sgaw Karen", HZ_TAG('K','S','W',' '), "ksw"}, /* Sgaw Karen */
    {HZ_LANGUAGE_KUANYAMA, "Kuanyama", HZ_TAG('K','U','A',' '), "kua"}, /* Kuanyama */
    {HZ_LANGUAGE_KUI, "Kui", HZ_TAG('K','U','I',' '), "kxu"}, /* Kui */
    {HZ_LANGUAGE_KULVI, "Kulvi", HZ_TAG('K','U','L',' '), "kfx"}, /* Kulvi */
    {HZ_LANGUAGE_KUMYK, "Kumyk", HZ_TAG('K','U','M',' '), "kum"}, /* Kumyk */
    {HZ_LANGUAGE_KURDISH, "Kurdish", HZ_TAG('K','U','R',' '), "kur"}, /* Kurdish */
    {HZ_LANGUAGE_KURUKH, "Kurukh", HZ_TAG('K','U','U',' '), "kru"}, /* Kurukh */
    {HZ_LANGUAGE_KUY, "Kuy", HZ_TAG('K','U','Y',' '), "kdt"}, /* Kuy */
    {HZ_LANGUAGE_KORYAK, "Koryak", HZ_TAG('K','Y','K',' '), "kpy"}, /* Koryak */
    {HZ_LANGUAGE_WESTERN_KAYAH, "Western Kayah", HZ_TAG('K','Y','U',' '), "kyu"}, /* Western Kayah */
    {HZ_LANGUAGE_LADIN, "Ladin", HZ_TAG('L','A','D',' '), "lld"}, /* Ladin */
    {HZ_LANGUAGE_LAHULI, "Lahuli", HZ_TAG('L','A','H',' '), "bfu"}, /* Lahuli */
    {HZ_LANGUAGE_LAK, "Lak", HZ_TAG('L','A','K',' '), "lbe"}, /* Lak */
    {HZ_LANGUAGE_LAMBANI, "Lambani", HZ_TAG('L','A','M',' '), "lmn"}, /* Lambani */
    {HZ_LANGUAGE_LAO, "Lao", HZ_TAG('L','A','O',' '), "lao"}, /* Lao */
    {HZ_LANGUAGE_LATIN, "Latin", HZ_TAG('L','A','T',' '), "lat"}, /* Latin */
    {HZ_LANGUAGE_LAZ, "Laz", HZ_TAG('L','A','Z',' '), "lzz"}, /* Laz */
    {HZ_LANGUAGE_L_CREE, "L-Cree", HZ_TAG('L','C','R',' '), "crm"}, /* L-Cree */
    {HZ_LANGUAGE_LADAKHI, "Ladakhi", HZ_TAG('L','D','K',' '), "lbj"}, /* Ladakhi */
    {HZ_LANGUAGE_LEZGI, "Lezgi", HZ_TAG('L','E','Z',' '), "lez"}, /* Lezgi */
    {HZ_LANGUAGE_LIGURIAN, "Ligurian", HZ_TAG('L','I','J',' '), "lij"}, /* Ligurian */
    {HZ_LANGUAGE_LIMBURGISH, "Limburgish", HZ_TAG('L','I','M',' '), "lim"}, /* Limburgish */
    {HZ_LANGUAGE_LINGALA, "Lingala", HZ_TAG('L','I','N',' '), "lin"}, /* Lingala */
    {HZ_LANGUAGE_LISU, "Lisu", HZ_TAG('L','I','S',' '), "lis"}, /* Lisu */
    {HZ_LANGUAGE_LAMPUNG, "Lampung", HZ_TAG('L','J','P',' '), "ljp"}, /* Lampung */
    {HZ_LANGUAGE_LAKI, "Laki", HZ_TAG('L','K','I',' '), "lki"}, /* Laki */
    {HZ_LANGUAGE_LOW_MARI, "Low Mari", HZ_TAG('L','M','A',' '), "mhr"}, /* Low Mari */
    {HZ_LANGUAGE_LIMBU, "Limbu", HZ_TAG('L','M','B',' '), "lif"}, /* Limbu */
    {HZ_LANGUAGE_LOMBARD, "Lombard", HZ_TAG('L','M','O',' '), "lmo"}, /* Lombard */
    {HZ_LANGUAGE_LOMWE, "Lomwe", HZ_TAG('L','M','W',' '), "ngl"}, /* Lomwe */
    {HZ_LANGUAGE_LOMA, "Loma", HZ_TAG('L','O','M',' '), "lom"}, /* Loma */
    {HZ_LANGUAGE_LURI, "Luri", HZ_TAG('L','R','C',' '), "lrc:luz:bqi:zum"}, /* Luri */
    {HZ_LANGUAGE_LOWER_SORBIAN, "Lower Sorbian", HZ_TAG('L','S','B',' '), "dsb"}, /* Lower Sorbian */
    {HZ_LANGUAGE_LULE_SAMI, "Lule Sami", HZ_TAG('L','S','M',' '), "smj"}, /* Lule Sami */
    {HZ_LANGUAGE_LITHUANIAN, "Lithuanian", HZ_TAG('L','T','H',' '), "lit"}, /* Lithuanian */
    {HZ_LANGUAGE_LUXEMBOURGISH, "Luxembourgish", HZ_TAG('L','T','Z',' '), "ltz"}, /* Luxembourgish */
    {HZ_LANGUAGE_LUBA_LULUA, "Luba-Lulua", HZ_TAG('L','U','A',' '), "lua"}, /* Luba-Lulua */
    {HZ_LANGUAGE_LUBA_KATANGA, "Luba-Katanga", HZ_TAG('L','U','B',' '), "lub"}, /* Luba-Katanga */
    {HZ_LANGUAGE_GANDA, "Ganda", HZ_TAG('L','U','G',' '), "lug"}, /* Ganda */
    {HZ_LANGUAGE_LUYIA, "Luyia", HZ_TAG('L','U','H',' '), "luy"}, /* Luyia */
    {HZ_LANGUAGE_LUO, "Luo", HZ_TAG('L','U','O',' '), "luo"}, /* Luo */
    {HZ_LANGUAGE_LATVIAN, "Latvian", HZ_TAG('L','V','I',' '), "lav"}, /* Latvian */
    {HZ_LANGUAGE_MADURA, "Madura", HZ_TAG('M','A','D',' '), "mad"}, /* Madura */
    {HZ_LANGUAGE_MAGAHI, "Magahi", HZ_TAG('M','A','G',' '), "mag"}, /* Magahi */
    {HZ_LANGUAGE_MARSHALLESE, "Marshallese", HZ_TAG('M','A','H',' '), "mah"}, /* Marshallese */
    {HZ_LANGUAGE_MAJANG, "Majang", HZ_TAG('M','A','J',' '), "mpe"}, /* Majang */
    {HZ_LANGUAGE_MAKHUWA, "Makhuwa", HZ_TAG('M','A','K',' '), "vmw"}, /* Makhuwa */
    {HZ_LANGUAGE_MALAYALAM, "Malayalam", HZ_TAG('M','A','L',' '), "mal"}, /* Malayalam */
    {HZ_LANGUAGE_MAM, "Mam", HZ_TAG('M','A','M',' '), "mam"}, /* Mam */
    {HZ_LANGUAGE_MANSI, "Mansi", HZ_TAG('M','A','N',' '), "mns"}, /* Mansi */
    {HZ_LANGUAGE_MAPUDUNGUN, "Mapudungun", HZ_TAG('M','A','P',' '), "arn"}, /* Mapudungun */
    {HZ_LANGUAGE_MARATHI, "Marathi", HZ_TAG('M','A','R',' '), "mar"}, /* Marathi */
    {HZ_LANGUAGE_MARWARI, "Marwari", HZ_TAG('M','A','W',' '), "mwr:dhd:rwr:mve:wry:mtr:swv"}, /* Marwari */
    {HZ_LANGUAGE_MBUNDU, "Mbundu", HZ_TAG('M','B','N',' '), "kmb"}, /* Mbundu */
    {HZ_LANGUAGE_MBO, "Mbo", HZ_TAG('M','B','O',' '), "mbo"}, /* Mbo */
    {HZ_LANGUAGE_MANCHU, "Manchu", HZ_TAG('M','C','H',' '), "mnc"}, /* Manchu */
    {HZ_LANGUAGE_MOOSE_CREE, "Moose Cree", HZ_TAG('M','C','R',' '), "crm"}, /* Moose Cree */
    {HZ_LANGUAGE_MENDE, "Mende", HZ_TAG('M','D','E',' '), "men"}, /* Mende */
    {HZ_LANGUAGE_MANDAR, "Mandar", HZ_TAG('M','D','R',' '), "mdr"}, /* Mandar */
    {HZ_LANGUAGE_MEEN, "Meen", HZ_TAG('M','E','N',' '), "mym"}, /* Meen */
    {HZ_LANGUAGE_MERU, "Meru", HZ_TAG('M','E','R',' '), "mer"}, /* Meru */
    {HZ_LANGUAGE_PATTANI_MALAY, "Pattani Malay", HZ_TAG('M','F','A',' '), "mfa"}, /* Pattani Malay */
    {HZ_LANGUAGE_MORISYEN, "Morisyen", HZ_TAG('M','F','E',' '), "mfe"}, /* Morisyen */
    {HZ_LANGUAGE_MINANGKABAU, "Minangkabau", HZ_TAG('M','I','N',' '), "min"}, /* Minangkabau */
    {HZ_LANGUAGE_MIZO, "Mizo", HZ_TAG('M','I','Z',' '), "lus"}, /* Mizo */
    {HZ_LANGUAGE_MACEDONIAN, "Macedonian", HZ_TAG('M','K','D',' '), "mkd"}, /* Macedonian */
    {HZ_LANGUAGE_MAKASAR, "Makasar", HZ_TAG('M','K','R',' '), "mak"}, /* Makasar */
    {HZ_LANGUAGE_KITUBA, "Kituba", HZ_TAG('M','K','W',' '), "mkw"}, /* Kituba */
    {HZ_LANGUAGE_MALE, "Male", HZ_TAG('M','L','E',' '), "mdy"}, /* Male */
    {HZ_LANGUAGE_MALAGASY, "Malagasy", HZ_TAG('M','L','G',' '), "mlg"}, /* Malagasy */
    {HZ_LANGUAGE_MALINKE, "Malinke", HZ_TAG('M','L','N',' '), "mlq"}, /* Malinke */
    {HZ_LANGUAGE_MALAYALAM_REFORMED, "Malayalam Reformed", HZ_TAG('M','L','R',' '), "mal"}, /* Malayalam Reformed */
    {HZ_LANGUAGE_MALAY, "Malay", HZ_TAG('M','L','Y',' '), "msa"}, /* Malay */
    {HZ_LANGUAGE_MANDINKA, "Mandinka", HZ_TAG('M','N','D',' '), "mnk"}, /* Mandinka */
    {HZ_LANGUAGE_MONGOLIAN, "Mongolian", HZ_TAG('M','N','G',' '), "mon"}, /* Mongolian */
    {HZ_LANGUAGE_MANIPURI, "Manipuri", HZ_TAG('M','N','I',' '), "mni"}, /* Manipuri */
    {HZ_LANGUAGE_MANINKA, "Maninka", HZ_TAG('M','N','K',' '), "man:mnk:myq:mku:msc:emk:mwk:mlq"}, /* Maninka */
    {HZ_LANGUAGE_MANX, "Manx", HZ_TAG('M','N','X',' '), "glv"}, /* Manx */
    {HZ_LANGUAGE_MOHAWK, "Mohawk", HZ_TAG('M','O','H',' '), "moh"}, /* Mohawk */
    {HZ_LANGUAGE_MOKSHA, "Moksha", HZ_TAG('M','O','K',' '), "mdf"}, /* Moksha */
    {HZ_LANGUAGE_MOLDAVIAN, "Moldavian", HZ_TAG('M','O','L',' '), "mol"}, /* Moldavian */
    {HZ_LANGUAGE_MON, "Mon", HZ_TAG('M','O','N',' '), "mnw"}, /* Mon */
    {HZ_LANGUAGE_MOROCCAN, "Moroccan", HZ_TAG('M','O','R',' '), NULL}, /* Moroccan */
    {HZ_LANGUAGE_MOSSI, "Mossi", HZ_TAG('M','O','S',' '), "mos"}, /* Mossi */
    {HZ_LANGUAGE_MAORI, "Maori", HZ_TAG('M','R','I',' '), "mri"}, /* Maori */
    {HZ_LANGUAGE_MAITHILI, "Maithili", HZ_TAG('M','T','H',' '), "mai"}, /* Maithili */
    {HZ_LANGUAGE_MALTESE, "Maltese", HZ_TAG('M','T','S',' '), "mlt"}, /* Maltese */
    {HZ_LANGUAGE_MUNDARI, "Mundari", HZ_TAG('M','U','N',' '), "unr"}, /* Mundari */
    {HZ_LANGUAGE_MUSCOGEE, "Muscogee", HZ_TAG('M','U','S',' '), "mus"}, /* Muscogee */
    {HZ_LANGUAGE_MIRANDESE, "Mirandese", HZ_TAG('M','W','L',' '), "mwl"}, /* Mirandese */
    {HZ_LANGUAGE_HMONG_DAW, "Hmong Daw", HZ_TAG('M','W','W',' '), "mww"}, /* Hmong Daw */
    {HZ_LANGUAGE_MAYAN, "Mayan", HZ_TAG('M','Y','N',' '), "acr:agu:caa:cac:cak:chf:ckz:cob:ctu:emy:hus:itz:ixl:jac:kek:kjb:knj:lac:mam:mhc:mop:myn:poc:poh:quc:qum:quv:toj:ttc:tzh:tzj:tzo:usp:yua"}, /* Mayan */
    {HZ_LANGUAGE_MAZANDERANI, "Mazanderani", HZ_TAG('M','Z','N',' '), "mzn"}, /* Mazanderani */
    {HZ_LANGUAGE_NAGA_ASSAMESE, "Naga-Assamese", HZ_TAG('N','A','G',' '), "nag"}, /* Naga-Assamese */
    {HZ_LANGUAGE_NAHUATL, "Nahuatl", HZ_TAG('N','A','H',' '), "azd:azn:azz:nah:naz:nch:nci:ncj:ncl:ncx:ngu:nhc:nhe:nhg:nhi:nhk:nhm:nhn:nhp:nhq:nht:nhv:nhw:nhx:nhy:nhz:nlv:npl:nsu:nuz"}, /* Nahuatl */
    {HZ_LANGUAGE_NANAI, "Nanai", HZ_TAG('N','A','N',' '), "gld"}, /* Nanai */
    {HZ_LANGUAGE_NEAPOLITAN, "Neapolitan", HZ_TAG('N','A','P',' '), "nap"}, /* Neapolitan */
    {HZ_LANGUAGE_NASKAPI, "Naskapi", HZ_TAG('N','A','S',' '), "nsk"}, /* Naskapi */
    {HZ_LANGUAGE_NAURUAN, "Nauruan", HZ_TAG('N','A','U',' '), "nau"}, /* Nauruan */
    {HZ_LANGUAGE_NAVAJO, "Navajo", HZ_TAG('N','A','V',' '), "nav"}, /* Navajo */
    {HZ_LANGUAGE_N_CREE, "N-Cree", HZ_TAG('N','C','R',' '), "csw"}, /* N-Cree */
    {HZ_LANGUAGE_NDEBELE, "Ndebele", HZ_TAG('N','D','B',' '), "nbl:nde"}, /* Ndebele */
    {HZ_LANGUAGE_NDAU, "Ndau", HZ_TAG('N','D','C',' '), "ndc"}, /* Ndau */
    {HZ_LANGUAGE_NDONGA, "Ndonga", HZ_TAG('N','D','G',' '), "ndo"}, /* Ndonga */
    {HZ_LANGUAGE_LOW_SAXON, "Low Saxon", HZ_TAG('N','D','S',' '), "nds"}, /* Low Saxon */
    {HZ_LANGUAGE_NEPALI, "Nepali", HZ_TAG('N','E','P',' '), "nep"}, /* Nepali */
    {HZ_LANGUAGE_NEWARI, "Newari", HZ_TAG('N','E','W',' '), "new"}, /* Newari */
    {HZ_LANGUAGE_NGBAKA, "Ngbaka", HZ_TAG('N','G','A',' '), "nga"}, /* Ngbaka */
    {HZ_LANGUAGE_NAGARI, "Nagari", HZ_TAG('N','G','R',' '), NULL}, /* Nagari */
    {HZ_LANGUAGE_NORWAY_HOUSE_CREE, "Norway House Cree", HZ_TAG('N','H','C',' '), "csw"}, /* Norway House Cree */
    {HZ_LANGUAGE_NISI, "Nisi", HZ_TAG('N','I','S',' '), "dap:njz:tgj"}, /* Nisi */
    {HZ_LANGUAGE_NIUEAN, "Niuean", HZ_TAG('N','I','U',' '), "niu"}, /* Niuean */
    {HZ_LANGUAGE_NYANKOLE, "Nyankole", HZ_TAG('N','K','L',' '), "nyn"}, /* Nyankole */
    {HZ_LANGUAGE_NKO, "NKo", HZ_TAG('N','K','O',' '), "nqo"}, /* NKo */
    {HZ_LANGUAGE_DUTCH, "Dutch", HZ_TAG('N','L','D',' '), "nld"}, /* Dutch */
    {HZ_LANGUAGE_NIMADI, "Nimadi", HZ_TAG('N','O','E',' '), "noe"}, /* Nimadi */
    {HZ_LANGUAGE_NOGAI, "Nogai", HZ_TAG('N','O','G',' '), "nog"}, /* Nogai */
    {HZ_LANGUAGE_NORWEGIAN, "Norwegian", HZ_TAG('N','O','R',' '), "nob"}, /* Norwegian */
    {HZ_LANGUAGE_NOVIAL, "Novial", HZ_TAG('N','O','V',' '), "nov"}, /* Novial */
    {HZ_LANGUAGE_NORTHERN_SAMI, "Northern Sami", HZ_TAG('N','S','M',' '), "sme"}, /* Northern Sami */
    {HZ_LANGUAGE_NORTHERN_SOTHO, "Northern Sotho", HZ_TAG('N','S','O',' '), "nso"}, /* Northern Sotho */
    {HZ_LANGUAGE_NORTHERN_TAI, "Northern Tai", HZ_TAG('N','T','A',' '), "nod"}, /* Northern Tai */
    {HZ_LANGUAGE_ESPERANTO, "Esperanto", HZ_TAG('N','T','O',' '), "epo"}, /* Esperanto */
    {HZ_LANGUAGE_NYAMWEZI, "Nyamwezi", HZ_TAG('N','Y','M',' '), "nym"}, /* Nyamwezi */
    {HZ_LANGUAGE_NORWEGIAN_NYNORSK_NYNORSK_NORWEGIAN, "Norwegian Nynorsk (Nynorsk, Norwegian)", HZ_TAG('N','Y','N',' '), "nno"}, /* Norwegian Nynorsk (Nynorsk, Norwegian) */
    {HZ_LANGUAGE_MBEMBE_TIGON, "Mbembe Tigon", HZ_TAG('N','Z','A',' '), "nza"}, /* Mbembe Tigon */
    {HZ_LANGUAGE_OCCITAN, "Occitan", HZ_TAG('O','C','I',' '), "oci"}, /* Occitan */
    {HZ_LANGUAGE_OJI_CREE, "Oji-Cree", HZ_TAG('O','C','R',' '), "ojs"}, /* Oji-Cree */
    {HZ_LANGUAGE_OJIBWAY, "Ojibway", HZ_TAG('O','J','B',' '), "oji"}, /* Ojibway */
    {HZ_LANGUAGE_ODIA_FORMERLY_ORIYA, "Odia (formerly Oriya)", HZ_TAG('O','R','I',' '), "ori"}, /* Odia (formerly Oriya) */
    {HZ_LANGUAGE_OROMO, "Oromo", HZ_TAG('O','R','O',' '), "orm"}, /* Oromo */
    {HZ_LANGUAGE_OSSETIAN, "Ossetian", HZ_TAG('O','S','S',' '), "oss"}, /* Ossetian */
    {HZ_LANGUAGE_PALESTINIAN_ARAMAIC, "Palestinian Aramaic", HZ_TAG('P','A','A',' '), "sam"}, /* Palestinian Aramaic */
    {HZ_LANGUAGE_PANGASINAN, "Pangasinan", HZ_TAG('P','A','G',' '), "pag"}, /* Pangasinan */
    {HZ_LANGUAGE_PALI, "Pali", HZ_TAG('P','A','L',' '), "pli"}, /* Pali */
    {HZ_LANGUAGE_PAMPANGAN, "Pampangan", HZ_TAG('P','A','M',' '), "pam"}, /* Pampangan */
    {HZ_LANGUAGE_PUNJABI, "Punjabi", HZ_TAG('P','A','N',' '), "pan"}, /* Punjabi */
    {HZ_LANGUAGE_PALPA, "Palpa", HZ_TAG('P','A','P',' '), "plp"}, /* Palpa */
    {HZ_LANGUAGE_PAPIAMENTU, "Papiamentu", HZ_TAG('P','A','P','0'), "pap"}, /* Papiamentu */
    {HZ_LANGUAGE_PASHTO, "Pashto", HZ_TAG('P','A','S',' '), "pus"}, /* Pashto */
    {HZ_LANGUAGE_PALAUAN, "Palauan", HZ_TAG('P','A','U',' '), "pau"}, /* Palauan */
    {HZ_LANGUAGE_BOUYEI, "Bouyei", HZ_TAG('P','C','C',' '), "pcc"}, /* Bouyei */
    {HZ_LANGUAGE_PICARD, "Picard", HZ_TAG('P','C','D',' '), "pcd"}, /* Picard */
    {HZ_LANGUAGE_PENNSYLVANIA_GERMAN, "Pennsylvania German", HZ_TAG('P','D','C',' '), "pdc"}, /* Pennsylvania German */
    {HZ_LANGUAGE_POLYTONIC_GREEK, "Polytonic Greek", HZ_TAG('P','G','R',' '), "ell"}, /* Polytonic Greek */
    {HZ_LANGUAGE_PHAKE, "Phake", HZ_TAG('P','H','K',' '), "phk"}, /* Phake */
    {HZ_LANGUAGE_NORFOLK, "Norfolk", HZ_TAG('P','I','H',' '), "pih"}, /* Norfolk */
    {HZ_LANGUAGE_FILIPINO, "Filipino", HZ_TAG('P','I','L',' '), "fil"}, /* Filipino */
    {HZ_LANGUAGE_PALAUNG, "Palaung", HZ_TAG('P','L','G',' '), "pce:rbb:pll"}, /* Palaung */
    {HZ_LANGUAGE_POLISH, "Polish", HZ_TAG('P','L','K',' '), "pol"}, /* Polish */
    {HZ_LANGUAGE_PIEMONTESE, "Piemontese", HZ_TAG('P','M','S',' '), "pms"}, /* Piemontese */
    {HZ_LANGUAGE_WESTERN_PANJABI, "Western Panjabi", HZ_TAG('P','N','B',' '), "pnb"}, /* Western Panjabi */
    {HZ_LANGUAGE_POCOMCHI, "Pocomchi", HZ_TAG('P','O','H',' '), "poh"}, /* Pocomchi */
    {HZ_LANGUAGE_POHNPEIAN, "Pohnpeian", HZ_TAG('P','O','N',' '), "pon"}, /* Pohnpeian */
    {HZ_LANGUAGE_PROVENCAL_OLD_PROVENCAL, "Provencal / Old Provencal", HZ_TAG('P','R','O',' '), "pro"}, /* Provencal / Old Provencal */
    {HZ_LANGUAGE_PORTUGUESE, "Portuguese", HZ_TAG('P','T','G',' '), "por"}, /* Portuguese */
    {HZ_LANGUAGE_WESTERN_PWO_KAREN, "Western Pwo Karen", HZ_TAG('P','W','O',' '), "pwo"}, /* Western Pwo Karen */
    {HZ_LANGUAGE_CHIN, "Chin", HZ_TAG('Q','I','N',' '), "bgr:biu:cek:cey:cfm:cbl:cka:ckn:clj:clt:cmr:cnb:cnh:cnk:cnw:csh:csj:csv:csy:ctd:cth:czt:dao:gnb:hlt:hmr:hra:lus:mrh:mwq:pck:pkh:pub:ral:rtc:sch:sez:shl:smt:tcp:tcz:vap:weu:zom:zyp"}, /* Chin */
    {HZ_LANGUAGE_KICHE, "Kiche", HZ_TAG('Q','U','C',' '), "quc"}, /* Kiche */
    {HZ_LANGUAGE_QUECHUA_BOLIVIA, "Quechua (Bolivia)", HZ_TAG('Q','U','H',' '), "quh"}, /* Quechua (Bolivia) */
    {HZ_LANGUAGE_QUECHUA, "Quechua", HZ_TAG('Q','U','Z',' '), "quz"}, /* Quechua */
    {HZ_LANGUAGE_QUECHUA_ECUADOR, "Quechua (Ecuador)", HZ_TAG('Q','V','I',' '), "qvi"}, /* Quechua (Ecuador) */
    {HZ_LANGUAGE_QUECHUA_PERU, "Quechua (Peru)", HZ_TAG('Q','W','H',' '), "qwh"}, /* Quechua (Peru) */
    {HZ_LANGUAGE_RAJASTHANI, "Rajasthani", HZ_TAG('R','A','J',' '), "raj"}, /* Rajasthani */
    {HZ_LANGUAGE_RAROTONGAN, "Rarotongan", HZ_TAG('R','A','R',' '), "rar"}, /* Rarotongan */
    {HZ_LANGUAGE_RUSSIAN_BURIAT, "Russian Buriat", HZ_TAG('R','B','U',' '), "bxr"}, /* Russian Buriat */
    {HZ_LANGUAGE_R_CREE, "R-Cree", HZ_TAG('R','C','R',' '), "atj"}, /* R-Cree */
    {HZ_LANGUAGE_REJANG, "Rejang", HZ_TAG('R','E','J',' '), "rej"}, /* Rejang */
    {HZ_LANGUAGE_RIANG, "Riang", HZ_TAG('R','I','A',' '), "ria"}, /* Riang */
    {HZ_LANGUAGE_TARIFIT, "Tarifit", HZ_TAG('R','I','F',' '), "rif"}, /* Tarifit */
    {HZ_LANGUAGE_RITARUNGO, "Ritarungo", HZ_TAG('R','I','T',' '), "rit"}, /* Ritarungo */
    {HZ_LANGUAGE_ARAKWAL, "Arakwal", HZ_TAG('R','K','W',' '), "rkw"}, /* Arakwal */
    {HZ_LANGUAGE_ROMANSH, "Romansh", HZ_TAG('R','M','S',' '), "roh"}, /* Romansh */
    {HZ_LANGUAGE_VLAX_ROMANI, "Vlax Romani", HZ_TAG('R','M','Y',' '), "rmy"}, /* Vlax Romani */
    {HZ_LANGUAGE_ROMANIAN, "Romanian", HZ_TAG('R','O','M',' '), "ron"}, /* Romanian */
    {HZ_LANGUAGE_ROMANY, "Romany", HZ_TAG('R','O','Y',' '), "rom"}, /* Romany */
    {HZ_LANGUAGE_RUSYN, "Rusyn", HZ_TAG('R','S','Y',' '), "rue"}, /* Rusyn */
    {HZ_LANGUAGE_ROTUMAN, "Rotuman", HZ_TAG('R','T','M',' '), "rtm"}, /* Rotuman */
    {HZ_LANGUAGE_KINYARWANDA, "Kinyarwanda", HZ_TAG('R','U','A',' '), "kin"}, /* Kinyarwanda */
    {HZ_LANGUAGE_RUNDI, "Rundi", HZ_TAG('R','U','N',' '), "run"}, /* Rundi */
    {HZ_LANGUAGE_AROMANIAN, "Aromanian", HZ_TAG('R','U','P',' '), "rup"}, /* Aromanian */
    {HZ_LANGUAGE_RUSSIAN, "Russian", HZ_TAG('R','U','S',' '), "rus"}, /* Russian */
    {HZ_LANGUAGE_SADRI, "Sadri", HZ_TAG('S','A','D',' '), "sck"}, /* Sadri */
    {HZ_LANGUAGE_SANSKRIT, "Sanskrit", HZ_TAG('S','A','N',' '), "san"}, /* Sanskrit */
    {HZ_LANGUAGE_SASAK, "Sasak", HZ_TAG('S','A','S',' '), "sas"}, /* Sasak */
    {HZ_LANGUAGE_SANTALI, "Santali", HZ_TAG('S','A','T',' '), "sat"}, /* Santali */
    {HZ_LANGUAGE_SAYISI, "Sayisi", HZ_TAG('S','A','Y',' '), "chp"}, /* Sayisi */
    {HZ_LANGUAGE_SICILIAN, "Sicilian", HZ_TAG('S','C','N',' '), "scn"}, /* Sicilian */
    {HZ_LANGUAGE_SCOTS, "Scots", HZ_TAG('S','C','O',' '), "sco"}, /* Scots */
    {HZ_LANGUAGE_NORTH_SLAVEY, "North Slavey", HZ_TAG('S','C','S',' '), "scs"}, /* North Slavey */
    {HZ_LANGUAGE_SEKOTA, "Sekota", HZ_TAG('S','E','K',' '), "xan"}, /* Sekota */
    {HZ_LANGUAGE_SELKUP, "Selkup", HZ_TAG('S','E','L',' '), "sel"}, /* Selkup */
    {HZ_LANGUAGE_OLD_IRISH, "Old Irish", HZ_TAG('S','G','A',' '), "sga"}, /* Old Irish */
    {HZ_LANGUAGE_SANGO, "Sango", HZ_TAG('S','G','O',' '), "sag"}, /* Sango */
    {HZ_LANGUAGE_SAMOGITIAN, "Samogitian", HZ_TAG('S','G','S',' '), "sgs"}, /* Samogitian */
    {HZ_LANGUAGE_TACHELHIT, "Tachelhit", HZ_TAG('S','H','I',' '), "shi"}, /* Tachelhit */
    {HZ_LANGUAGE_SHAN, "Shan", HZ_TAG('S','H','N',' '), "shn"}, /* Shan */
    {HZ_LANGUAGE_SIBE, "Sibe", HZ_TAG('S','I','B',' '), "sjo"}, /* Sibe */
    {HZ_LANGUAGE_SIDAMO, "Sidamo", HZ_TAG('S','I','D',' '), "sid"}, /* Sidamo */
    {HZ_LANGUAGE_SILTE_GURAGE, "Silte Gurage", HZ_TAG('S','I','G',' '), "xst:stv:wle"}, /* Silte Gurage */
    {HZ_LANGUAGE_SKOLT_SAMI, "Skolt Sami", HZ_TAG('S','K','S',' '), "sms"}, /* Skolt Sami */
    {HZ_LANGUAGE_SLOVAK, "Slovak", HZ_TAG('S','K','Y',' '), "slk"}, /* Slovak */
    {HZ_LANGUAGE_SLAVEY, "Slavey", HZ_TAG('S','L','A',' '), "den:scs:xsl"}, /* Slavey */
    {HZ_LANGUAGE_SLOVENIAN, "Slovenian", HZ_TAG('S','L','V',' '), "slv"}, /* Slovenian */
    {HZ_LANGUAGE_SOMALI, "Somali", HZ_TAG('S','M','L',' '), "som"}, /* Somali */
    {HZ_LANGUAGE_SAMOAN, "Samoan", HZ_TAG('S','M','O',' '), "smo"}, /* Samoan */
    {HZ_LANGUAGE_SENA, "Sena", HZ_TAG('S','N','A',' '), "seh"}, /* Sena */
    {HZ_LANGUAGE_SHONA, "Shona", HZ_TAG('S','N','A','0'), "sna"}, /* Shona */
    {HZ_LANGUAGE_SINDHI, "Sindhi", HZ_TAG('S','N','D',' '), "snd"}, /* Sindhi */
    {HZ_LANGUAGE_SINHALA_SINHALESE, "Sinhala (Sinhalese)", HZ_TAG('S','N','H',' '), "sin"}, /* Sinhala (Sinhalese) */
    {HZ_LANGUAGE_SONINKE, "Soninke", HZ_TAG('S','N','K',' '), "snk"}, /* Soninke */
    {HZ_LANGUAGE_SODO_GURAGE, "Sodo Gurage", HZ_TAG('S','O','G',' '), "gru"}, /* Sodo Gurage */
    {HZ_LANGUAGE_SONGE, "Songe", HZ_TAG('S','O','P',' '), "sop"}, /* Songe */
    {HZ_LANGUAGE_SOUTHERN_SOTHO, "Southern Sotho", HZ_TAG('S','O','T',' '), "sot"}, /* Southern Sotho */
    {HZ_LANGUAGE_ALBANIAN, "Albanian", HZ_TAG('S','Q','I',' '), "sqi"}, /* Albanian */
    {HZ_LANGUAGE_SERBIAN, "Serbian", HZ_TAG('S','R','B',' '), "cnr:srp"}, /* Serbian */
    {HZ_LANGUAGE_SARDINIAN, "Sardinian", HZ_TAG('S','R','D',' '), "srd"}, /* Sardinian */
    {HZ_LANGUAGE_SARAIKI, "Saraiki", HZ_TAG('S','R','K',' '), "skr"}, /* Saraiki */
    {HZ_LANGUAGE_SERER, "Serer", HZ_TAG('S','R','R',' '), "srr"}, /* Serer */
    {HZ_LANGUAGE_SOUTH_SLAVEY, "South Slavey", HZ_TAG('S','S','L',' '), "xsl"}, /* South Slavey */
    {HZ_LANGUAGE_SOUTHERN_SAMI, "Southern Sami", HZ_TAG('S','S','M',' '), "sma"}, /* Southern Sami */
    {HZ_LANGUAGE_SATERLAND_FRISIAN, "Saterland Frisian", HZ_TAG('S','T','Q',' '), "stq"}, /* Saterland Frisian */
    {HZ_LANGUAGE_SUKUMA, "Sukuma", HZ_TAG('S','U','K',' '), "suk"}, /* Sukuma */
    {HZ_LANGUAGE_SUNDANESE, "Sundanese", HZ_TAG('S','U','N',' '), "sun"}, /* Sundanese */
    {HZ_LANGUAGE_SURI, "Suri", HZ_TAG('S','U','R',' '), "suq"}, /* Suri */
    {HZ_LANGUAGE_SVAN, "Svan", HZ_TAG('S','V','A',' '), "sva"}, /* Svan */
    {HZ_LANGUAGE_SWEDISH, "Swedish", HZ_TAG('S','V','E',' '), "swe"}, /* Swedish */
    {HZ_LANGUAGE_SWADAYA_ARAMAIC, "Swadaya Aramaic", HZ_TAG('S','W','A',' '), "aii"}, /* Swadaya Aramaic */
    {HZ_LANGUAGE_SWAHILI, "Swahili", HZ_TAG('S','W','K',' '), "swa"}, /* Swahili */
    {HZ_LANGUAGE_SWATI, "Swati", HZ_TAG('S','W','Z',' '), "ssw"}, /* Swati */
    {HZ_LANGUAGE_SUTU, "Sutu", HZ_TAG('S','X','T',' '), "ngo:xnj:xnq"}, /* Sutu */
    {HZ_LANGUAGE_UPPER_SAXON, "Upper Saxon", HZ_TAG('S','X','U',' '), "sxu"}, /* Upper Saxon */
    {HZ_LANGUAGE_SYLHETI, "Sylheti", HZ_TAG('S','Y','L',' '), "syl"}, /* Sylheti */
    {HZ_LANGUAGE_SYRIAC, "Syriac", HZ_TAG('S','Y','R',' '), "aii:amw:cld:syc:syr:tru"}, /* Syriac */
    {HZ_LANGUAGE_SYRIAC_ESTRANGELA_SCRIPT_VARIANT_EQUIVALENT_TO_ISO_15924_SYRE, "Syriac, Estrangela script-variant (equivalent to ISO 15924 'Syre')", HZ_TAG('S','Y','R','E'), "syc:syr"}, /* Syriac, Estrangela script-variant (equivalent to ISO 15924 'Syre') */
    {HZ_LANGUAGE_SYRIAC_WESTERN_SCRIPT_VARIANT_EQUIVALENT_TO_ISO_15924_SYRJ, "Syriac, Western script-variant (equivalent to ISO 15924 'Syrj')", HZ_TAG('S','Y','R','J'), "syc:syr"}, /* Syriac, Western script-variant (equivalent to ISO 15924 'Syrj') */
    {HZ_LANGUAGE_SYRIAC_EASTERN_SCRIPT_VARIANT_EQUIVALENT_TO_ISO_15924_SYRN, "Syriac, Eastern script-variant (equivalent to ISO 15924 'Syrn')", HZ_TAG('S','Y','R','N'), "syc:syr"}, /* Syriac, Eastern script-variant (equivalent to ISO 15924 'Syrn') */
    {HZ_LANGUAGE_SILESIAN, "Silesian", HZ_TAG('S','Z','L',' '), "szl"}, /* Silesian */
    {HZ_LANGUAGE_TABASARAN, "Tabasaran", HZ_TAG('T','A','B',' '), "tab"}, /* Tabasaran */
    {HZ_LANGUAGE_TAJIKI, "Tajiki", HZ_TAG('T','A','J',' '), "tgk"}, /* Tajiki */
    {HZ_LANGUAGE_TAMIL, "Tamil", HZ_TAG('T','A','M',' '), "tam"}, /* Tamil */
    {HZ_LANGUAGE_TATAR, "Tatar", HZ_TAG('T','A','T',' '), "tat"}, /* Tatar */
    {HZ_LANGUAGE_TH_CREE, "TH-Cree", HZ_TAG('T','C','R',' '), "cwd"}, /* TH-Cree */
    {HZ_LANGUAGE_DEHONG_DAI, "Dehong Dai", HZ_TAG('T','D','D',' '), "tdd"}, /* Dehong Dai */
    {HZ_LANGUAGE_TELUGU, "Telugu", HZ_TAG('T','E','L',' '), "tel"}, /* Telugu */
    {HZ_LANGUAGE_TETUM, "Tetum", HZ_TAG('T','E','T',' '), "tet"}, /* Tetum */
    {HZ_LANGUAGE_TAGALOG, "Tagalog", HZ_TAG('T','G','L',' '), "tgl"}, /* Tagalog */
    {HZ_LANGUAGE_TONGAN, "Tongan", HZ_TAG('T','G','N',' '), "ton"}, /* Tongan */
    {HZ_LANGUAGE_TIGRE, "Tigre", HZ_TAG('T','G','R',' '), "tig"}, /* Tigre */
    {HZ_LANGUAGE_TIGRINYA, "Tigrinya", HZ_TAG('T','G','Y',' '), "tir"}, /* Tigrinya */
    {HZ_LANGUAGE_THAI, "Thai", HZ_TAG('T','H','A',' '), "tha"}, /* Thai */
    {HZ_LANGUAGE_TAHITIAN, "Tahitian", HZ_TAG('T','H','T',' '), "tah"}, /* Tahitian */
    {HZ_LANGUAGE_TIBETAN, "Tibetan", HZ_TAG('T','I','B',' '), "bod"}, /* Tibetan */
    {HZ_LANGUAGE_TIV, "Tiv", HZ_TAG('T','I','V',' '), "tiv"}, /* Tiv */
    {HZ_LANGUAGE_TURKMEN, "Turkmen", HZ_TAG('T','K','M',' '), "tuk"}, /* Turkmen */
    {HZ_LANGUAGE_TAMASHEK, "Tamashek", HZ_TAG('T','M','H',' '), "taq:thv:thz:tmh:ttq"}, /* Tamashek */
    {HZ_LANGUAGE_TEMNE, "Temne", HZ_TAG('T','M','N',' '), "tem"}, /* Temne */
    {HZ_LANGUAGE_TSWANA, "Tswana", HZ_TAG('T','N','A',' '), "tsn"}, /* Tswana */
    {HZ_LANGUAGE_TUNDRA_ENETS, "Tundra Enets", HZ_TAG('T','N','E',' '), "enh"}, /* Tundra Enets */
    {HZ_LANGUAGE_TONGA, "Tonga", HZ_TAG('T','N','G',' '), "toi"}, /* Tonga */
    {HZ_LANGUAGE_TODO, "Todo", HZ_TAG('T','O','D',' '), "xal"}, /* Todo */
    {HZ_LANGUAGE_TOMA, "Toma", HZ_TAG('T','O','D','0'), "tod"}, /* Toma */
    {HZ_LANGUAGE_TOK_PISIN, "Tok Pisin", HZ_TAG('T','P','I',' '), "tpi"}, /* Tok Pisin */
    {HZ_LANGUAGE_TURKISH, "Turkish", HZ_TAG('T','R','K',' '), "tur"}, /* Turkish */
    {HZ_LANGUAGE_TSONGA, "Tsonga", HZ_TAG('T','S','G',' '), "tso"}, /* Tsonga */
    {HZ_LANGUAGE_TSHANGLA, "Tshangla", HZ_TAG('T','S','J',' '), "tsj"}, /* Tshangla */
    {HZ_LANGUAGE_TUROYO_ARAMAIC, "Turoyo Aramaic", HZ_TAG('T','U','A',' '), "tru"}, /* Turoyo Aramaic */
    {HZ_LANGUAGE_TUMBUKA, "Tumbuka", HZ_TAG('T','U','L',' '), "tcy"}, /* Tumbuka */
    {HZ_LANGUAGE_TULU, "Tulu", HZ_TAG('T','U','M',' '), "tum"}, /* Tulu */
    {HZ_LANGUAGE_TUVIN, "Tuvin", HZ_TAG('T','U','V',' '), "tyv"}, /* Tuvin */
    {HZ_LANGUAGE_TUVALU, "Tuvalu", HZ_TAG('T','V','L',' '), "tvl"}, /* Tuvalu */
    {HZ_LANGUAGE_TWI, "Twi", HZ_TAG('T','W','I',' '), "twi"}, /* Twi */
    {HZ_LANGUAGE_TAY, "Tay", HZ_TAG('T','Y','Z',' '), "tyz"}, /* Tay */
    {HZ_LANGUAGE_TAMAZIGHT, "Tamazight", HZ_TAG('T','Z','M',' '), "tzm"}, /* Tamazight */
    {HZ_LANGUAGE_TZOTZIL, "Tzotzil", HZ_TAG('T','Z','O',' '), "tzo"}, /* Tzotzil */
    {HZ_LANGUAGE_UDMURT, "Udmurt", HZ_TAG('U','D','M',' '), "udm"}, /* Udmurt */
    {HZ_LANGUAGE_UKRAINIAN, "Ukrainian", HZ_TAG('U','K','R',' '), "ukr"}, /* Ukrainian */
    {HZ_LANGUAGE_UMBUNDU, "Umbundu", HZ_TAG('U','M','B',' '), "umb"}, /* Umbundu */
    {HZ_LANGUAGE_URDU, "Urdu", HZ_TAG('U','R','D',' '), "urd"}, /* Urdu */
    {HZ_LANGUAGE_UPPER_SORBIAN, "Upper Sorbian", HZ_TAG('U','S','B',' '), "hsb"}, /* Upper Sorbian */
    {HZ_LANGUAGE_UYGHUR, "Uyghur", HZ_TAG('U','Y','G',' '), "uig"}, /* Uyghur */
    {HZ_LANGUAGE_UZBEK, "Uzbek", HZ_TAG('U','Z','B',' '), "uzb"}, /* Uzbek */
    {HZ_LANGUAGE_VENETIAN, "Venetian", HZ_TAG('V','E','C',' '), "vec"}, /* Venetian */
    {HZ_LANGUAGE_VENDA, "Venda", HZ_TAG('V','E','N',' '), "ven"}, /* Venda */
    {HZ_LANGUAGE_VIETNAMESE, "Vietnamese", HZ_TAG('V','I','T',' '), "vie"}, /* Vietnamese */
    {HZ_LANGUAGE_VOLAPUK, "Volapuk", HZ_TAG('V','O','L',' '), "vol"}, /* Volapuk */
    {HZ_LANGUAGE_VORO, "Voro", HZ_TAG('V','R','O',' '), "vro"}, /* Voro */
    {HZ_LANGUAGE_WA, "Wa", HZ_TAG('W','A',' ',' '), "wbm"}, /* Wa */
    {HZ_LANGUAGE_WAGDI, "Wagdi", HZ_TAG('W','A','G',' '), "wbr"}, /* Wagdi */
    {HZ_LANGUAGE_WARAY_WARAY, "Waray-Waray", HZ_TAG('W','A','R',' '), "war"}, /* Waray-Waray */
    {HZ_LANGUAGE_WEST_CREE, "West-Cree", HZ_TAG('W','C','R',' '), "crk"}, /* West-Cree */
    {HZ_LANGUAGE_WELSH, "Welsh", HZ_TAG('W','E','L',' '), "cym"}, /* Welsh */
    {HZ_LANGUAGE_WOLOF, "Wolof", HZ_TAG('W','L','F',' '), "wol"}, /* Wolof */
    {HZ_LANGUAGE_WALLOON, "Walloon", HZ_TAG('W','L','N',' '), "wln"}, /* Walloon */
    {HZ_LANGUAGE_MEWATI, "Mewati", HZ_TAG('W','T','M',' '), "wtm"}, /* Mewati */
    {HZ_LANGUAGE_LU, "Lu", HZ_TAG('X','B','D',' '), "khb"}, /* Lu */
    {HZ_LANGUAGE_XHOSA, "Xhosa", HZ_TAG('X','H','S',' '), "xho"}, /* Xhosa */
    {HZ_LANGUAGE_MINJANGBAL, "Minjangbal", HZ_TAG('X','J','B',' '), "xjb"}, /* Minjangbal */
    {HZ_LANGUAGE_KHENGKHA, "Khengkha", HZ_TAG('X','K','F',' '), "xkf"}, /* Khengkha */
    {HZ_LANGUAGE_SOGA, "Soga", HZ_TAG('X','O','G',' '), "xog"}, /* Soga */
    {HZ_LANGUAGE_KPELLE_LIBERIA, "Kpelle (Liberia)", HZ_TAG('X','P','E',' '), "xpe"}, /* Kpelle (Liberia) */
    {HZ_LANGUAGE_SAKHA, "Sakha", HZ_TAG('Y','A','K',' '), "sah"}, /* Sakha */
    {HZ_LANGUAGE_YAO, "Yao", HZ_TAG('Y','A','O',' '), "yao"}, /* Yao */
    {HZ_LANGUAGE_YAPESE, "Yapese", HZ_TAG('Y','A','P',' '), "yap"}, /* Yapese */
    {HZ_LANGUAGE_YORUBA, "Yoruba", HZ_TAG('Y','B','A',' '), "yor"}, /* Yoruba */
    {HZ_LANGUAGE_Y_CREE, "Y-Cree", HZ_TAG('Y','C','R',' '), "crj:crk:crl"}, /* Y-Cree */
    {HZ_LANGUAGE_YI_CLASSIC, "Yi Classic", HZ_TAG('Y','I','C',' '), NULL}, /* Yi Classic */
    {HZ_LANGUAGE_YI_MODERN, "Yi Modern", HZ_TAG('Y','I','M',' '), "iii"}, /* Yi Modern */
    {HZ_LANGUAGE_ZEALANDIC, "Zealandic", HZ_TAG('Z','E','A',' '), "zea"}, /* Zealandic */
    {HZ_LANGUAGE_STANDARD_MOROCCAN_TAMAZIGHT, "Standard Moroccan Tamazight", HZ_TAG('Z','G','H',' '), "zgh"}, /* Standard Moroccan Tamazight */
    {HZ_LANGUAGE_ZHUANG, "Zhuang", HZ_TAG('Z','H','A',' '), "zha"}, /* Zhuang */
    {HZ_LANGUAGE_CHINESE_TRADITIONAL_HONG_KONG_SAR, "Chinese, Traditional, Hong Kong SAR", HZ_TAG('Z','H','H',' '), "zho"}, /* Chinese, Traditional, Hong Kong SAR */
    {HZ_LANGUAGE_CHINESE_PHONETIC, "Chinese, Phonetic", HZ_TAG('Z','H','P',' '), "zho"}, /* Chinese, Phonetic */
    {HZ_LANGUAGE_CHINESE_SIMPLIFIED, "Chinese, Simplified", HZ_TAG('Z','H','S',' '), "zho"}, /* Chinese, Simplified */
    {HZ_LANGUAGE_CHINESE_TRADITIONAL, "Chinese, Traditional", HZ_TAG('Z','H','T',' '), "zho"}, /* Chinese, Traditional */
    {HZ_LANGUAGE_CHINESE_TRADITIONAL_MACAO_SAR, "Chinese, Traditional, Macao SAR", HZ_TAG('Z','H','T','M'), "zho"}, /* Chinese, Traditional, Macao SAR */
    {HZ_LANGUAGE_ZANDE, "Zande", HZ_TAG('Z','N','D',' '), "zne"}, /* Zande */
    {HZ_LANGUAGE_ZULU, "Zulu", HZ_TAG('Z','U','L',' '), "zul"}, /* Zulu */
    {HZ_LANGUAGE_ZAZAKI, "Zazaki", HZ_TAG('Z','Z','A',' '), "zza"}, /* Zazaki */
};

#endif /* HZ_OT_LANGUAGE_LIST_H */
