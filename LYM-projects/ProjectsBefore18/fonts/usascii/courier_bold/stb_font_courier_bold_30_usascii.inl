// Font generated by stb_font_inl_generator.c (4/1 bpp)
//
// Following instructions show how to use the only included font, whatever it is, in
// a generic way so you can replace it with any other font by changing the include.
// To use multiple fonts, replace STB_SOMEFONT_* below with STB_FONT_courier_bold_30_usascii_*,
// and separately install each font. Note that the CREATE function call has a
// totally different name; it's just 'stb_font_courier_bold_30_usascii'.
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

#define STB_FONT_courier_bold_30_usascii_BITMAP_WIDTH         256
#define STB_FONT_courier_bold_30_usascii_BITMAP_HEIGHT        104
#define STB_FONT_courier_bold_30_usascii_BITMAP_HEIGHT_POW2   128

#define STB_FONT_courier_bold_30_usascii_FIRST_CHAR            32
#define STB_FONT_courier_bold_30_usascii_NUM_CHARS             95

#define STB_FONT_courier_bold_30_usascii_LINE_SPACING          14

static unsigned int stb__courier_bold_30_usascii_pixels[]={
    0x000fbae0,0x98000326,0x2f220000,0xdddd9101,0x7912e203,0x39100080,
    0x6f775400,0x0000000b,0x00008000,0x00010000,0x80000000,0x00008800,
    0x20013ff2,0x07d504ff,0x2ea81fe2,0x903fff70,0x503fffff,0x3fe65fff,
    0x3e23fd01,0x2006fd86,0x03fffffa,0x07bffd71,0x0bbffaa0,0x77fed440,
    0x4df76d43,0x002dfffd,0xd39fffd7,0xfb5019dd,0x36a005bf,0xeeb9dffe,
    0x20f7dc3d,0x04efffc8,0x004ffc80,0x7c417fdc,0x03ff506f,0x3a603ff6,
    0xffd986ff,0xeffe81ff,0xf103ff50,0x703fee7f,0x3ea005ff,0xea80dfff,
    0x1effffff,0x3fffff20,0x7ffcc02f,0xff11ffff,0xffffffff,0x3a201bff,
    0xffffffff,0x7fdc4fff,0x2206ffff,0xfffffffc,0xf11fffff,0x7ffc4bff,
    0x000effff,0x0ffd0000,0x3aa0fff2,0x2a2bbeff,0x3ff205ff,0x307fea00,
    0x1ffa87ff,0xff73ff88,0x006ff881,0x7cc03ff5,0xfffddeff,0x22ffa81f,
    0x3fea07fa,0x645dbdff,0xcdfffffe,0xfd05fffe,0xffffbdff,0x7fcc3bff,
    0x03fffcdf,0xfd9bfffb,0x4c9fffff,0x3ffa7fff,0x05fffdcf,0xffa80000,
    0x20fff883,0xffffffe8,0x80fff47f,0x3ea00ffb,0x20ffd41f,0xff981ffa,
    0x3f20ff92,0x1ffa802f,0x907ffd10,0x05fd0dff,0xfff503fb,0x77ffc007,
    0x21fff441,0x2e21dffb,0xffc83fff,0x503ffa21,0xfe881dff,0x7ffc40ff,
    0xfd0bff36,0xddd9103f,0x001ddddd,0xff901ffd,0xdefffc87,0x3ee0ffff,
    0x01ff704f,0x3ea0ffd4,0x40ffd41f,0x0ff92ff9,0x2a017fe2,0x5ff901ff,
    0xf327fdc0,0xf10fea0d,0xff8005ff,0x8dff101f,0xffd01ffe,0xf50dfd07,
    0x00ffd89f,0xff01fff1,0x207feebf,0xffb82ffa,0x0fffffff,0x4427fd40,
    0x77fc0fff,0x3e1ff220,0x0ffb80ff,0xf507fea0,0x81ffa83f,0x0dfb1ffa,
    0x2a00ffe4,0x0dff01ff,0x2fd4dfd0,0xf713f2a2,0xdff0007f,0x7fc7fc80,
    0x10ffea05,0x1bfa07ff,0xffd80bff,0xf55fff80,0x05ff705f,0xffffffb3,
    0xffd801ff,0x444ffa80,0x905903ff,0x1ff705ff,0x2a0ffd40,0x0ffd41ff,
    0xffffffd5,0xff309fff,0x81ffa80b,0xfb803ff8,0xffa8ff27,0x9bff44ff,
    0x6ff801bb,0xff13fec0,0x21ffd409,0xff885ff8,0x3a037fc7,0x4ffe80ff,
    0x7cc3dff1,0x7dc000ff,0x13fe600f,0x7f41ffdc,0xff5000bf,0x403ff509,
    0x7fdc1ffa,0xfb0ffd41,0xffffffff,0x1ffb01ff,0x540ffd40,0xffa801ff,
    0x3fe23f91,0x6ffc4fde,0x00effffd,0x7cc0fffe,0x807ff46f,0x7fe83ffd,
    0x360fffd8,0xfff502ff,0x647ffb01,0xffffefff,0x0ffb8003,0x6407ff60,
    0xfffb81ff,0x3e601cef,0x4fff985f,0x3623ff50,0x7fd40fff,0x3bfff261,
    0x302dfffe,0x7fd409ff,0x801ffa81,0x23f91ffa,0x449f53fc,0xffffffff,
    0xffff00ff,0x3fff2639,0x4c27fe43,0xffb83fff,0xffffd9ae,0x473ffea1,
    0x80ffffc9,0x7ffc2ffd,0x0006ffff,0x3e201ff7,0x03ffa05f,0x7fffff54,
    0x20dff01e,0xfa81fffe,0x09fff71f,0xff903ff5,0x7f4027fc,0x03ff500f,
    0xf7005ff3,0x3fc7f21f,0x7ffc49f5,0x4ffd9aef,0xfffffff0,0xf109ffff,
    0xffd57fff,0x3ffa07ff,0x1fffffff,0x7fffffdc,0xc80fffff,0x3fff22ff,
    0x003fffff,0xf900ffb8,0x03ffc03f,0x7ffff64c,0xf987ff80,0xffa80dff,
    0x50fffd11,0x2ffb03ff,0xfa803ff8,0x03ff504f,0xfb009ff1,0x0fd8fe4f,
    0x6fff89f5,0xdff07fe8,0x7ffffffd,0x7ffffd40,0x103fffff,0xf9dffffb,
    0x3fffee0f,0x80ffefff,0xdff51ffb,0x3fff7117,0x80ffb800,0x7f406ff8,
    0x3f22000f,0xa87ff83f,0x3fea02ff,0x2a0ffdc1,0x3faa21ff,0x01dffbaf,
    0xfa80ffe8,0x03ffb01f,0x3f92ffd4,0x13f33fea,0xff903ffd,0x32a9ff03,
    0xe8800acd,0xffbeffff,0xe95dcc03,0x66e5406f,0xf101ff92,0x5c17fe4d,
    0x7dc003ff,0x02ffc80f,0x3ee0ffec,0xf113fe03,0x01ff70bf,0x3ea0ffd4,
    0x98ffd41f,0xffffffff,0x7d403fff,0x01ffa83f,0xf303dff5,0x917dc1ff,
    0xfc8fffff,0x81ffb80f,0x100004ff,0x0ffe2353,0x009ff900,0x4003ff20,
    0x17fe07fd,0x881ff700,0xffb806ff,0x5407fec2,0x27fd42ff,0xf5007fdc,
    0x41ffa83f,0xfff11ffa,0xffffffff,0x50ffd007,0x7fdc03ff,0xfffcaacf,
    0x2e21fd43,0x27fd42cc,0x7fc07ff4,0x2e000004,0xf93001ff,0x7e4000df,
    0x3ff9000f,0x8007ff50,0x7fdc0ffb,0x44ffa802,0xdbacfffe,0x3ffb87ff,
    0x2a00ffb8,0x0ffd41ff,0xff307fea,0x015ff73b,0xf50bfee0,0xfff9803f,
    0x883fffff,0xffd8006f,0x86ffdbad,0x0001dff8,0xffb99988,0xfd759507,
    0x20001fff,0x1101ffc9,0x5567ffcc,0x4001fffc,0x37fc0ffb,0x6c7ff880,
    0xffffffff,0x80ffe83f,0x3ea00ffb,0x20ffd41f,0x7fc41ffa,0x4003ff74,
    0x7fd46ff8,0xffff9801,0x0bfa01df,0xfffff100,0xffd83fff,0x8004ffff,
    0xffffffe8,0xffff882f,0x8001efff,0x6ffffffb,0xff90ffdc,0x07ffffff,
    0x541ff700,0x3f2003ff,0x3fb7ee3f,0xfa80cfff,0x01ff705f,0x3ea0ffd4,
    0x20ffd42f,0x1ff73ff8,0x22ffc800,0xfb801ffa,0x0ea0aeff,0x75c17ff3,
    0xffffb300,0xffff883d,0x98006fff,0x2fffffff,0x3fffff20,0x3ff60003,
    0x2e0fffff,0x7ff541ff,0xd0002eff,0x00ffe8ff,0xb81bfe20,0x3ffa02ff,
    0x886feb81,0x441ffdca,0x3fea2eff,0xfc9ffcc1,0x6ff88007,0x40b37fea,
    0xfffffffa,0x7dc1fffe,0x00fffeff,0x2e600998,0x000bcccc,0x03799930,
    0x00004cc0,0x5e6665c4,0x26200981,0x26660009,0x3ea5ffd9,0xffd8004f,
    0x203ff502,0xffb86ffa,0xffffc82f,0x2fffd81f,0xff983ff5,0x2000ff92,
    0xfff51ffc,0xfffd07ff,0xdfffffff,0x33bff6e0,0x00000001,0x00000000,
    0x00000000,0x3ffea000,0x360effff,0xf88000ff,0x03ff506f,0x3ea07ff6,
    0xffff502e,0x43ff203f,0x7fc40ef8,0x2000bf71,0xfff54ff8,0x7bd503ff,
    0x05bd9535,0x00000110,0x00000000,0x00000000,0x32000000,0x0effffff,
    0x50001fd4,0x81fe209f,0x980003fb,0x40000199,0x00005419,0x266625d7,
    0x00000001,0x00000000,0x00000000,0x00000000,0xccca8800,0x0000000a,
    0x00000440,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x11000080,0x17bdb700,0x01ef75c4,
    0x00000000,0x06eec980,0x6dc005cc,0x000003ee,0x02000000,0x3bbbbba0,
    0x6c2eeeee,0xceeeeeee,0x00000001,0x04eed800,0x3ae006ee,0x6cc02dff,
    0xa80cffff,0x2e2fffff,0x101fffff,0x007bffd7,0x6f7f6544,0x6c000ba9,
    0xfd307fff,0x7fc409ff,0x440004ff,0xb9beffec,0x03fb7100,0x7dffd930,
    0x3fffffe0,0x7c3fffff,0xffffffff,0x7f6dc406,0x980b9bdf,0x00ceffec,
    0x20dfff50,0x7e406ffb,0x902fffff,0xffffffff,0xdfffd883,0x37fff660,
    0xffffea80,0x7cc01eff,0xffffffff,0x7ed4002f,0x915dd07f,0x7ff6403f,
    0x3fea0004,0x4fffffff,0x3fffff22,0x7fffd401,0x7fc0efff,0xffffffff,
    0x7ffffc3f,0x2202efff,0xfffffffe,0x3ffa23ff,0x01ffffff,0x837fffc4,
    0x3ee07ffc,0x1fffddff,0x666fffcc,0xffd06fff,0x20bff301,0xfddefff9,
    0xff101fff,0xfffd9bff,0x3f20009f,0x2f42f987,0x80027fc0,0xeddffff8,
    0x3ff65fff,0x2a01ffff,0xffdcefff,0xb817f45f,0x02ffc1ff,0x77fffcc0,
    0x24fffedc,0xfddffff9,0x3f200eff,0xb9106fff,0x30fff405,0x1ef88dff,
    0xfa81ffd8,0x81ffd84f,0xfc83ffe8,0x4ffe886f,0x004ffe98,0x3f50ff90,
    0x4ff806e8,0x07ff9000,0x77d4bff9,0x3601ffcc,0x7ff442ff,0x7fe80620,
    0xe8005ff8,0x9ff703ff,0x7e41ffcc,0xddff103f,0x7d4000df,0x00ffd84f,
    0x740bfe60,0x17fe60ff,0xf701ffe4,0x03ffa87f,0xda80ffe2,0x7fc9dfff,
    0x0bea1bd0,0x3f673fe0,0x3f602dff,0xa813fa07,0x13fe01ff,0x88017fd4,
    0x35ff84ff,0x3ffb8035,0x3fe17f40,0xb04ff880,0x00dfd5ff,0x5c17fe40,
    0x3ee001ff,0x49ff301f,0xdff01ffd,0xfc8ffd00,0x881fc807,0xfffffffd,
    0xdff707ff,0x7c15c4df,0xffffffff,0x1ffc80df,0x7d403b50,0x402e601f,
    0xf7002ffa,0xfffff03f,0x3fa05fff,0x18839807,0xf509ff30,0x001bfabf,
    0xff107fd8,0xffca9809,0x4c7fec07,0x7ff105ff,0x741ff700,0x7ec0005f,
    0xffffcfff,0x2b72207f,0xf85ffeb8,0xffcfffff,0xfff985ff,0x7d4000ac,
    0x3fa0001f,0x0ffd000f,0x3ffffffe,0x05ff84ff,0xffe98000,0xfd1ffe81,
    0x37f4000d,0x7f40bff0,0x4c01ffff,0x01ffddff,0xf5003ff5,0x00ffe23f,
    0x42effb80,0x4407ffea,0x01cfffeb,0xea85dfff,0xfffb82ff,0xa801ceff,
    0xfb0001ff,0x3fe600bf,0x57dffb04,0x7c43fffb,0x2600003f,0xfb86fffd,
    0x741bfa4f,0x0dfd00ee,0xfff02ffc,0xfb001fff,0x7d40bfff,0x2ffa801f,
    0x40007ff1,0xff701ffe,0xdffd710f,0x5fff0039,0x541bfe60,0x4ffffffe,
    0x000ffd40,0x003bffa2,0xa880ffdc,0x225ffa80,0xccc983ff,0xff500acc,
    0xfff10bff,0x7d437f41,0x81bfa04f,0x32a205ff,0x4c00effe,0x2601ffff,
    0xffb802ff,0x0007ff10,0xff1037fc,0x337dfd0f,0x7fc05bf9,0x203ff407,
    0x4fffeca8,0x8007fea0,0x000effe8,0xf8001ffa,0x50ffe26f,0x9fffffff,
    0x4077fea0,0xeaaadffc,0x07fe40ef,0x3e20ffb0,0x3ff2004f,0x05ffd804,
    0xfd804ff8,0x8002ff47,0x7ec04ff8,0x3fe60987,0xbff03fec,0x220ffc80,
    0x81ffa600,0xf3001ffa,0x4c001dff,0x320004ff,0x2627fc7f,0x2fffffff,
    0x7c01ff10,0xffffffff,0x00bfe1ff,0xff503ff7,0x06ff8007,0xe801ffe8,
    0x3fea01ff,0x400ffdc5,0x2206ff80,0x47c807ff,0x01fff06d,0x1ff13ff4,
    0xa80ffdc0,0xffa801ff,0xffb8005f,0x1bfa0001,0x26660dfb,0x004404ff,
    0x3ffffffe,0xbf31ffff,0x909ff300,0xfd8005ff,0x02ffd407,0x981eff98,
    0x7fcc0fff,0xb17f200e,0x3fee05ff,0x7d44f807,0x503fff81,0x2ffd4bff,
    0x2a01ffd0,0x7fdc01ff,0xffd0004f,0xff3014c0,0x404ffb8b,0x980004ff,
    0xfe999999,0x4003f20e,0x37fc46ff,0x0bff3004,0x03ffd331,0x567ffdc0,
    0x703fffca,0x95359fff,0xff98dffd,0xffec9bef,0x1ba02cff,0xfffca8f9,
    0xffeb9cef,0x3bffea1f,0x6fffdaac,0xd807fea0,0x30003fff,0x3fe209ff,
    0xffca9abe,0xbfffd82f,0x4ffecb9a,0x22000cc0,0x2e3fffcb,0x3ffee002,
    0x7c3fffbb,0xfca9aacf,0x3ffa23ff,0x003fffff,0x3ffffff2,0x7fdc03ff,
    0xefffffff,0xfffffb81,0xffffffff,0xf95bf500,0x7ffffc49,0xffffffff,
    0x7ffffcc3,0x21ffffff,0xdffdccb8,0xfff70bcc,0xdb99999d,0x00ffdc01,
    0xfffffffb,0x3fa209ff,0xffffffff,0x13ff203f,0xfffffb80,0x3f60002f,
    0x445fffff,0xffffffff,0x7ffcc4ff,0x005fffff,0x77ffff4c,0xffd95001,
    0xe9809dff,0xfddfffff,0x7fdc05ff,0x7ffec0cf,0xefffffdd,0xffd7bb01,
    0xfd819fff,0xffffffff,0xfffff94f,0x05ffffff,0x3220274c,0x2effffff,
    0xffffeb80,0x7ec01cff,0x3fa6004f,0x0000dfff,0x827fffec,0xffffffd9,
    0x9999502e,0x20001799,0x20000998,0x988001a8,0x0c00009a,0x04d4c400,
    0x806a6600,0xffffffea,0xfff91eff,0xffffffff,0x99800007,0x5300009a,
    0x01980033,0x80000000,0x55310019,0x00000003,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x9ddb5000,0xeeec8800,0x774c0000,0xdddb7005,0x665c403d,0x3332e2cc,
    0x009b00bc,0x66665c40,0x8802cccc,0x20accecb,0xcccccccc,0x2e3ccccc,
    0x65c43ccc,0x666543cc,0x333260bc,0x6665c2cc,0x1e665c43,0x2e0f332a,
    0x2a0bcccc,0x32a02ccc,0x332e22cc,0x99731ccc,0x3fffe079,0x7ffe4005,
    0x7cc0001f,0xfff8806f,0x3f201fff,0xf98fffff,0x983fffff,0xf90000ff,
    0xffffffff,0x7ffdc01f,0x3fe7ffff,0xffffffff,0xfff55fff,0x7ffdc5ff,
    0xffff11ff,0x7ffec9ff,0xfff50fff,0x7ffdc5ff,0xffff11ff,0x7fffcc5f,
    0xffff14ff,0x7ffff40d,0xffffff70,0x3fffff6b,0x05ffeb81,0x0fff6cc0,
    0x1bfe6000,0x7ff77640,0x3fffa601,0x3ffff65f,0x01ffa81e,0xffffd300,
    0x4c0bffff,0xffffffff,0x3fffffe6,0x5fffffff,0x41dfffb1,0x6c5ffff9,
    0x2a2effff,0x225ffffe,0x7cc6fffd,0x7fe45fff,0x3ff60fff,0x3ff61eff,
    0xfffa82ff,0x3fffa65f,0xffff72ef,0x02ff401b,0x001ffb80,0x7d400000,
    0x04ff801f,0xffa81ff2,0x3f600001,0x85ff9007,0xa97fe098,0x717fa1ff,
    0x7f441dff,0x00bff02f,0x3fea0dfb,0x201ffe85,0xfc84ffff,0x0dfffd07,
    0x2e0dfffd,0x3ff620ff,0x05fe801f,0x003ff700,0xfa800000,0x04ff801f,
    0x3b6a1ff2,0xceeeeeff,0x07fd8001,0x3e00ffb0,0xe87fea5f,0x25ffc85f,
    0x3fa04ffe,0xfe8bbaa6,0xc8fff605,0x7ffc04ff,0x83fe41ff,0x261ffffe,
    0xf706ffff,0x3bffa21f,0x5cbfd000,0xf702deed,0x3bfffb7f,0xeeeedb80,
    0x0ffd4001,0xf9027fc0,0xffffff0f,0x00dfffff,0x6440ffb0,0xeeeefffe,
    0xf52ff40c,0xd82ff43f,0x05ffccff,0xbffd3fec,0xff1027fc,0x00dff31f,
    0xc8dffdff,0xffffd07f,0x37fffecb,0xff98ffb8,0x3fa000df,0xffffff55,
    0x3ffffee0,0xf102ffff,0x003fffff,0xf801ffa8,0x21ff204f,0xffffffeb,
    0xd8002eff,0x3fff207f,0x4fffffff,0x3ff517e4,0xfe881fe4,0x6400efff,
    0x1ffff17f,0xfa807ff1,0x801ffeef,0x23ffccff,0xbbfd07fc,0xbbff13ff,
    0x2ffee0df,0xe8004fff,0xdffff55f,0x3fffee07,0x207ffece,0x01ffffec,
    0x400ffd40,0x1ff204ff,0x0001ffa8,0xd981ff60,0xffffffff,0x7fd4081d,
    0x3fe60041,0xffb800ff,0x9bffffb8,0xffc803ff,0x9ff004ff,0x3f23fff1,
    0xff3bfd07,0xdfb3ff99,0x3ffffee0,0xdfe8002f,0x400dfffa,0x7dc2fffb,
    0x1ffa801f,0x00ffd400,0xff9027fc,0x000ffd40,0x03fec015,0x20007fd8,
    0x4c001ffa,0xa800ffff,0xffffe9ff,0x8802ffad,0x3e006fff,0x32dff54f,
    0x3abfd07f,0xdfb6ffff,0x3ffffee0,0x7f4002ff,0x7004ffff,0x7ff303ff,
    0x003ff500,0xf801ffa8,0x81ff204f,0xf5001ffa,0x803fec0d,0x2a0007fd,
    0x3a2001ff,0x400effff,0xdcffcff9,0x001ffcff,0x7c005ff5,0x32fff64f,
    0x2abfd07f,0xdfb2ffff,0x2ebffee0,0x3a002fff,0x2004ffff,0xff881ffb,
    0x01ffa803,0x400ffd40,0x1ff204ff,0x9001ffa8,0x03fec0ff,0x20007fd8,
    0x36001ffa,0x05ffccff,0x4fffffc4,0x00fffffa,0x4003ff50,0x3ffe24ff,
    0x4bfd07fd,0x1bf67ffe,0xff30ffdc,0x3ffa001f,0xf7003fff,0x07ff103f,
    0x8003ff50,0xfe801ffa,0xa81bfa06,0xff9001ff,0xd8037ec0,0x3ea0007f,
    0xdff9001f,0x3e027ff4,0x3ffe7fff,0x3fea007f,0x713fe001,0x3a0fffff,
    0xd8ffee5f,0x81ff706f,0xfd003ffb,0x009fff7d,0xff103ff7,0x03ff5007,
    0x801ffa80,0xffb82ffc,0x40bfea04,0x203ff228,0xfb004ff9,0x7fd4000f,
    0x0effa801,0xf80bffa2,0x3ff64fff,0x3fea006f,0x213fe001,0xfd07fffe,
    0x37ec198b,0xfe80ffb8,0x35fe800f,0xfb809fff,0x03ff881f,0x4001ffa8,
    0xf9801ffa,0xffdabeff,0x5f7fc00f,0x93ffedb9,0xf957bfff,0x3f6003ff,
    0x6e5cc007,0x440bcdff,0x220dfffb,0xe80bfffd,0x3fea2fff,0x6e5cc05f,
    0x100bcdff,0x4c59dff7,0xffb87fff,0xfecb83cf,0x3ff6e23f,0x37fea1bd,
    0x5ffc981c,0x0b3bffe6,0x4c5bffb7,0x6543effc,0xccdffdcc,0x6e66542c,
    0x02cccdff,0xfffffff5,0xfff7005f,0x25ffffff,0xfffffffa,0x664c02ff,
    0x3ccccffe,0x3fffffa0,0x7fe44fff,0x3ff65fff,0xffd84fff,0x404fff87,
    0xfffffffd,0xffff705f,0x3ffdc3ff,0x3fffffea,0x3ffffea2,0xfffff72f,
    0x6fffe8bf,0x265ffff8,0x30ffffff,0x7cffffff,0xf32fffff,0xffffffff,
    0x3fe61fff,0xffffffff,0xffd300ff,0x22001dff,0xcffffffd,0x7fff5c40,
    0xfff801df,0xffffffff,0xfffff702,0xfe985dff,0x3faa2eff,0xffc81eff,
    0x403ffd85,0xffffffeb,0xfffd302e,0x88ffd0bf,0x20effffd,0x0effffe8,
    0x7dffffd3,0xc83fffa8,0x3f625fff,0x3f625fff,0x3ff24fff,0x3ff60dff,
    0xffffffff,0x7ffffec4,0x805fffff,0x10000998,0x22001353,0x7fe40019,
    0xdfffffff,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x3333332e,0x997302bc,0x17999999,0x71e6664c,0xc9839999,
    0xcccccccc,0x3332e21c,0xcccccccc,0x9999712c,0x99999999,0x666665c4,
    0x97100abc,0xcb887999,0x66543ccc,0x002ccccc,0x33333326,0xcccb8802,
    0x402bcccc,0xccccccb8,0x26000acc,0x00002acd,0x79100000,0xfffff301,
    0x83dfffff,0xfffffffd,0x3ff64fff,0xfff99fff,0x7ffdc5ff,0x3fffffff,
    0x3ffffff2,0x3fffffff,0xfffffff7,0x5cffffff,0xffffffff,0xfffb82ff,
    0x3fee2fff,0x3fe2ffff,0x07ffffff,0x3fffffe0,0xfffb806f,0xefffffff,
    0xfffff701,0x05ffffff,0xfffffe88,0x00067dc2,0x3a01ba00,0xffd885ff,
    0xffffffff,0x3fffaa1f,0x21efffff,0x365ffffa,0x7dc2ffff,0xffffffff,
    0x3fffa63f,0xffffffff,0xffffd33f,0xffffffff,0x7fffff4c,0x444fffff,
    0x20dffffe,0x0effffe9,0x3ffffff2,0x3ff2004f,0x401fffff,0xffffffe9,
    0x980fffff,0xfffffffe,0x3201ffff,0x41ffffff,0x0003fffe,0xe803ff30,
    0x3fe605ff,0x037fe4c2,0xfb803ff5,0x83ff880f,0x7fd40ffb,0x00ffb80e,
    0xffa87ff1,0xff0ff901,0x2fffb989,0x2e02ffb8,0x37f403ff,0x7fff4000,
    0x3ff7004f,0x205ffda8,0x7dcc0ffb,0x29ff105f,0xffe980c8,0x540002ff,
    0x664c01ff,0x40bfe600,0xff500ffd,0x80ffb803,0xffb83ff8,0x407ffe20,
    0x3fe00ffb,0xb80ffd43,0x504ff87f,0xbff10dff,0x2007ff40,0x4c0006fe,
    0x00ffdcff,0xfd01ffb8,0x807fdc0d,0x5ff507fd,0x7fffd400,0xff50001e,
    0x3e600003,0x03ff702f,0xf7007fea,0x07ff101f,0x3ffe8ff5,0x7cc3fee0,
    0xff507f65,0x27cdba23,0xff9027fc,0x301ffb03,0xdfd009ff,0x9ffc8000,
    0xfb803ffa,0x40dfd01f,0xff880ffb,0x00dff106,0x3bffff70,0x01ffa800,
    0x05ff3000,0x7d40fff1,0x57fdc01f,0x3ffa9999,0x27fec570,0x3267fee0,
    0xaffa807f,0x7c00ffb9,0x0ffe604f,0x3f613fe6,0x06fe801f,0xff37fc00,
    0x83ff700d,0x2e04ffd8,0xec999aff,0x3ff604ff,0xffc88003,0xf5000cff,
    0x2600003f,0x3fae62ff,0x07fea05f,0xffffff70,0x7007ffff,0xff700dff,
    0x801fffff,0xfffffffa,0x1027fc01,0x1ffa09ff,0x3a00bff1,0x3ea0006f,
    0x007ff23f,0xfdb9bff7,0x7dc01dff,0xffffffff,0x3fffe606,0x26005950,
    0x543ffffd,0xeffeeeed,0x8001deee,0xfffffff9,0x3ea00eff,0x7ffdc01f,
    0x3fffffff,0x007ffcc0,0x7fffffdc,0x7ffd400f,0x7c01ffff,0x827fc04f,
    0x3fee2ffb,0x00dfd002,0xf107ff40,0xfffb809f,0x00efffff,0x7fffffdc,
    0xf982ffff,0xfff6ffff,0x7ffec003,0x3ffffa5f,0xffffffff,0xfff98005,
    0x00cfffff,0x2e00ffd4,0xcccccdff,0x3fe203ff,0x7dc04c2f,0x00ffdabf,
    0x7e55ffd4,0x027fc01f,0x3e207ff1,0x801ffa6f,0x09b106fe,0x3fffffe6,
    0x2e00ffff,0xffffffff,0xabffb803,0x2ffffdba,0x7fe5ffe4,0x9801fffd,
    0x542ffffe,0xeffeeeed,0x3aa1deee,0x37fe602e,0x5000accc,0xffb803ff,
    0x203ff880,0x2fd44ffd,0x6fa87fdc,0xf31ffa80,0x3fe06d4f,0x80bfea04,
    0x3ffa9ffc,0x541bfa00,0xfffc80ff,0x3fffffff,0xfc87fee0,0x7fdc02ff,
    0xe8dff500,0xffffd15f,0xfffd9803,0x3fea003f,0x037ff401,0x40005ff3,
    0x7dc01ffa,0x03ff880f,0xff90bff9,0x04c1ff70,0x130ffd40,0x13fe1ff4,
    0x2600ffd8,0x00ffdcff,0x7dc1bfa0,0xeefff81f,0x06ffeeee,0xff507fee,
    0x07fdc03f,0x7fc1ffb0,0x81bffe65,0x0cffffc8,0x00ffd400,0xf9813ff6,
    0x3ea0002f,0x07fdc01f,0x7d41ffc4,0x87fc80ef,0xa8000ffb,0x13fe01ff,
    0x3ff609ff,0xffffe805,0x837f4005,0x7fd41ffb,0x407fee03,0xffa81ffb,
    0x803fee06,0x67ff47fd,0x83efffba,0x01dffffb,0x003ff500,0x7fed4033,
    0x2200bccd,0xcdffdccb,0x3ff6e0bc,0x3bff2a2d,0x666fffc3,0x227feccc,
    0xbcccffeb,0x3ff6e200,0xfccccccd,0x9dff714f,0x1ffffd99,0x3ffff500,
    0xfff99300,0xffb99999,0x33ffae23,0x77fedc43,0x37ff6e23,0x1effe41b,
    0x99bffb71,0x8bfffd99,0xfffffff9,0xfff32fff,0xa80005df,0x4c0001ff,
    0xffffffff,0x3ffff605,0x34ffffff,0x33ffffff,0x43ffffff,0xffffffff,
    0x3ff27fff,0x03ffffff,0x3fffffee,0x4fffffff,0xfffffff7,0x4003dfff,
    0xff006fff,0xffffffff,0x3ee3ffff,0x2e2fffff,0x72ffffff,0x0dffffff,
    0x3ee3fffd,0xffffffff,0xea80ffff,0xdffdffff,0x005fffd0,0x003fe600,
    0xffffb100,0x75405dff,0xffffffff,0xffffb11e,0x6ffffecb,0x3fffffe0,
    0x267fffff,0xeffffffe,0x3fffa601,0xffffffff,0xffffd34f,0x00079dff,
    0x32007ff9,0xffffffff,0xd31fffff,0x4c1dffff,0x30effffe,0x07dffffd,
    0x3a61bff3,0xffffffff,0x54c00bef,0x00faa00a,0x0017a000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x21eeeb88,
    0x003eeec9,0x776d4000,0xceeda81c,0x5bdddb31,0x8ef776d4,0xeeeeeeea,
    0x3b262eee,0x3bb6a2de,0x001e401c,0x44000022,0xffd702fc,0x2a0777df,
    0x0cefffec,0xbffdb300,0xfdb88017,0x6cc00bdf,0x84bcfffe,0xb52eeed9,
    0xed9817df,0x1dffec9e,0x43ffff70,0xb84ffffd,0x8dffbded,0xff02dffc,
    0xfff0dfff,0x3fffadff,0x3fffe7ff,0xffff76ff,0x47ffffff,0x3e7ffffe,
    0x2605ffff,0xbd1000ff,0xffc88000,0x7fff4c5f,0x41ffffff,0xfffffffa,
    0x3f6200ff,0x2fffffff,0xffffff30,0x3fee03ff,0x1fffffff,0x6e7ffff4,
    0x745fffff,0xffffffff,0xffd983ff,0x3fff661f,0x7ffffd44,0xffffffff,
    0xefffb80f,0x3ffae332,0xdfffd52e,0x77fff647,0xffffff72,0x7545ffff,
    0x3ff24fff,0x3fea02ef,0x09ffb001,0x3fffee00,0x3fffe20e,0x2ffffece,
    0x6e6fffcc,0x3f604ffe,0xfffdceff,0x7fffcc0f,0x81fffecd,0xfeccefff,
    0x7ff542ff,0xfedfffff,0x7ffff5c7,0x206ffece,0x9ff01ffa,0xdffffd88,
    0xffcefffe,0x2e3ff703,0x05ff52ff,0xff705ff9,0x2a0ffa87,0xfd883fff,
    0x5fffa9ff,0x3ff51a88,0x7ffe40a6,0x3faa002f,0xffd82eff,0x17ff441c,
    0x06fe8044,0xf901dff7,0x5fffc4bf,0x2a3ffdc0,0x1ff982ff,0x1bffff30,
    0x7ffd40d4,0xa80ffd42,0x09ff01ff,0x7fccbff6,0x260dfb0e,0x2bffd3ff,
    0xff880ffc,0xd887fe86,0x00bffea4,0x9ffffff9,0x3f7bff60,0x543fffdf,
    0x01ffffff,0x1fffff4c,0x2606ff88,0xb95100ff,0x360ffd99,0x99999aff,
    0xff51ffd9,0x117fdc09,0x2157bfff,0x3ffe6019,0x0bfea003,0x7d40ffe2,
    0x209ff01f,0x8bfe66fd,0x37fe07fd,0xfe8ffff9,0xa8bff207,0xffa803ff,
    0xfff7002f,0x7ffdc05f,0x2fffffff,0x3ea7ffe6,0xffd880ff,0x3ea00cff,
    0x4414c03f,0xfffffffc,0xfffff07f,0xffffffff,0x8803ff27,0x7ffe44ff,
    0x4c03ffff,0x7d4003ff,0x04ff881f,0x3fe03ff5,0x7ccdfb04,0xd81ff62f,
    0xcffffcff,0x3fe205ff,0x5003ffa6,0x6c005fff,0x4c04ffff,0xbeffffeb,
    0xb8bffe20,0x3ff226ff,0xfb800dff,0xffb0001f,0xffffffff,0xffffff10,
    0x9fffffff,0x3fe01bfa,0xffffda85,0xff300eff,0x1ffa8005,0xf504ff88,
    0x413fe03f,0x8bfe66fd,0x3fee07fd,0xffffeeff,0x557fe403,0x7fd404ff,
    0xffd1002f,0x900bfffd,0xf907ffff,0x29ffb09f,0x004ffffe,0x20013fe6,
    0x261beffb,0x5ffd07fd,0x55555555,0x80bfee35,0x10a23ffa,0x209ffb73,
    0x54002ff9,0x4ff881ff,0xf705ff50,0x99bf609f,0x81ff62ff,0xfbbffff9,
    0x7c401fff,0x200ffeef,0x7dc2fffa,0x477ff442,0x500effe8,0x83fffdff,
    0x8ff605fc,0x01effffb,0x200fff88,0x03ff62c8,0x7e41ffa6,0x7fcc004f,
    0x1ffe880e,0x6fe80ffa,0x8005ff30,0xff881ffa,0x55dff104,0x05dffff7,
    0x17fccdfb,0xfff80ffb,0x07fff88f,0x09ffff90,0x6677ffd4,0x3ee5fecc,
    0x3fee2fff,0x9ffe83ef,0x80085ffa,0xffffc880,0x7ffe400d,0xffca99ab,
    0x26b3fee6,0x2cffffdb,0x5677ffc4,0x21ffecb9,0xdaadfffd,0x7ffc5fff,
    0xffdba9ad,0x3ff72e24,0x501bcccd,0x5c45bffb,0xffb02eff,0xffffffff,
    0x9fffdc1f,0xffb1dff9,0x97ff6017,0xf1005ffd,0x7ec01fff,0xffffffff,
    0xfffff56f,0x7fffffcf,0xdf91ffc1,0xfb100000,0x6c019fff,0xffffffff,
    0x7fff42ff,0xffffffff,0x7fffd40f,0x3fffffff,0x7fffffec,0xffff84ff,
    0x0fffffff,0x3fffffee,0xf986ffff,0xf90fffff,0x3601ffff,0xffceffff,
    0x3fffea5f,0xb6fff9af,0x3ee07fff,0x00ffe62f,0x6c04ffd8,0xffffffff,
    0xfffb16ff,0x7fffe49f,0x04c0c40d,0xfd300000,0x7dc07fff,0x2effffff,
    0xfffffc98,0x104fffed,0xfffffffb,0x3ffae07b,0xbf703eff,0x1bffffff,
    0xfffffe98,0xfb03efff,0x7f4cbfff,0x554404ff,0xfffd8801,0xb3fff98d,
    0x00001dff,0x00000000,0x00000000,0xfd500000,0x54c03dff,0x530009aa,
    0x54c00013,0x2a20009a,0x2a620009,0x0000009a,0x00000000,0xcb800000,
    0x4413313c,0xaaaaaaaa,0x3ee0aaaa,0x407ffe5f,0x002e2000,0xdddddda8,
    0x0002dddd,0x0bfff700,0x00000000,0x00000000,0x00000000,0x00000000,
    0x7ffc0000,0x3e67ffb1,0xffffffff,0xf51fffff,0x207ff49f,0x1983ffd9,
    0x3aa03bea,0xfffff72e,0x007fffff,0x17644000,0x00000000,0x00000000,
    0x00000000,0x00000000,0xff980000,0x7ccbff25,0xffffffff,0xf30fffff,
    0x2e3fec7f,0x50dfffff,0x0ffdc1ff,0x6655bffa,0xcccccccc,0x00000002,
    0x00000000,0x00000000,0x00000000,0x00000000,0x7fdc0000,0x9887fea1,
    0x99999999,0x3fe20999,0xff89bf22,0xfbefffbf,0x21ff987f,0x00004ffc,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x4c5fe800,
    0x400000ff,0x897ea1ff,0xfffb11ff,0x032205ff,0xcccc9833,0xcccccccc,
    0x0001cccc,0x00000000,0x00000000,0x00000000,0x00000000,0x44000000,
    0x23fc41ff,0xaaaaaaa8,0x740aaaaa,0x1113e60f,0x003dff90,0x3ffffa00,
    0xffffffff,0x0006ffff,0x00000000,0x00000000,0x00000000,0x00000000,
    0x54000000,0x7ccff04f,0xffffffff,0x321fffff,0x20017c46,0x75c00000,
    0xffffffff,0x3effffff,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x5e807dc0,0x3fffffe6,0xffffffff,0x00004040,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x0d401000,
    0x26666662,0x00199999,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
};

