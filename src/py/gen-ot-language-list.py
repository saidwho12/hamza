#!/usr/bin/env python3

"""
Usage: ./gen-ot-language-list
"""

import os.path, sys
import re, unicodedata



def latin1_to_ascii(unicrap):
    """This takes a UNICODE string and replaces Latin-1 characters with
        something equivalent in 7-bit ASCII. It returns a plain ASCII string. 
        This function makes a best effort to convert Latin-1 characters into 
        ASCII equivalents. It does not just strip out the Latin-1 characters.
        All characters in the standard 7-bit ASCII range are preserved. 
        In the 8th bit range all the Latin-1 accented letters are converted 
        to unaccented equivalents. Most symbol characters are converted to 
        something meaningful. Anything not converted is deleted.
    """

    xlate={0xc0:'A', 0xc1:'A', 0xc2:'A', 0xc3:'A', 0xc4:'A', 0xc5:'A',
        0xc6:'Ae', 0xc7:'C',
        0xc8:'E', 0xc9:'E', 0xca:'E', 0xcb:'E',
        0xcc:'I', 0xcd:'I', 0xce:'I', 0xcf:'I',
        0xd0:'Th', 0xd1:'N',
        0xd2:'O', 0xd3:'O', 0xd4:'O', 0xd5:'O', 0xd6:'O', 0xd8:'O',
        0xd9:'U', 0xda:'U', 0xdb:'U', 0xdc:'U',
        0xdd:'Y', 0xde:'th', 0xdf:'ss',
        0xe0:'a', 0xe1:'a', 0xe2:'a', 0xe3:'a', 0xe4:'a', 0xe5:'a',
        0xe6:'ae', 0xe7:'c',
        0xe8:'e', 0xe9:'e', 0xea:'e', 0xeb:'e',
        0xec:'i', 0xed:'i', 0xee:'i', 0xef:'i',
        0xf0:'th', 0xf1:'n',
        0xf2:'o', 0xf3:'o', 0xf4:'o', 0xf5:'o', 0xf6:'o', 0xf8:'o',
        0xf9:'u', 0xfa:'u', 0xfb:'u', 0xfc:'u',
        0xfd:'y', 0xfe:'th', 0xff:'y',
        0xa1:'!', 0xa2:'{cent}', 0xa3:'{pound}', 0xa4:'{currency}',
        0xa5:'{yen}', 0xa6:'|', 0xa7:'{section}', 0xa8:'{umlaut}',
        0xa9:'{C}', 0xaa:'{^a}', 0xab:'<<', 0xac:'{not}',
        0xad:'-', 0xae:'{R}', 0xaf:'_', 0xb0:'{degrees}',
        0xb1:'{+/-}', 0xb2:'{^2}', 0xb3:'{^3}', 0xb4:"'",
        0xb5:'{micro}', 0xb6:'{paragraph}', 0xb7:'*', 0xb8:'{cedilla}',
        0xb9:'{^1}', 0xba:'{^o}', 0xbb:'>>', 
        0xbc:'{1/4}', 0xbd:'{1/2}', 0xbe:'{3/4}', 0xbf:'?',
        0xd7:'*', 0xf7:'/'
        }

    r = ''
    for i in unicrap:
        if xlate.has_key(ord(i)):
            r += xlate[ord(i)]
        elif ord(i) >= 0x80:
            r += i
        else:
            r += str(i)
    return r


def lang_to_enum(lang):
    result = 'HZ_LANGUAGE'

    words = re.split(r"['\s\,\(\)\-\/]", lang)
    
    # remove words which are empty (maybe there's a better way
    # to accomplish this with regex)
    for i in xrange(len(words)-1,0,-1):
        if len(words[i]) == 0:
            words.pop(i)

    for word in words:
        result += '_' + word.upper()

    print(words)

    return result

class LangItem:
    def __init__(self, langspec):
        matches = re.findall(r"<td>(.*?)(?=<\/td>)", langspec)
        self.lang = matches[0]
        self.tag = matches[1].strip("'")
        if len(matches) == 3:
            self.codes = matches[2]
            self.codes = self.codes.replace(', ', ':')
        elif len(matches) == 2:
            self.codes = None

        lang_norm = unicodedata.normalize('NFC', unicode(self.lang, encoding='utf8'))
        self.lang_ascii = latin1_to_ascii(lang_norm).encode('utf8')
        self.unilang = unicode(self.lang_ascii, encoding='utf8')

        # U+2014 is the Em Dash, replace with regular Dash
        self.unilang = self.unilang.replace(u"\u2014", u"-")
        # remove Right Single Quotation Mark U+2019
        self.unilang = self.unilang.replace(u"\u2019", u"")

    def __str__(self):
        if self.codes == None:
            return str('{\"%s\", \'%s\', NULL}' % (self.lang_ascii, self.tag))
        else:
            return str('{\"%s\", \'%s\', \"%s\"}' % (self.lang_ascii, self.tag, self.codes))

    def __repr__(self):
        return self.__str__()


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)

    with open(sys.argv[1]) as ih, open("hz-ot-language-list.h", "w+") as oh:
        lines = ih.readlines()
        lang_items = []

        for b in range(0, len(lines)/5):
            i = b*5;
            langspec = lines[i+1].strip() + lines[i+2].strip() + lines[i+3].strip()
            item = LangItem(langspec)
            lang_items.append(item)

        print(len(lang_items))

        oh.write("#ifndef HZ_OT_LANGUAGE_LIST_H\n")
        oh.write("#define HZ_OT_LANGUAGE_LIST_H\n\n")

        oh.write("#include \"hz-base.h\"\n\n")

        oh.write("typedef enum hz_language_t {\n")
        oh.write("    HZ_LANGUAGE_DFLT,\n")
        
        enum_values = []

        for item in lang_items:
            lang = item.unilang.encode('ascii')
            lang_enum = lang_to_enum(item.unilang).encode('ascii')
            if lang_enum not in enum_values:
                enum_values.append(lang_enum)
                oh.write("    %s, /* %s */ \n" % (lang_enum, lang))

        oh.write("} hz_language_t;\n\n")

        oh.write("typedef struct hz_language_map_t {\n")
        oh.write("    hz_language_t language;\n")
        oh.write("    const char *language_name;\n")
        oh.write("    hz_tag_t tag;\n")
        oh.write("    const char *codes;\n")
        oh.write("} hz_language_map_t;\n\n")

        oh.write("static const hz_language_map_t language_map_list[] = {\n")
        
        for item in lang_items:
            lang = item.unilang.encode('ascii')
            lang_enum = lang_to_enum(item.unilang).encode('ascii')
            tag = item.tag

            tag_text = "HZ_TAG_NONE"
            codes_text = "NULL"

            if len(tag) > 0:
                tag_text = "HZ_TAG('%c','%c','%c','%c')" % (tag[0],tag[1],tag[2],tag[3])

            if item.codes is not None:
                codes_text = "\"%s\"" % (item.codes)

            oh.write("    {%s, \"%s\", %s, %s}, /* %s */\n"
                % (lang_enum, lang, tag_text, codes_text, lang))
        
        oh.write("};\n\n")

        oh.write("#endif /* HZ_OT_LANGUAGE_LIST_H */\n")



if __name__ == "__main__":
    main()
