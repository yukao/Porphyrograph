// Font generated by stb_font_inl_generator.c (4/1 bpp)
//
// Following instructions show how to use the only included font, whatever it is, in
// a generic way so you can replace it with any other font by changing the include.
// To use multiple fonts, replace STB_SOMEFONT_* below with STB_FONT_arial_12_usascii_*,
// and separately install each font. Note that the CREATE function call has a
// totally different name; it's just 'stb_font_arial_12_usascii'.
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

#define STB_FONT_arial_12_usascii_BITMAP_WIDTH         128
#define STB_FONT_arial_12_usascii_BITMAP_HEIGHT         50
#define STB_FONT_arial_12_usascii_BITMAP_HEIGHT_POW2    64

#define STB_FONT_arial_12_usascii_FIRST_CHAR            32
#define STB_FONT_arial_12_usascii_NUM_CHARS             95

#define STB_FONT_arial_12_usascii_LINE_SPACING           8

static unsigned int stb__arial_12_usascii_pixels[]={
    0x920f2044,0x441971d8,0x6dcc0712,0x21e440bc,0x05c170c3,0x9302f6e2,
    0x32ea2153,0x65c35481,0x80edc121,0x251481ca,0x20742eeb,0x44390ee6,
    0x472e0321,0x31260da8,0x22953447,0x35c56dc6,0x263f53d3,0x5220e9af,
    0x26b1910c,0xe868ca84,0x98a44699,0x360e9ad8,0x1c839340,0x9d4498a2,
    0x2658c8ca,0x27f706c4,0x40e87568,0x98f4773c,0x676dcb25,0x06d6733f,
    0x92a34ed4,0xcaa89540,0x9341c81a,0xd1261c83,0x2bb37958,0xe98d1265,
    0x69836720,0x8ec77598,0x5644ee49,0x26b360d9,0x9703ba03,0x930d85db,
    0x89307dd1,0x4c390d36,0x4d193344,0x9926194b,0x17663445,0x962b3095,
    0xb0f4d31f,0x23610731,0x27af60cb,0x1d11d70a,0x76dc932c,0x77687620,
    0x3a24c390,0x96495550,0x44499264,0x43cc2e86,0x47bb6e4c,0x3ae3eee9,
    0x9b983dec,0x3a69548c,0x732c9266,0x64d4464c,0x06cd0b22,0x33449872,
    0x323e2d39,0x44689262,0x217b0686,0x263b01e9,0x40369104,0xb2cd6735,
    0x2e3d2c1d,0x26b2b261,0x64d0721d,0x44930e41,0xddcecb45,0x3a1a24c2,
    0x76407b34,0x4c7601ee,0x4c167504,0xeeb8f363,0x8fb6a5bb,0x4eeda4c5,
    0x41c9a06c,0x42b92a1c,0x30e980da,0x02202b89,0x80408000,0x02020108,
    0x2a004000,0xb53435c1,0x174c7ae3,0x3337660b,0xb0fa60ce,0x00000000,
    0x00000000,0x01101000,0x44310881,0x00cc0040,0x664c0813,0xcca80a20,
    0x06764c0c,0x43882f22,0x70929838,0x3333263d,0x0000510c,0x00000000,
    0x4d53d100,0x2aaba049,0x3aa276a0,0x689a67a0,0x0c9dc344,0x7333107c,
    0x6e5424cd,0xa80bc882,0x564c0bdc,0x033b220a,0x85c01991,0xcac99e18,
    0x350f0341,0x8d10e9a2,0x4553b868,0x41d104fc,0x644edc49,0x71667a24,
    0x74cb715b,0x22ba1fa9,0x1aa7a0fa,0x31ecc1d8,0x3a61d35f,0x201663ed,
    0x11a23ddc,0x9922770d,0x4c2e881e,0x4a8e1a24,0x9e43c472,0x23447a3b,
    0x30f4b50b,0xeb983b79,0x289e4b30,0xff92d47a,0x436eb629,0x1dc34468,
    0x307b068b,0x92a02d49,0x2950b52d,0xcfc8762b,0x22591dc1,0xc821fb9e,
    0xb815c932,0x2e930d13,0xacc78b23,0xd0dc7708,0xa9260b70,0x2ecce804,
    0x566b5097,0x76dc41f8,0x3b964770,0x21d2cc7a,0xd2cc3a69,0x4d322641,
    0x321e8764,0x398b2774,0x24c0d51a,0x898170d1,0x49b0f31c,0x8531ecda,
    0xc8ec953c,0x3ddc8762,0xb90f7766,0x9fdd905b,0x221eee41,0x3ba25fee,
    0xffb1a585,0xb1263fff,0x74912d45,0x87e64f60,0x074c3a18,0x50e9a259,
    0x00000fcb,0x00000000,0x88000010,0x6e40deed,0x6f76c42d,0x51373a2e,
    0xeda89ddf,0x00f67642,0x00000000,0x00000000,0x14000000,0x00008022,
    0x0f333220,0x33322051,0x2703222c,0x8864c1c8,0x22b87103,0x38871244,
    0x00000000,0x3a200000,0x1265c999,0x974ccf44,0x7cda24f8,0x05987e45,
    0xf1e2e4b3,0x55d11a21,0x550e6398,0x2387505c,0x40a2480b,0x11cccccb,
    0x4d26688d,0x22971a23,0x366d12ee,0x2cc3b60e,0x8f0b9598,0xf3d11a27,
    0x44f60f41,0x26e8ee0e,0x49512e59,0x3d531049,0x2a67a213,0x83a7263d,
    0x6c4b6668,0x7b3688e9,0x2cc3aea5,0x8f07cd98,0x33333e27,0x2212eb76,
    0x8a4c1e9e,0x344572cd,0x1cac9876,0xccf88598,0x20ff4c2f,0x222deef8,
    0x366d1975,0x261d1959,0x0999f305,0x2abe268f,0x81afa6ba,0x2ead03fa,
    0x11d81b4a,0x0f9af98d,0xd53442cc,0x2225f4c1,0x6ae96206,0x87563ab3,
    0x3a2d3059,0x468963c1,0xa807e668,0x21b1b04f,0x0ee950d6,0x1660ecb3,
    0xa4c991a2,0x5881a22d,0xdbacdbe6,0x4c1661d3,0x20783a65,0x81e34468,
    0xab81e8e8,0xeb412b66,0x983b24c0,0x8f446885,0x1a21d149,0x59b5c588,
    0x7cc3a1f1,0xb8b34fff,0x2344f1e5,0x25d03c68,0x726cc0e9,0x2133205b,
    0x0b307649,0x00000000,0x00000000,0x89907800,0x1e62f85b,0x24981f88,
    0x0002cc1d,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x40000000,0x93333224,0x11ccccc8,0xc9839999,0x99911ccc,0x40322019,
    0x66cc1bb9,0x22483501,0x2cc982a2,0x2f261e5c,0x2205dc40,0x4b2a20bb,
    0x267a27d8,0x2667a219,0xd533d109,0x8fd554c5,0x706a99e9,0xd33d107b,
    0x20744744,0x493242f9,0x1d31d30d,0x744ee32a,0x354f4599,0xaf99a67a,
    0x0344f5b1,0x263440d1,0x22cc7706,0x9868e80e,0x590c4b62,0x92602cd8,
    0x43aa620d,0x092a0ae9,0x5b51c8ea,0x3c1663b5,0x443555f1,0x1a22aaaf,
    0xe98d103e,0xb5984caa,0x4c074405,0x80ae720e,0x67443649,0x4fb260da,
    0x9dc5703b,0x931bbbdb,0xcccf89e0,0x8aaabe21,0x1dc0e868,0x03b2aba6,
    0x220b7379,0x260e882e,0xd2cc0574,0x551e8ee1,0x4cd2c5a2,0x392c4765,
    0x0d13c126,0x88d10344,0x22cc0e87,0x3333a23b,0xb80f440f,0x17777743,
    0x10dbdd8f,0x3625bbbd,0x0b76e2ed,0xddb87db7,0xd13c1264,0x8d103440,
    0x2cc3444c,0x1e44a8b6,0x204403d1,0x120bc998,0x00000100,0xd13c0000,
    0x7ffffc40,0x09dfff14,0xffff3095,0x93c41e89,0x3c85ffff,0x00030ae0,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x510e4d44,
    0x04391197,0x5288d470,0x705c9999,0x441c4aa1,0x36040480,0x86aaaa20,
    0x2a550419,0x8c2ee1ba,0x2eeee248,0x35f3043b,0x3ea7e61d,0x033a669b,
    0x895d3a39,0x44ee4d99,0x20b63a26,0xeb883839,0xbb88f6e3,0x47bb51bb,
    0x8eddcee7,0x2e4febbb,0x02222220,0x261b2ccf,0x6d41d9e5,0x2d91751d,
    0xab445a86,0x262274c3,0xd91009cb,0x888d1d07,0x49066408,0x0019552a,
    0xd9260000,0x0393c930,0x6d56c7d5,0x9d834c59,0x261fc40d,0xd303cedc,
    0x4cae9507,0x0223eeee,0x26200003,0x49300009,0x393c930d,0xbb033ae2,
    0x07445d51,0xe9d85da8,0x44083981,0x22130ceb,0x00000001,0x0000ccb8,
    0x24986c93,0x7014c727,0xeed83e2d,0x2e17c0ee,0xf0530d33,0x00001d40,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,
};

