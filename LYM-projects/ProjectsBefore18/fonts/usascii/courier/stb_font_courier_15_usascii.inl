// Font generated by stb_font_inl_generator.c (4/1 bpp)
//
// Following instructions show how to use the only included font, whatever it is, in
// a generic way so you can replace it with any other font by changing the include.
// To use multiple fonts, replace STB_SOMEFONT_* below with STB_FONT_courier_15_usascii_*,
// and separately install each font. Note that the CREATE function call has a
// totally different name; it's just 'stb_font_courier_15_usascii'.
//
/* // Example usage:

static stb_fontchar fontdata[STB_SOMEFONT_NUM_CHARS];

static void init(void)
{
    // optionally replace both STB_SOMEFONT_BITMAP_HEIGHT with STB_SOMEFONT_BITMAP_HEIGHT_POW2
    static unsigned char fontpixels[STB_SOMEFONT_BITMAP_HEIGHT][STB_SOMEFONT_BITMAP_WIDTH];
    STB_SOMEFONT_CREATE(fontdata, fontpixels, STB_SOMEFONT_BITMAP_HEIGHT);
    ... create texture ...
    // for best results rendering 1:1 pixels texels, use nearest-neighbor sampling
    // if allowed to scale up, use bilerp
}

// This function positions characters on integer coordinates, and assumes 1:1 texels to pixels
// Appropriate if nearest-neighbor sampling is used
static void draw_string_integer(int x, int y, char *str) // draw with top-left point x,y
{
    ... use texture ...
    ... turn on alpha blending and gamma-correct alpha blending ...
    glBegin(GL_QUADS);
    while (*str) {
        int char_codepoint = *str++;
        stb_fontchar *cd = &fontdata[char_codepoint - STB_SOMEFONT_FIRST_CHAR];
        glTexCoord2f(cd->s0, cd->t0); glVertex2i(x + cd->x0, y + cd->y0);
        glTexCoord2f(cd->s1, cd->t0); glVertex2i(x + cd->x1, y + cd->y0);
        glTexCoord2f(cd->s1, cd->t1); glVertex2i(x + cd->x1, y + cd->y1);
        glTexCoord2f(cd->s0, cd->t1); glVertex2i(x + cd->x0, y + cd->y1);
        // if bilerping, in D3D9 you'll need a half-pixel offset here for 1:1 to behave correct
        x += cd->advance_int;
    }
    glEnd();
}

// This function positions characters on float coordinates, and doesn't require 1:1 texels to pixels
// Appropriate if bilinear filtering is used
static void draw_string_float(float x, float y, char *str) // draw with top-left point x,y
{
    ... use texture ...
    ... turn on alpha blending and gamma-correct alpha blending ...
    glBegin(GL_QUADS);
    while (*str) {
        int char_codepoint = *str++;
        stb_fontchar *cd = &fontdata[char_codepoint - STB_SOMEFONT_FIRST_CHAR];
        glTexCoord2f(cd->s0f, cd->t0f); glVertex2f(x + cd->x0f, y + cd->y0f);
        glTexCoord2f(cd->s1f, cd->t0f); glVertex2f(x + cd->x1f, y + cd->y0f);
        glTexCoord2f(cd->s1f, cd->t1f); glVertex2f(x + cd->x1f, y + cd->y1f);
        glTexCoord2f(cd->s0f, cd->t1f); glVertex2f(x + cd->x0f, y + cd->y1f);
        // if bilerping, in D3D9 you'll need a half-pixel offset here for 1:1 to behave correct
        x += cd->advance;
    }
    glEnd();
}
*/

#ifndef STB_FONTCHAR__TYPEDEF
#define STB_FONTCHAR__TYPEDEF
typedef struct
{
    // coordinates if using integer positioning
    float s0,t0,s1,t1;
    signed short x0,y0,x1,y1;
    int   advance_int;
    // coordinates if using floating positioning
    float s0f,t0f,s1f,t1f;
    float x0f,y0f,x1f,y1f;
    float advance;
} stb_fontchar;
#endif

#define STB_FONT_courier_15_usascii_BITMAP_WIDTH         128
#define STB_FONT_courier_15_usascii_BITMAP_HEIGHT         60
#define STB_FONT_courier_15_usascii_BITMAP_HEIGHT_POW2    64