static signed short stb__courier_bold_30_usascii_x[95]={ 0,5,3,1,2,2,2,6,6,3,2,1,5,2,
5,2,2,2,1,1,1,1,2,1,2,2,5,5,-1,0,1,2,2,-1,0,0,0,0,1,0,0,2,1,0,
0,-1,-1,0,1,0,0,1,1,0,-1,-1,0,0,1,6,2,3,2,-2,5,1,-1,1,0,0,1,0,0,1,
2,0,1,-1,0,1,-1,0,1,1,1,0,0,-1,0,0,2,4,6,4,1, };
static signed short stb__courier_bold_30_usascii_y[95]={ 22,4,6,3,3,5,7,5,4,4,5,6,18,13,
18,3,4,4,4,4,5,5,4,5,4,4,10,10,6,10,6,5,4,6,6,5,6,6,6,5,6,6,6,6,
6,6,6,5,6,5,6,5,6,6,6,6,6,6,6,5,3,5,4,27,4,9,5,9,5,9,5,9,5,5,
5,5,5,9,9,9,9,9,9,9,6,10,10,10,10,10,10,5,5,5,11, };
static unsigned short stb__courier_bold_30_usascii_w[95]={ 0,5,10,14,12,12,13,4,7,7,12,14,6,12,
5,12,12,12,13,14,13,14,13,13,12,13,5,7,16,16,16,12,12,18,16,16,15,15,15,16,16,12,16,17,
16,18,17,16,15,16,17,14,14,16,18,18,16,16,14,7,12,7,12,20,6,15,17,15,17,15,15,16,16,14,
11,16,14,18,16,14,17,17,15,14,15,16,16,17,16,16,12,8,4,8,14, };
static unsigned short stb__courier_bold_30_usascii_h[95]={ 0,19,8,22,23,18,16,9,23,23,12,16,9,3,
5,23,19,18,18,19,17,18,19,18,19,19,13,15,16,9,16,18,21,16,16,18,16,16,16,18,16,16,17,16,
16,16,16,18,16,21,16,18,16,17,16,16,16,16,16,22,22,22,10,3,5,14,18,14,18,14,17,19,17,17,
23,17,17,13,13,14,19,19,13,14,17,13,12,12,12,18,12,22,22,22,7, };
static unsigned short stb__courier_bold_30_usascii_s[95]={ 252,233,194,86,34,98,200,172,26,47,230,
231,165,233,227,13,1,144,157,14,230,185,152,171,239,201,246,244,1,177,214,
217,139,146,183,63,94,78,62,200,30,17,98,236,129,217,199,46,1,122,165,
129,131,65,110,163,146,182,47,114,101,64,243,233,220,34,111,18,80,50,115,
184,18,35,1,1,50,146,112,66,166,215,96,81,82,129,183,165,213,29,200,
72,81,55,205, };
static unsigned short stb__courier_bold_30_usascii_t[95]={ 1,1,93,1,1,25,63,93,1,1,80,
63,93,93,93,1,25,25,25,25,25,25,1,25,1,1,63,25,80,93,63,
25,1,63,63,25,63,63,63,25,63,63,45,45,63,45,45,25,63,1,63,
25,45,45,63,45,45,45,63,1,1,1,80,97,93,80,25,80,25,80,45,
1,45,45,1,45,45,80,80,80,1,1,80,80,45,80,80,80,80,25,80,
1,1,1,93, };
static unsigned short stb__courier_bold_30_usascii_a[95]={ 254,254,254,254,254,254,254,254,
254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,
254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,
254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,
254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,
254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,
254,254,254,254,254,254,254, };