static signed short stb__arial_12_usascii_x[95]={ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,1,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, };
static signed short stb__arial_12_usascii_y[95]={ 9,1,1,1,0,1,1,1,1,1,1,2,7,5,
7,1,1,1,1,1,1,1,1,1,1,1,3,3,2,3,2,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,10,1,3,1,3,1,3,1,3,1,1,
1,1,1,3,3,3,3,3,3,3,1,3,3,3,3,3,3,1,1,1,4, };
static unsigned short stb__arial_12_usascii_w[95]={ 0,3,4,6,6,9,7,2,4,4,4,6,3,4,
3,3,6,4,6,6,6,6,6,6,6,6,3,3,6,6,6,6,11,9,7,8,8,7,7,8,7,2,5,8,
6,9,7,8,7,8,8,7,7,7,8,11,8,8,7,3,3,3,5,8,3,6,6,6,6,6,4,6,6,2,
3,6,2,9,6,6,6,6,4,5,3,6,6,8,6,6,6,4,2,4,6, };
static unsigned short stb__arial_12_usascii_h[95]={ 0,8,4,9,11,9,9,4,11,11,4,6,4,2,
2,9,9,8,8,9,8,9,9,8,9,9,6,8,6,4,6,8,11,8,8,9,8,8,8,9,8,8,9,8,
8,8,8,9,8,9,8,9,8,9,8,8,8,8,8,11,9,11,5,2,2,7,9,7,9,7,8,9,8,8,
11,8,8,6,6,7,9,9,6,7,9,7,6,6,6,9,6,11,11,11,3, };
static unsigned short stb__arial_12_usascii_s[95]={ 125,116,98,89,1,96,106,95,50,29,86,
55,91,110,123,121,41,1,56,55,70,69,114,31,85,10,62,77,66,79,18,
63,34,46,38,1,22,14,6,76,120,113,100,104,97,87,79,66,71,17,55,
33,47,92,31,19,10,1,115,46,106,21,73,114,110,88,62,101,48,115,110,
26,40,125,25,64,123,8,1,108,82,75,122,95,55,81,41,25,48,59,34,
16,13,8,103, };
static unsigned short stb__arial_12_usascii_t[95]={ 10,21,41,1,1,1,1,41,1,1,41,
41,41,44,41,1,13,32,32,11,32,11,1,32,11,13,41,32,41,41,41,
32,1,32,32,13,32,32,32,11,21,21,11,21,21,21,21,1,21,13,21,
13,23,11,23,23,23,23,11,1,11,1,41,41,41,32,11,32,13,32,11,
13,23,1,1,21,11,41,41,32,1,1,32,32,1,32,41,41,41,1,41,
1,1,1,41, };
static unsigned short stb__arial_12_usascii_a[95]={ 48,48,61,96,96,153,115,33,
57,57,67,100,48,57,48,48,96,96,96,96,96,96,96,96,
96,96,48,48,100,100,100,96,174,115,115,124,124,115,105,134,
124,48,86,115,96,143,124,134,115,134,124,115,105,124,115,162,
115,115,105,48,48,48,81,96,57,96,96,86,96,96,48,96,
96,38,38,86,38,143,96,96,96,96,57,86,48,96,86,124,
86,86,86,57,45,57,100, };

