#ifndef __XMSQLPUBLIC_H__
#define __XMSQLPUBLIC_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef XMSQL_SQLITE
#include "sqlite/sqlite3.h"
#else
#include "mysql/mysql.h"
#endif
#include <pthread.h>

#endif