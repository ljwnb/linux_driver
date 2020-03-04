#ifndef __DIR_H__
#define __DIR_H__

#include "list.h"

int read_dir(PT_List list, PT_List music, const char *pathname);

int get_file_type(char *file_name);
int is_point_dir(const char *path);


#endif /* __DIR_H__ */
