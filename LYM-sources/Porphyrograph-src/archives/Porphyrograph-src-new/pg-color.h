////////////////////////////////////////////
// COLOR PRESETS
// pen palettes presets
class Color {
public:
	float color;
	float grey;
	float alpha;
	Color() {
		color = 0.f;
		grey = 0.f;
		alpha = 1.f;
	}
	Color(float c, float g, float a) {
		color = c;
		grey = g;
		alpha = a;
	}
	~Color() {
	}
	Color operator*(float x) const
	{
		return Color(color * x, grey * x, alpha * x);
	}
	Color operator+(const Color c) const
	{
		return Color(color + c.color, grey + c.grey, alpha + c.alpha);
	}
	void print(void) {
		printf("Color %.2f, Color %.2f, Color %.2f\n", color, grey, alpha);
	}
};
class ColorPreset {
public:
	string pen_colorPresets_names;
	Color pg_colorPreset_values;
	ColorPreset(string &id, Color &color) {
		pen_colorPresets_names = id;
		pg_colorPreset_values = color;
	}
	~ColorPreset(void) {
	}
};
extern vector<ColorPreset> pg_ColorPresets[PG_MAX_CONFIGURATIONS];

////////////////////////////////////////////
// COLOR PALETTES
// pen palettes colors
class Palette {
public:
	string pen_palette_colors_names;
	array<float, 9> pen_palette_colors_values;
	Palette(string &id, array<float, 9> &colors) {
		pen_palette_colors_names = id;
		pen_palette_colors_values = colors;
	}
	~Palette(void) {
	}
};
extern vector<Palette> pg_Palettes[PG_MAX_CONFIGURATIONS];

extern float light_color_prec[PG_NB_LIGHTS + 1];
extern float light_grey_prec[PG_NB_LIGHTS + 1];
extern float light_dimmer_prec[PG_NB_LIGHTS + 1];
extern float light_strobe_prec[PG_NB_LIGHTS + 1];
// addition of color for light based on palette
extern float light_color_pulse_prec[PG_NB_LIGHTS + 1];
extern float light_grey_pulse_prec[PG_NB_LIGHTS + 1];
extern float light_dimmer_pulse_prec[PG_NB_LIGHTS + 1];
extern float light_strobe_pulse_prec[PG_NB_LIGHTS + 1];