#define STB_FONT_courier_15_usascii_FIRST_CHAR            32
#define STB_FONT_courier_15_usascii_NUM_CHARS             95

#define STB_FONT_courier_15_usascii_LINE_SPACING           7

static unsigned int stb__courier_15_usascii_pixels[]={
    0x04c00018,0x25300062,0x08826003,0x1008818c,0x4ccc0440,0x08802600,
    0x10266662,0x0dcb0077,0x99315c33,0x04872a17,0xb2401724,0x5170ee15,
    0x2a23abb8,0x1d564419,0x36e0aab6,0xac92a01a,0x22e62baa,0x98e001b8,
    0x505c90e1,0x43880dc3,0x2e1c7303,0xb8e0cdc3,0x86a01450,0x3800ae03,
    0x1c1c1702,0x2387332a,0x20389219,0x41c1221a,0x50e0c80b,0x46f36e25,
    0x55c4a619,0x13706401,0x72cb80b0,0x07071240,0x90cc714c,0x0d40bba8,
    0x0e20e21a,0x512a871a,0x710e1577,0x7541aa88,0x712556c2,0x711b8797,
    0x8298e209,0x95c5300a,0x0dc05919,0x05c0d835,0x2a1c5838,0x4382ea61,
    0x80135338,0x3a25300b,0x12624531,0x80e12054,0x88cc3229,0x0d453063,
    0x42880e16,0xb79301c6,0x41982a15,0x80e00b9a,0x20e27123,0x44190704,
    0x2638a603,0x4bacb921,0x0171a0d4,0x500e3448,0x20e38871,0x0c6604a2,
    0x58e2482a,0x97038254,0x8e298077,0x206a1219,0x0090e61a,0xd870dc53,
    0x2ee0e2a4,0x50ba982c,0x5ecc777b,0x05de443b,0x85b377b1,0x315b3019,
    0x26386601,0x2a0660e1,0x001a8321,0x08872449,0x0000814a,0x80200401,
    0x266e0000,0x05c9102b,0x54014c11,0x80000a0b,0x00002ae2,0x00000000,
    0x2a000000,0x000000aa,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00310620,0x98044022,0x25930000,
    0x644dd42c,0x80110061,0x99006018,0xc8ca9654,0x5059503c,0xb31d4dc5,
    0xa8775700,0x501c83ab,0x88dc1977,0x2d88dc4c,0x04980322,0x024072a2,
    0x0cc890a6,0x3a8e4493,0x70e18b74,0x382a2880,0x2038006a,0x550e2443,
    0x401c00a8,0x4ccec819,0x03214c90,0x406e0549,0x75383662,0x442e2883,
    0x40cc89c2,0x1c1ccec9,0x42a87122,0x3382cb9a,0x12033079,0x06424a60,
    0x7038838e,0x0bc82eaa,0x43dcb873,0x203312e4,0x44b82e03,0x4a85b10b,
    0x40ae702e,0x32009019,0x8e24c829,0x9cbe6703,0x8e0e4173,0x64c499c8,
    0x380cc49b,0xa8496540,0x105429ab,0x2601cc85,0x16600901,0x41c2ca9c,
    0xc99cb72a,0x2170320d,0x1cc0a61a,0x2001c066,0x82a0cc04,0x3024e428,
    0x14c00e03,0x3214c038,0x900dc1d0,0x0a8a6387,0x80cc0c88,0x91024003,
    0x4e144150,0x26033038,0x40903bab,0x0dbca803,0xa8219106,0x5c0bbb9d,
    0x372e23bb,0x4b3772a0,0x200ccec9,0x554c1aa9,0x1b926c40,0x951dc96a,
    0x406059bb,0xaa982aaa,0xb3000801,0x40004059,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x04401300,
    0x0b332e00,0x73033773,0x4664c317,0x59751cc8,0x3219bb98,0x3264cccc,
    0x1e5c0bcc,0xb8f22797,0x65c7910c,0x540b3320,0x903bac8a,0x200ee4c7,
    0x8d11cc0b,0x7071aa29,0xb8e23524,0x5438cce3,0x48381911,0x1a8350d4,
    0xdd00e0aa,0x22e05415,0x05c01983,0x85090212,0x054e0383,0x50e33386,
    0x0e21c383,0x88c81c52,0x2a000703,0x0cc1ce21,0x403882e0,0x03838028,
    0x3080dc37,0x2e235023,0x10171441,0x02640177,0x48910007,0xb810660e,
    0x0aaa8a20,0x07070051,0x0197101b,0x3b36a033,0x55024900,0x00e03c80,
    0x8a624455,0x2e1c1983,0x41931240,0x18383805,0x80cc1c04,0x22a0911a,
    0xa8066029,0x0a8702a9,0x65c245ba,0x20cc0ecc,0x0e0641c4,0x30d42819,
    0x1c458103,0x261a80cc,0x19807703,0x1c05c730,0x0073082a,0x5930660e,
    0x75b91037,0x03959909,0x8f907579,0x640cbbbc,0x0eecc3dd,0x64077077,
    0x91f4c4dd,0x6667643d,0xcccce80d,0xc9876442,0x80041cdd,0x00400801,
    0x00000802,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x64732600,0x6647950c,0x43999932,0x2e4c88cc,0x665c1ccc,0x99931ccc,
    0x2f32e599,0x866664c1,0x540bccc9,0x2a39910c,0x332a2c84,0x00597102,
    0x86e1a844,0x81983828,0x4c70a21a,0x1a8a21c3,0xa8aa289c,0x2e1a82e0,
    0x590e2641,0x504b8254,0xaec07701,0x2e70191a,0x35033072,0x4170e144,
    0x42350c43,0x150b8511,0x438350a6,0x4970e1ab,0x81a9983c,0x90591029,
    0x9c0366a9,0x5033074c,0x43855335,0x5405ce44,0x2614449a,0x4d542442,
    0x21c93839,0x3839999b,0x40c56a0e,0x6d4ee1c9,0x3b8b80d9,0x57503307,
    0xcbac8575,0x54065640,0x221444ab,0x55d40642,0x7389c2ba,0x8e0e9938,
    0x05dc5443,0x2a330da8,0x262ae121,0x1a81983a,0x138070a2,0x511846a0,
    0x2286a0a8,0x5c705c1a,0x872a9c38,0x262baaba,0x03930772,0x4b8150d4,
    0x2a0660f2,0x38070a21,0x144e0d40,0xa8e1c491,0x8790e1c1,0x2241c383,
    0x5911ccbb,0x361dd980,0x262c92e1,0xbb11cddc,0x3ae1bb11,0x83ceb84c,
    0x95cccdd9,0xccc8999b,0x99bb34cc,0xa8674c59,0x3eb8fae4,0x21f5c7d7,
    0x00000280,0x00000000,0x00000000,0x00000000,0x00000000,0x10379710,
    0x77303377,0x88177101,0xba83730c,0x4c3930ab,0x65c7931c,0x21977711,
    0x5c2b88b9,0x97370ee3,0xc9bbaa85,0xc9801982,0x4c710e00,0x54370dc5,
    0x441306e2,0x4e1a8a62,0x144150e0,0x4f222413,0x98191c40,0x52ab02e5,
    0x32201983,0x38955102,0x4ccce408,0x4130e0e3,0x206b2228,0x38e0e773,
    0x100e2120,0x44130197,0x307199c2,0x5c077103,0x401c488b,0x0e1aaaac,
    0x4144130e,0x5dca0ca8,0x640e3828,0x4c801880,0x4e144130,0x5e554719,
    0x87012e2a,0x07060544,0x8661c438,0x4c710e2a,0xa881ba9b,0x8860b80a,
    0x12232201,0x99c28826,0x22033071,0x9557703c,0x42babb89,0x222cabb8,
    0x97903bbb,0x65d5dcb7,0x25c0d340,0xa9dddecc,0x27443bdc,0xc93621d9,
    0x5d2636a5,0x32600cc2,0x00100082,0x40080001,0x00000008,0x00000000,
    0x26001100,0x00000000,0x00000000,0x00000000,0x00000000,0x45f30000,
    0x26088888,0x5710faaf,0x22550504,0x77308888,0x00177777,0x00000000,
    0x81c00000,0x2e26b83b,0x3e0bbbbb,0x5e4ca799,0x2edd0ea1,0x0003cccc,
    0x00000000,0x2a000000,0x266a28b8,0x222216c2,0x3447c088,0x00083010,
    0x00000000,0x00000000,0x4c2fee20,0x5c4582e3,0x8a0bbbbb,0x00000002,
    0x00000000,0x00000000,0x01011cdc,0x00000000,0x00000000,0x00000000,
    0x00318000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,
};

