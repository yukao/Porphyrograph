// Font generated by stb_font_inl_generator.c (4/1 bpp)
//
// Following instructions show how to use the only included font, whatever it is, in
// a generic way so you can replace it with any other font by changing the include.
// To use multiple fonts, replace STB_SOMEFONT_* below with STB_FONT_arial_17_usascii_*,
// and separately install each font. Note that the CREATE function call has a
// totally different name; it's just 'stb_font_arial_17_usascii'.
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

#define STB_FONT_arial_17_usascii_BITMAP_WIDTH         128
#define STB_FONT_arial_17_usascii_BITMAP_HEIGHT         86
#define STB_FONT_arial_17_usascii_BITMAP_HEIGHT_POW2   128

#define STB_FONT_arial_17_usascii_FIRST_CHAR            32
#define STB_FONT_arial_17_usascii_NUM_CHARS             95

#define STB_FONT_arial_17_usascii_LINE_SPACING          11

static unsigned int stb__arial_17_usascii_pixels[]={
    0x00000000,0x7647a000,0x730bdb1e,0x00000000,0x00000000,0xc85f91d8,
    0x03b2fd44,0xcdfeca88,0x7cc47201,0x7ecc37f1,0xffd702ef,0x793c405b,
    0x3443ed98,0x1c9dfd88,0x743bfd5b,0x7b892601,0x9833a264,0x22b7e605,
    0x1002fba8,0x3990be3f,0x5bfb0f79,0x9a607f95,0x20e96c1e,0xd33db03c,
    0x322ffe5f,0x0f40790f,0x1f8991e2,0x01f881d8,0x887a0be6,0x4fc45f1f,
    0x0fd41949,0xf25c17d4,0x225b0f81,0x7cc3f106,0xb34f89f2,0x83e21aa0,
    0x35c0fc4c,0x6feccba2,0x2f8799e8,0x3e17c7e2,0x409d049a,0x3bbfb65d,
    0x6c7a1efe,0x21ea0592,0x25c85f2f,0x1f04c80f,0x101f1264,0xbf8a5c1f,
    0xf07a7fb8,0x42f8fc45,0x2f884dfc,0xae989ae0,0x56e41db9,0x43cc0d37,
    0x25c85f2f,0x5d03e82d,0x7c0f3264,0x3a2ecb61,0x445f0f26,0xfb505f1f,
    0x500fcc7f,0x0793c41f,0x1896c2ea,0x7cbe62f8,0xf0972744,0xa64ba605,
    0xf8be02ea,0x364d83f0,0x23f117c2,0x5f79302f,0x47b80fe2,0x26007a69,
    0xdc8bbd56,0x5df2fd89,0xf86883ee,0x5327d302,0x44be01eb,0x22e81f37,
    0x3f117c0f,0x6d2630be,0x25c81ba4,0xfffffffe,0xd96cb601,0x3f3fee21,
    0x03dff9f2,0x5d03d81d,0x220d5264,0x87e27e1f,0x5f09b1fa,0x262f8fc4,
    0x5c7d498f,0x6c2f981f,0x4d300d32,0x2f80790f,0x43c8025f,0x320f884c,
    0x23cc1e24,0xff53bd3d,0x45f01d95,0xdf85f1f8,0x7e40fac9,0xe84fd99c,
    0x45b00971,0x2f803b1d,0xa8b5005f,0x1320f886,0x5b8d503f,0x9ff39fb1,
    0x221f8111,0xfe985f1f,0xffb702ef,0x1e4f107b,0xdbd51a60,0x00be5f00,
    0xf107e23c,0x207e2641,0x400de82e,0x883e62f9,0x12605f1f,0x11040040,
    0x01881980,0x54040000,0xd1323dc5,0xdd106987,0x1fe54435,0x8ffec9f9,
    0x002887ff,0x00000000,0x74000000,0x44c87f90,0x500765fb,0x19dffffb,
    0x00000088,0x00000000,0x80000000,0x01101108,0x22000c11,0x00000001,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x98017400,0x1d801ced,0x3a05f6d4,0xfc887601,
    0x3bf9101d,0x9dfb7059,0x77f6d403,0x33ff6e04,0x3bffaa02,0x2f803e02,
    0xbfa97fa2,0x22f806c9,0x7c5d9bfa,0x8dc87202,0xd33db06b,0xa9bfc87f,
    0x9bfb80fd,0xdfc86ea9,0xfa82fcaa,0x982fcaac,0x3d917c05,0x00762741,
    0x1f326c5f,0x27d0017c,0x30fcc0f9,0x6c0fdc7f,0x7c40fd45,0x3ee03ee2,
    0x436c26c1,0x9917c03c,0x64400f88,0x9362fbef,0x3f2be0f9,0x9fc8761e,
    0x3f87a85d,0x360c413a,0x03e82605,0x310bb13a,0xb2f803d0,0x27e61dff,
    0x4f6c1cfe,0x37a62fe9,0x45ffc3eb,0xff85f0fc,0x47f0f504,0x00fe002f,
    0x8d900fc4,0x2201cff9,0xbb12f807,0x64df7d43,0x2f987e27,0xf0bfbf62,
    0x225f4f89,0x87e04fed,0x003f33f9,0x99970be2,0x7b803e67,0x0cffec88,
    0x917c0254,0x97c3fa89,0x93e2f87a,0xbb0be2f9,0x7d46ecbe,0x5576c6bb,
    0x800be3fe,0xb777643f,0x01b201f8,0x42c8df71,0x89917c08,0x7aaec0f9,
    0xd81f32f8,0x3e990be4,0x3fee1f8a,0x2f9dea83,0xb06cc13a,0x07d35c0b,
    0x301912ec,0x9ea07c3f,0xf11321f8,0x43f13e85,0xe81f12f9,0x3e5f0fe3,
    0x0ff42f8a,0x7dc3f031,0x17dc6d80,0xd83f26c8,0x41fc1faa,0xf10d30f8,
    0x2641f937,0x21fb8764,0x22fd89dd,0x7c1f70dd,0x4be7b8af,0xfffb89ed,
    0x43e44fa0,0x1fc98aed,0xda99cfb8,0x5ce7ec4f,0x9bfc85ff,0x412e5ea9,
    0x2642ffeb,0x442efea8,0x222eaefd,0x57c2dffc,0x44be1dfd,0x3e89effb,
    0x207bffaa,0x01dffec8,0x077fff6e,0x7e6fff5c,0x4fffeb84,0x640200ec,
    0x00200404,0x10004004,0x04002200,0x81000400,0x01804048,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0xdb300000,0x41d85b87,
    0x10004eda,0xddddd103,0x225c0f23,0x8ae44eda,0x2e2d883d,0x34c0ec84,
    0xd725c099,0x2bdddddd,0x36afee5b,0x47a83e65,0x445eaaeb,0x26c1cfeb,
    0x8e6667cc,0x7ccb904e,0xb9f65eab,0x1ee2f41f,0x54d90fbe,0x54c7f00f,
    0x92aacfaa,0x45f11f4d,0x45f1363e,0x227b20f9,0x2e1360fc,0x2e413a05,
    0x7d8fc47d,0x40ec876c,0x16faa1f8,0xfa83f87d,0x5b20be00,0xd71ec0f9,
    0x4f7007e2,0xfc9f42f8,0xe80a645f,0x03e2b904,0x9f11b27d,0xc8fa03f5,
    0x907e279c,0x2f80bb0d,0xf83f16c8,0x81ae7e24,0x740c3fb9,0xee839d13,
    0x413a2efe,0x4d81f35c,0x07ff50d9,0x47e5d264,0x221f987a,0x6417c02f,
    0x2fdc3766,0xeb80f736,0xb07dc02f,0xf9119f09,0xaae413a1,0x20b75c87,
    0x45aa00fe,0x41729b0f,0x3e01ae3e,0x3f666c82,0x7fd44cdf,0x702f9800,
    0xd004d83f,0x9aec1367,0x90b54d87,0x47c40ddd,0x81f5ae5b,0x3e00fa6b,
    0xb0886c82,0x6c00bf07,0x4d81f984,0x4172b900,0x3d81f14d,0xf9afa895,
    0x88f6ba03,0x3f100f8f,0x05f001f1,0x0be064d9,0xd07e20be,0x1360d507,
    0xf73f83f1,0x0be3f881,0x49f1045f,0x3f9901fb,0x66c06be8,0x905f005b,
    0x2e446e4d,0x8dd81ae6,0x02980fc8,0x88dd83b9,0x35bd10fc,0x4f5c1dd7,
    0x6d8ea6d8,0x7fa80ed8,0x74c04fc8,0x905f002e,0x06ffec4d,0x64403ff1,
    0x09501dff,0xff911fea,0xfffd983d,0x1dfea80d,0x440fddb2,0xa85f885f,
    0x007e802f,0x021b20be,0x20040008,0x0020007c,0x00004002,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0x201b7000,0x3bbbb23d,
    0x900ec2de,0x019ddddd,0xeeeee92e,0x205db4ee,0x6c07b2eb,0x1722ea83,
    0x9703ca5c,0xeeeec8f4,0x36eb73ee,0x20fc85f8,0xfdbaaade,0xade80be2,
    0xfb86fbaa,0x2eaaaaa4,0x3a0bfd4f,0x27409d3f,0x09fd0fea,0x2e4136b9,
    0x2aabb63f,0x4ea751aa,0xd0be62fa,0x02f97a09,0x44be613a,0x3a204efd,
    0xd303f7a6,0x4e813a7d,0x3fba0fea,0x904dae41,0x005d8fcb,0x42f43f60,
    0x97cbb04e,0x2213a4d8,0x0991952f,0x5d7d07ec,0x09d3ecc8,0xd02faa74,
    0x9b5c8db7,0xbb1f9720,0x5e7c4000,0x5cc4e80f,0x22ecbe2f,0x9907b84e,
    0x77d07f20,0x9d3e9e8b,0x027d6740,0xae49f17d,0xeaaaaadd,0x4ceec7e5,
    0x3ea00099,0xffffd02f,0x5daf85bf,0xffeeefe8,0x5f509903,0x261f37d0,
    0xe813a7d6,0x7d00fdfe,0xfdae47ea,0x25feeeee,0xfffffb1f,0x205e800b,
    0x83fa99de,0xcee83fff,0x13201acc,0x74fa09f1,0x274fa792,0x417a5fd0,
    0xdae5b63e,0xd8fcb904,0x83a9d405,0xf513a04d,0x41fadf87,0x1264004e,
    0x5b9f40bd,0x0274fa1f,0x741fcc9d,0x6d733e23,0xd8fcb904,0x86cb6405,
    0x3213a04d,0x236cbe1f,0xb264004e,0xf99f401d,0x0274fa6a,0x741fb89d,
    0x4daf7d43,0x5d8fcb90,0x026c0580,0x7cdd109d,0x02747f12,0x4d7dd320,
    0x47d09999,0x674fa3fe,0x09d09999,0x6c1f41dd,0x97209b5f,0x2b80bb1f,
    0x7409b075,0x8be7f504,0x8009d1fa,0xfffffb4c,0xfb8fadff,0x7fff4fa0,
    0xf109d6ff,0xafc41f4b,0x8fcb904d,0x01b2005d,0x00000000,0x00000000,
    0x00000000,0x66c00000,0x0eeeeeee,0x05bdddd9,0x039ddddb,0xdb500bb0,
    0x77774c17,0x43d104ee,0x0000001d,0x5c000000,0xaaaaadda,0x32eab7a2,
    0x5556f45f,0x3faa02fd,0x366fae01,0x33333227,0x7c3fb04f,0x27ffd402,
    0x405ff5c4,0xec80bfda,0x7fec40df,0xe804d83e,0x13a3f984,0x5ccd8176,
    0x405f0be0,0x43fd706c,0x22be202f,0x4d76c3f9,0x6457647c,0x36c57647,
    0x07ee277a,0x209d009b,0x13613a6d,0x807d4fc4,0x3f304e81,0x7c3f3f30,
    0x3e62efba,0x42f98221,0x25d0ba2f,0xb83222f8,0xddba97c0,0x3a099999,
    0x56f4f704,0xd700ecaa,0x1fa801fc,0x22ba09d0,0x322bfe1f,0x073bf60f,
    0x3e6b90f5,0xf34feeee,0xfdb75001,0x3fff7b25,0x413a4fff,0xffffd0fa,
    0xc8fa01bf,0x304f9806,0x3e29901f,0x42f88fe1,0x1ea3feb9,0x555fcd72,
    0x801f32aa,0x42f8acfe,0x704e804d,0x1b66274f,0x3f2aafe6,0xd813e601,
    0x3f10ea85,0x1c4ba17c,0x4e83f1ee,0x17c350be,0x883ea3b1,0x74026c2f,
    0x4274d904,0x777e41f9,0x7f505fee,0x77417c40,0x5f4efeee,0x227f12e8,
    0x5c4eec5d,0x7d47b21f,0x9b227721,0x87fb12f9,0x884e804d,0x3ea13a3f,
    0x07dc17c1,0x0f500fea,0x33eeaaaa,0x2a2e85f1,0x5c40dffe,0x75c42efe,
    0x7f5c43ef,0x5fffe41d,0x4cceec4d,0x33bd0999,0x6741df75,0x546eb999,
    0x223f880f,0x409999cf,0x43f1006b,0x0041742f,0x08008008,0x3ff60040,
    0xfd2fffff,0xe817dfff,0x40ceffff,0xfb9b205d,0x644fffff,0x7c3f1005,
    0x00001742,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00ec1e80,0x00000000,0x792dc3d3,0x10970f4c,0x3bbbaa0f,0x3d112e6e,
    0x8e980010,0x3e35404d,0x02d897c1,0x54f7ecec,0x77d744fe,0x87dfb3b0,
    0x35c3f36d,0x441f47f9,0xf3333107,0x0d707cc9,0x30ae05f3,0xf9109d1f,
    0x17c1f87d,0xff017ff7,0xf33bfb35,0xff1dffc5,0x7441f717,0xe87cc3fc,
    0xf101f15d,0x8fa17a00,0x4c5ec83e,0x1f10dccc,0x16fecc9b,0x5417c1f8,
    0xa8fe1cfd,0x83f9362f,0x3e617c5f,0xf3f15f06,0xffff89aa,0x01db07ff,
    0xf103e2d7,0x77d443f1,0xdf12bb41,0x17c1f801,0x317cfd30,0x20be5c8f,
    0x3ea17c2f,0xd5acc80f,0xfa9987b3,0x00fc8199,0xb70b71f1,0x05b97132,
    0x402fbee0,0x220be22f,0x98be3efb,0xf05f2e47,0x2be22f85,0x93cea85d,
    0x81e201f9,0x75ec01fb,0x41f12f82,0xea800111,0x446f41cf,0x37f662fd,
    0x91e62f82,0x217c17cb,0x17c57a2f,0x06fa8ffc,0x4dfd41e2,0x87fa8199,
    0x00016c6a,0xff31f260,0x33e25d7f,0x4798be00,0x0be0be5c,0xfb83ee5f,
    0x09f13ec0,0xffffb018,0x009f03ff,0x01000000,0x2f800802,0x17cb91e6,
    0x000be17c,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x44000000,0x24cccccc,0x82ae20e9,0xcc993a08,0xfffff12c,0x00009fff,
    0x00000000,0x00000000,0x33333322,0x77cc3e64,0x221fcdff,0x07bbb50f,
    0x00000000,0x00000000,0x00000000,0x511987cc,0x000107bb,0x00000000,
    0x00000000,0x55000000,0x06855555,0x00000000,0x00000000,0x00000000,
    0x3ffe2000,0x00007fff,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,
};

