bool pg_ReadAllDisplayMessages(string basefilename);
int pg_displayMessage_update(int indMesg);
void pg_screenMessage_update(void);
void pg_writeMessageOnScreen(string text);
void pg_loadScreenMessageTexture(void);
void pg_bindFontTextureID(void);
void pg_bindMessageTextureID(void);
void pg_init_texDataScreenFont(void);
void pg_loadScreenFontTexture(void);