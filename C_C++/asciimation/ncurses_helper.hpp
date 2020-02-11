/**
 * Shamelessly copied (mostly) from HMC CS70
 * */

#include <fstream>

void cleanUpScreenHandler( int );
void cleanup();
void init_ncurses();
void extractLine(std::ifstream& in, char *buf, size_t n);