// Call this function with
//    font: NULL or array length
//    data: NULL or specified size
//    height: STB_FONT_arial_12_usascii_BITMAP_HEIGHT or STB_FONT_arial_12_usascii_BITMAP_HEIGHT_POW2
//    return value: spacing between lines
static void stb_font_arial_12_usascii(stb_fontchar font[STB_FONT_arial_12_usascii_NUM_CHARS],
                unsigned char data[STB_FONT_arial_12_usascii_BITMAP_HEIGHT][STB_FONT_arial_12_usascii_BITMAP_WIDTH],
                int height)
{
    int i,j;
    if (data != 0) {
        unsigned int *bits = stb__arial_12_usascii_pixels;
        unsigned int bitpack = *bits++, numbits = 32;
        for (i=0; i < STB_FONT_arial_12_usascii_BITMAP_WIDTH*height; ++i)
            data[0][i] = 0;  // zero entire bitmap
        for (j=1; j < STB_FONT_arial_12_usascii_BITMAP_HEIGHT-1; ++j) {
            for (i=1; i < STB_FONT_arial_12_usascii_BITMAP_WIDTH-1; ++i) {
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
        float recip_width = 1.0f / STB_FONT_arial_12_usascii_BITMAP_WIDTH;
        float recip_height = 1.0f / height;
        for (i=0; i < STB_FONT_arial_12_usascii_NUM_CHARS; ++i) {
            // pad characters so they bilerp from empty space around each character
            font[i].s0 = (stb__arial_12_usascii_s[i]) * recip_width;
            font[i].t0 = (stb__arial_12_usascii_t[i]) * recip_height;
            font[i].s1 = (stb__arial_12_usascii_s[i] + stb__arial_12_usascii_w[i]) * recip_width;
            font[i].t1 = (stb__arial_12_usascii_t[i] + stb__arial_12_usascii_h[i]) * recip_height;
            font[i].x0 = stb__arial_12_usascii_x[i];
            font[i].y0 = stb__arial_12_usascii_y[i];
            font[i].x1 = stb__arial_12_usascii_x[i] + stb__arial_12_usascii_w[i];
            font[i].y1 = stb__arial_12_usascii_y[i] + stb__arial_12_usascii_h[i];
            font[i].advance_int = (stb__arial_12_usascii_a[i]+8)>>4;
            font[i].s0f = (stb__arial_12_usascii_s[i] - 0.5f) * recip_width;
            font[i].t0f = (stb__arial_12_usascii_t[i] - 0.5f) * recip_height;
            font[i].s1f = (stb__arial_12_usascii_s[i] + stb__arial_12_usascii_w[i] + 0.5f) * recip_width;
            font[i].t1f = (stb__arial_12_usascii_t[i] + stb__arial_12_usascii_h[i] + 0.5f) * recip_height;
            font[i].x0f = stb__arial_12_usascii_x[i] - 0.5f;
            font[i].y0f = stb__arial_12_usascii_y[i] - 0.5f;
            font[i].x1f = stb__arial_12_usascii_x[i] + stb__arial_12_usascii_w[i] + 0.5f;
            font[i].y1f = stb__arial_12_usascii_y[i] + stb__arial_12_usascii_h[i] + 0.5f;
            font[i].advance = stb__arial_12_usascii_a[i]/16.0f;
        }
    }
}

#ifndef STB_SOMEFONT_CREATE
#define STB_SOMEFONT_CREATE              stb_font_arial_12_usascii
#define STB_SOMEFONT_BITMAP_WIDTH        STB_FONT_arial_12_usascii_BITMAP_WIDTH
#define STB_SOMEFONT_BITMAP_HEIGHT       STB_FONT_arial_12_usascii_BITMAP_HEIGHT
#define STB_SOMEFONT_BITMAP_HEIGHT_POW2  STB_FONT_arial_12_usascii_BITMAP_HEIGHT_POW2
#define STB_SOMEFONT_FIRST_CHAR          STB_FONT_arial_12_usascii_FIRST_CHAR
#define STB_SOMEFONT_NUM_CHARS           STB_FONT_arial_12_usascii_NUM_CHARS
#define STB_SOMEFONT_LINE_SPACING        STB_FONT_arial_12_usascii_LINE_SPACING
#endif
