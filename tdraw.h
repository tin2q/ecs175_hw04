/* Functions implemented in draw.h */

void drawScene(void);
int init_resources(void);
void free_resources(void);

void moveCamera(float);
void moveCameraLR(float);
void rotateCamera(float);
void tiltCamera(float);
void resetCamera(void);