// Call this function with
//    font: NULL or array length
//    data: NULL or specified size
//    height: STB_FONT_courier_bold_30_usascii_BITMAP_HEIGHT or STB_FONT_courier_bold_30_usascii_BITMAP_HEIGHT_POW2
//    return value: spacing between lines
static void stb_font_courier_bold_30_usascii(stb_fontchar font[STB_FONT_courier_bold_30_usascii_NUM_CHARS],
                unsigned char data[STB_FONT_courier_bold_30_usascii_BITMAP_HEIGHT][STB_FONT_courier_bold_30_usascii_BITMAP_WIDTH],
                int height)
{
    int i,j;
    if (data != 0) {
        unsigned int *bits = stb__courier_bold_30_usascii_pixels;
        unsigned int bitpack = *bits++, numbits = 32;
        for (i=0; i < STB_FONT_courier_bold_30_usascii_BITMAP_WIDTH*height; ++i)
            data[0][i] = 0;  // zero entire bitmap
        for (j=1; j < STB_FONT_courier_bold_30_usascii_BITMAP_HEIGHT-1; ++j) {
            for (i=1; i < STB_FONT_courier_bold_30_usascii_BITMAP_WIDTH-1; ++i) {
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
        float recip_width = 1.0f / STB_FONT_courier_bold_30_usascii_BITMAP_WIDTH;
        float recip_height = 1.0f / height;
        for (i=0; i < STB_FONT_courier_bold_30_usascii_NUM_CHARS; ++i) {
            // pad characters so they bilerp from empty space around each character
            font[i].s0 = (stb__courier_bold_30_usascii_s[i]) * recip_width;
            font[i].t0 = (stb__courier_bold_30_usascii_t[i]) * recip_height;
            font[i].s1 = (stb__courier_bold_30_usascii_s[i] + stb__courier_bold_30_usascii_w[i]) * recip_width;
            font[i].t1 = (stb__courier_bold_30_usascii_t[i] + stb__courier_bold_30_usascii_h[i]) * recip_height;
            font[i].x0 = stb__courier_bold_30_usascii_x[i];
            font[i].y0 = stb__courier_bold_30_usascii_y[i];
            font[i].x1 = stb__courier_bold_30_usascii_x[i] + stb__courier_bold_30_usascii_w[i];
            font[i].y1 = stb__courier_bold_30_usascii_y[i] + stb__courier_bold_30_usascii_h[i];
            font[i].advance_int = (stb__courier_bold_30_usascii_a[i]+8)>>4;
            font[i].s0f = (stb__courier_bold_30_usascii_s[i] - 0.5f) * recip_width;
            font[i].t0f = (stb__courier_bold_30_usascii_t[i] - 0.5f) * recip_height;
            font[i].s1f = (stb__courier_bold_30_usascii_s[i] + stb__courier_bold_30_usascii_w[i] + 0.5f) * recip_width;
            font[i].t1f = (stb__courier_bold_30_usascii_t[i] + stb__courier_bold_30_usascii_h[i] + 0.5f) * recip_height;
            font[i].x0f = stb__courier_bold_30_usascii_x[i] - 0.5f;
            font[i].y0f = stb__courier_bold_30_usascii_y[i] - 0.5f;
            font[i].x1f = stb__courier_bold_30_usascii_x[i] + stb__courier_bold_30_usascii_w[i] + 0.5f;
            font[i].y1f = stb__courier_bold_30_usascii_y[i] + stb__courier_bold_30_usascii_h[i] + 0.5f;
            font[i].advance = stb__courier_bold_30_usascii_a[i]/16.0f;
        }
    }
}

#ifndef STB_SOMEFONT_CREATE
#define STB_SOMEFONT_CREATE              stb_font_courier_bold_30_usascii
#define STB_SOMEFONT_BITMAP_WIDTH        STB_FONT_courier_bold_30_usascii_BITMAP_WIDTH
#define STB_SOMEFONT_BITMAP_HEIGHT       STB_FONT_courier_bold_30_usascii_BITMAP_HEIGHT
#define STB_SOMEFONT_BITMAP_HEIGHT_POW2  STB_FONT_courier_bold_30_usascii_BITMAP_HEIGHT_POW2
#define STB_SOMEFONT_FIRST_CHAR          STB_FONT_courier_bold_30_usascii_FIRST_CHAR
#define STB_SOMEFONT_NUM_CHARS           STB_FONT_courier_bold_30_usascii_NUM_CHARS
#define STB_SOMEFONT_LINE_SPACING        STB_FONT_courier_bold_30_usascii_LINE_SPACING
#endif
