#ifndef LISTEN_JOYSTICK_H_
#define LISTEN_JOYSTICK_H_

// create a thread to listen the input from joystick, and call the functions accordingly.
// create the thread
void Listen_js_init(void);
// clean up the thread
void Listen_js_cleanup(void);
int getDir(void);
// get info whether save pic
bool get_save_pic(void);
// set save_pic to false
void set_save_pic(void);

#endif