static signed short stb__courier_15_usascii_x[95]={ 0,3,1,1,1,1,1,3,3,1,0,0,1,1,
2,1,1,1,1,1,1,1,1,1,1,1,2,2,0,0,0,0,1,0,0,0,1,0,1,0,0,1,1,0,
0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,2,0,0,1,0,0,1,0,0,1,
1,1,1,0,0,0,0,0,1,1,0,0,0,0,0,0,1,0,3,0,1, };
static signed short stb__courier_15_usascii_y[95]={ 11,2,3,2,2,2,4,2,2,2,2,3,9,6,
9,2,2,2,2,2,2,2,2,2,2,2,5,5,3,5,3,3,2,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,14,2,5,2,5,2,5,2,5,2,2,
2,2,2,5,5,5,5,5,5,5,3,5,5,5,5,5,5,2,2,2,6, };
static unsigned short stb__courier_15_usascii_w[95]={ 0,2,6,6,6,6,6,2,4,4,5,8,4,6,
3,6,6,6,6,6,6,6,6,6,6,6,3,3,7,8,7,5,6,8,8,8,7,7,7,8,8,6,7,8,
8,8,8,7,7,8,8,6,8,8,8,8,8,8,6,3,5,3,6,9,4,8,8,7,8,7,7,8,8,6,
5,7,6,8,8,8,8,8,7,6,8,8,8,8,8,8,6,3,2,3,6, };
static unsigned short stb__courier_15_usascii_h[95]={ 0,10,4,10,11,10,8,5,11,11,6,8,4,2,
3,11,10,9,9,10,9,10,10,10,10,10,7,8,8,4,8,9,10,8,8,9,8,8,8,9,8,8,9,8,
8,8,8,9,8,10,8,9,8,9,8,8,8,8,8,11,11,11,5,1,3,7,10,7,10,7,9,9,9,9,
12,9,9,6,6,7,9,9,6,7,9,7,6,7,6,9,6,11,11,11,3, };
static unsigned short stb__courier_15_usascii_s[95]={ 126,56,45,59,18,73,110,125,47,29,18,
1,31,68,64,34,66,15,1,80,8,87,94,110,17,24,124,123,10,36,117,
57,1,101,74,39,59,51,43,30,26,19,22,1,115,92,83,115,35,117,79,
63,70,48,88,10,106,97,67,52,41,25,24,75,59,18,8,27,101,35,38,
55,64,31,1,73,81,119,110,43,106,46,93,61,88,52,77,68,101,97,86,
14,11,7,52, };
static unsigned short stb__courier_15_usascii_t[95]={ 1,1,52,1,1,1,35,35,1,1,52,
44,52,52,52,1,1,25,25,1,25,1,1,1,14,14,25,14,44,52,35,
25,14,35,35,25,35,35,35,25,35,35,25,35,25,35,35,14,35,1,25,
25,25,25,25,35,25,25,35,1,1,1,52,52,52,44,14,44,1,44,14,
14,14,14,1,14,14,44,44,44,14,14,44,44,14,44,44,44,44,14,44,
1,1,1,52, };
static unsigned short stb__courier_15_usascii_a[95]={ 127,127,127,127,127,127,127,127,
127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
127,127,127,127,127,127,127, };

