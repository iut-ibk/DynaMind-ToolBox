#include "my_extension.h"

extern "C"{

/* Add your header comment here */
#include <sqlite3ext.h> /* Do not use <sqlite3.h>! */
#include <math.h>
SQLITE_EXTENSION_INIT1

/* Insert your extension code here */

#ifdef _WIN32
__declspec(dllexport)
#endif
/* TODO: Change the entry point name so that "extension" is replaced by
** text derived from the shared library filename as follows:  Copy every
** ASCII alphabetic character from the filename after the last "/" through
** the next following ".", converting each character to lowercase, and
** discarding the first three characters if they are "lib".
*/

void sqlite_power(sqlite3_context *context, int argc, sqlite3_value **argv) {
	double num = sqlite3_value_double(argv[0]); // get the first arg to the function
	double exp = sqlite3_value_double(argv[1]); // get the second arg
	//double res = -4                 // calculate the result
	double res = my_extension::give_me_stuff();
	sqlite3_result_double(context, res);        // save the result
}

int sqlite3_mysqliteplugin_init(
  sqlite3 *db,
  char **pzErrMsg,
  const sqlite3_api_routines *pApi
){
  int rc = SQLITE_OK;
  SQLITE_EXTENSION_INIT2(pApi);
  /* Insert here calls to
  **     sqlite3_create_function_v2(),
  **     sqlite3_create_collation_v2(),
  **     sqlite3_create_module_v2(), and/or
  **     sqlite3_vfs_register()
  ** to register the new features that your extension adds.
  */
//  sqlite3_create_function_v2(db, "huhu", 1, SQLITE_UTF8, 0,
//							 sqlEvalFunc, 0, 0);
  sqlite3_create_function(db, "MY_POWER_FUNC", 2, SQLITE_UTF8, 0, &sqlite_power, 0, 0);
  return rc;
}

}