static signed short stb__arial_17_usascii_x[95]={ 0,1,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,-1,1,0,1,1,1,0,1,1,0,1,
1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,1,0,0,0,-1,0,0,0,0,0,0,0,0,1,1,
-1,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0, };
static signed short stb__arial_17_usascii_y[95]={ 13,2,2,1,1,1,1,2,1,1,1,4,11,8,
11,1,2,2,2,2,2,2,2,2,2,2,5,5,3,5,3,1,1,2,2,1,2,2,2,1,2,2,2,2,
2,2,2,1,2,1,2,1,2,2,2,2,2,2,2,2,1,2,1,15,2,4,2,4,2,4,1,4,2,2,
2,2,2,4,4,4,4,4,4,4,2,5,5,5,5,5,5,1,1,1,6, };
static unsigned short stb__arial_17_usascii_w[95]={ 0,2,5,9,8,13,10,3,5,5,6,9,2,5,
2,5,8,5,8,8,8,8,8,8,8,8,2,2,9,9,9,8,15,12,9,11,10,9,8,11,9,2,7,10,
7,11,9,12,9,12,10,10,9,9,11,15,11,11,9,3,5,4,7,10,4,8,8,8,8,8,5,8,7,2,
4,7,3,11,7,8,7,8,6,8,5,8,8,11,8,8,8,5,2,5,9, };
static unsigned short stb__arial_17_usascii_h[95]={ 0,11,4,13,14,13,13,4,16,16,6,8,5,2,
2,13,12,11,11,12,11,12,12,11,12,12,8,11,9,5,9,12,16,11,11,13,11,11,11,13,11,11,12,11,
11,11,11,13,11,13,11,13,11,12,11,11,11,11,11,15,13,15,7,2,3,10,12,10,12,10,12,13,11,11,
15,11,11,9,9,10,13,13,9,10,12,9,8,8,8,12,8,16,16,16,3, };
static unsigned short stb__arial_17_usascii_s[95]={ 127,71,121,80,58,90,1,11,1,22,114,
78,125,30,125,69,62,42,45,19,63,43,89,54,107,1,125,122,1,1,20,
28,28,32,22,21,11,1,113,33,99,124,75,78,70,58,89,67,32,45,13,
58,114,52,102,86,74,1,48,54,121,49,106,36,25,116,116,107,98,98,83,
12,72,125,44,24,109,30,49,89,113,104,42,80,37,11,97,66,57,10,88,
16,13,7,15, };
static unsigned short stb__arial_17_usascii_t[95]={ 1,32,69,1,1,1,18,79,1,1,69,
69,54,79,60,18,32,45,57,32,57,32,18,57,18,32,45,45,69,79,69,
32,1,57,57,18,57,57,45,18,45,32,18,45,45,45,45,1,45,18,45,
18,32,32,32,32,32,45,45,1,1,1,69,79,79,57,18,57,18,57,18,
18,57,18,1,45,45,69,69,57,1,1,69,57,32,69,69,69,69,32,69,
1,1,1,79, };
static unsigned short stb__arial_17_usascii_a[95]={ 68,68,86,135,135,216,162,46,
81,81,95,142,68,81,68,68,135,135,135,135,135,135,135,135,
135,135,68,68,142,142,142,135,247,162,162,176,176,162,149,189,
176,68,122,162,135,203,176,189,162,189,176,162,149,176,162,230,
162,162,149,68,68,68,114,135,81,135,135,122,135,135,68,135,
135,54,54,122,54,203,135,135,135,135,81,122,68,135,122,176,
122,122,122,81,63,81,142, };

