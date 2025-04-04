unsigned char *BmpToTexture(char *filename, int *width, int *height);
int LoadObjFile(char *name);
/**
 * Draws a pill shape
 * @param time The current time
 * @param pill_width The width of the pill
 * @param pill_radius The radius of the pill
 * @param coords Pointer to an array of 2 floats to store the x and y coordinates of the pill
 * @param rotation Pointer to an float to store the rotation of the pill
 */
void pill_path(float time, float pill_width, float pill_radius, float coords[2], float *rotation);
void SetPointLight(int ilight, float x, float y, float z, float r, float g, float b);
void SetSpotLight(int ilight, float x, float y, float z, float xdir, float ydir, float zdir, float r, float g, float b);