// Call this function with
//    font: NULL or array length
//    data: NULL or specified size
//    height: STB_FONT_courier_15_usascii_BITMAP_HEIGHT or STB_FONT_courier_15_usascii_BITMAP_HEIGHT_POW2
//    return value: spacing between lines
static void stb_font_courier_15_usascii(stb_fontchar font[STB_FONT_courier_15_usascii_NUM_CHARS],
                unsigned char data[STB_FONT_courier_15_usascii_BITMAP_HEIGHT][STB_FONT_courier_15_usascii_BITMAP_WIDTH],
                int height)
{
    int i,j;
    if (data != 0) {
        unsigned int *bits = stb__courier_15_usascii_pixels;
        unsigned int bitpack = *bits++, numbits = 32;
        for (i=0; i < STB_FONT_courier_15_usascii_BITMAP_WIDTH*height; ++i)
            data[0][i] = 0;  // zero entire bitmap
        for (j=1; j < STB_FONT_courier_15_usascii_BITMAP_HEIGHT-1; ++j) {
            for (i=1; i < STB_FONT_courier_15_usascii_BITMAP_WIDTH-1; ++i) {
                unsigned int value;
                if (numbits==0) bitpack = *bits++, numbits=32;
                value = bitpack & 1;
                bitpack >>= 1, --numbits;
                if (value) {
                    if (numbits < 3) bitpack = *bits++, numbits = 32;
                    data[j][i] = (bitpack & 7) * 0x20 + 0x1f;
                    bitpack >>= 3, numbits -= 3;
                } else {
                    data[j][i] = 0;
                }
            }
        }
    }

    // build font description
    if (font != 0) {
        float recip_width = 1.0f / STB_FONT_courier_15_usascii_BITMAP_WIDTH;
        float recip_height = 1.0f / height;
        for (i=0; i < STB_FONT_courier_15_usascii_NUM_CHARS; ++i) {
            // pad characters so they bilerp from empty space around each character
            font[i].s0 = (stb__courier_15_usascii_s[i]) * recip_width;
            font[i].t0 = (stb__courier_15_usascii_t[i]) * recip_height;
            font[i].s1 = (stb__courier_15_usascii_s[i] + stb__courier_15_usascii_w[i]) * recip_width;
            font[i].t1 = (stb__courier_15_usascii_t[i] + stb__courier_15_usascii_h[i]) * recip_height;
            font[i].x0 = stb__courier_15_usascii_x[i];
            font[i].y0 = stb__courier_15_usascii_y[i];
            font[i].x1 = stb__courier_15_usascii_x[i] + stb__courier_15_usascii_w[i];
            font[i].y1 = stb__courier_15_usascii_y[i] + stb__courier_15_usascii_h[i];
            font[i].advance_int = (stb__courier_15_usascii_a[i]+8)>>4;
            font[i].s0f = (stb__courier_15_usascii_s[i] - 0.5f) * recip_width;
            font[i].t0f = (stb__courier_15_usascii_t[i] - 0.5f) * recip_height;
            font[i].s1f = (stb__courier_15_usascii_s[i] + stb__courier_15_usascii_w[i] + 0.5f) * recip_width;
            font[i].t1f = (stb__courier_15_usascii_t[i] + stb__courier_15_usascii_h[i] + 0.5f) * recip_height;
            font[i].x0f = stb__courier_15_usascii_x[i] - 0.5f;
            font[i].y0f = stb__courier_15_usascii_y[i] - 0.5f;
            font[i].x1f = stb__courier_15_usascii_x[i] + stb__courier_15_usascii_w[i] + 0.5f;
            font[i].y1f = stb__courier_15_usascii_y[i] + stb__courier_15_usascii_h[i] + 0.5f;
            font[i].advance = stb__courier_15_usascii_a[i]/16.0f;
        }
    }
}

#ifndef STB_SOMEFONT_CREATE
#define STB_SOMEFONT_CREATE              stb_font_courier_15_usascii
#define STB_SOMEFONT_BITMAP_WIDTH        STB_FONT_courier_15_usascii_BITMAP_WIDTH
#define STB_SOMEFONT_BITMAP_HEIGHT       STB_FONT_courier_15_usascii_BITMAP_HEIGHT
#define STB_SOMEFONT_BITMAP_HEIGHT_POW2  STB_FONT_courier_15_usascii_BITMAP_HEIGHT_POW2
#define STB_SOMEFONT_FIRST_CHAR          STB_FONT_courier_15_usascii_FIRST_CHAR
#define STB_SOMEFONT_NUM_CHARS           STB_FONT_courier_15_usascii_NUM_CHARS
#define STB_SOMEFONT_LINE_SPACING        STB_FONT_courier_15_usascii_LINE_SPACING
#endif