// Call this function with
//    font: NULL or array length
//    data: NULL or specified size
//    height: STB_FONT_arial_17_usascii_BITMAP_HEIGHT or STB_FONT_arial_17_usascii_BITMAP_HEIGHT_POW2
//    return value: spacing between lines
static void stb_font_arial_17_usascii(stb_fontchar font[STB_FONT_arial_17_usascii_NUM_CHARS],
                unsigned char data[STB_FONT_arial_17_usascii_BITMAP_HEIGHT][STB_FONT_arial_17_usascii_BITMAP_WIDTH],
                int height)
{
    int i,j;
    if (data != 0) {
        unsigned int *bits = stb__arial_17_usascii_pixels;
        unsigned int bitpack = *bits++, numbits = 32;
        for (i=0; i < STB_FONT_arial_17_usascii_BITMAP_WIDTH*height; ++i)
            data[0][i] = 0;  // zero entire bitmap
        for (j=1; j < STB_FONT_arial_17_usascii_BITMAP_HEIGHT-1; ++j) {
            for (i=1; i < STB_FONT_arial_17_usascii_BITMAP_WIDTH-1; ++i) {
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
        float recip_width = 1.0f / STB_FONT_arial_17_usascii_BITMAP_WIDTH;
        float recip_height = 1.0f / height;
        for (i=0; i < STB_FONT_arial_17_usascii_NUM_CHARS; ++i) {
            // pad characters so they bilerp from empty space around each character
            font[i].s0 = (stb__arial_17_usascii_s[i]) * recip_width;
            font[i].t0 = (stb__arial_17_usascii_t[i]) * recip_height;
            font[i].s1 = (stb__arial_17_usascii_s[i] + stb__arial_17_usascii_w[i]) * recip_width;
            font[i].t1 = (stb__arial_17_usascii_t[i] + stb__arial_17_usascii_h[i]) * recip_height;
            font[i].x0 = stb__arial_17_usascii_x[i];
            font[i].y0 = stb__arial_17_usascii_y[i];
            font[i].x1 = stb__arial_17_usascii_x[i] + stb__arial_17_usascii_w[i];
            font[i].y1 = stb__arial_17_usascii_y[i] + stb__arial_17_usascii_h[i];
            font[i].advance_int = (stb__arial_17_usascii_a[i]+8)>>4;
            font[i].s0f = (stb__arial_17_usascii_s[i] - 0.5f) * recip_width;
            font[i].t0f = (stb__arial_17_usascii_t[i] - 0.5f) * recip_height;
            font[i].s1f = (stb__arial_17_usascii_s[i] + stb__arial_17_usascii_w[i] + 0.5f) * recip_width;
            font[i].t1f = (stb__arial_17_usascii_t[i] + stb__arial_17_usascii_h[i] + 0.5f) * recip_height;
            font[i].x0f = stb__arial_17_usascii_x[i] - 0.5f;
            font[i].y0f = stb__arial_17_usascii_y[i] - 0.5f;
            font[i].x1f = stb__arial_17_usascii_x[i] + stb__arial_17_usascii_w[i] + 0.5f;
            font[i].y1f = stb__arial_17_usascii_y[i] + stb__arial_17_usascii_h[i] + 0.5f;
            font[i].advance = stb__arial_17_usascii_a[i]/16.0f;
        }
    }
}

#ifndef STB_SOMEFONT_CREATE
#define STB_SOMEFONT_CREATE              stb_font_arial_17_usascii
#define STB_SOMEFONT_BITMAP_WIDTH        STB_FONT_arial_17_usascii_BITMAP_WIDTH
#define STB_SOMEFONT_BITMAP_HEIGHT       STB_FONT_arial_17_usascii_BITMAP_HEIGHT
#define STB_SOMEFONT_BITMAP_HEIGHT_POW2  STB_FONT_arial_17_usascii_BITMAP_HEIGHT_POW2
#define STB_SOMEFONT_FIRST_CHAR          STB_FONT_arial_17_usascii_FIRST_CHAR
#define STB_SOMEFONT_NUM_CHARS           STB_FONT_arial_17_usascii_NUM_CHARS
#define STB_SOMEFONT_LINE_SPACING        STB_FONT_arial_17_usascii_LINE_SPACING
#endif
