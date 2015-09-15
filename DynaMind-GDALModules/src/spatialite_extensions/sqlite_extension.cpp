#include "my_extension.h"

#include "../standard/sqlite3ext.h"
#include <QByteArray>
#include <QString>
#include <iostream>
//Methods
#include "cgalgeometryhelper.h"
#include  "my_extension.h"
#include <vector>
#include <sstream>

#include <dmcompilersettings.h>

extern "C"{

SQLITE_EXTENSION_INIT1

/* calculate percentage filled */
void DM_HELPER_DLL_EXPORT poly_percentage_filled(sqlite3_context *context, int argc, sqlite3_value **argv) {
	const unsigned char * geometry = sqlite3_value_text(argv[0]);
	double res = CGALGeometryHelper::percentageFilled(geometry);
	if (res < 0) {
		sqlite3_result_null(context);
		return;
	}
	sqlite3_result_double(context, res);        // save the result
}

/* calculate aspect ratio */
void DM_HELPER_DLL_EXPORT polygon_aspect_ratio(sqlite3_context *context, int argc, sqlite3_value **argv) {
	const unsigned char * geometry = sqlite3_value_text(argv[0]);
	double res = CGALGeometryHelper::aspectRationBB(geometry);
	if (res < 0) {
		sqlite3_result_null(context);
		return;
	}
	sqlite3_result_double(context, res);        // save the result
}


/* multiply vector */
void DM_HELPER_DLL_EXPORT multiply_vector(sqlite3_context *context, int argc, sqlite3_value **argv) {
	const unsigned char * vec = sqlite3_value_text(argv[0]);
	double value = sqlite3_value_double(argv[1]);


	std::string ress = my_extension::multiply_vector(vec, value);
	sqlite3_result_text(context, ress.c_str(),ress.size(),SQLITE_TRANSIENT);
}

/* addition vector */
void DM_HELPER_DLL_EXPORT addition_vector(sqlite3_context *context, int argc, sqlite3_value **argv) {
	const unsigned char * vec1 = sqlite3_value_text(argv[0]);
	const unsigned char * vec2 = sqlite3_value_text(argv[1]);


	std::string ress = my_extension::addition_vector(vec1, vec2);
	sqlite3_result_text(context, ress.c_str(),ress.size(),SQLITE_TRANSIENT);
}

/* vector sum */
void DM_HELPER_DLL_EXPORT vector_sum(sqlite3_context *context, int argc, sqlite3_value **argv) {
	const unsigned char * vec = sqlite3_value_text(argv[0]);
	double ress = my_extension::vector_sum(vec);
	//sqlite3_result_text(context, ress.c_str(),ress.size(),SQLITE_TRANSIENT);
	sqlite3_result_double(context, ress);        // save the result
}


typedef struct SumCtx SumCtx;
struct SumCtx {
	std::vector<double> rSum;      /* Floating point sum */
};

static void sumVecStep(sqlite3_context *context, int argc, sqlite3_value **argv){
	SumCtx *p;

	p = reinterpret_cast<SumCtx*>(sqlite3_aggregate_context(context, sizeof(*p)));
	const unsigned char * vec = sqlite3_value_text(argv[0]);
	if (vec)
		my_extension::vector_addition(p->rSum, vec);

}

static void sumVecFinalize(sqlite3_context *context){
	SumCtx *p;
	p = reinterpret_cast<SumCtx*>(sqlite3_aggregate_context(context, 0));
	if (!p) {
		return;
	}
	std::stringstream ss;
	bool first = true;
	foreach (double s, p->rSum) {
		if (!first)
			ss << " ";
		ss << s;
		first =false;
	}
	std::string ress = ss.str();
	sqlite3_result_text(context, ress.c_str(),ress.size(),SQLITE_TRANSIENT);
}

int DM_HELPER_DLL_EXPORT sqlite3_dmsqliteplugin_init(
		sqlite3 *db,
		char **pzErrMsg,
		const sqlite3_api_routines *pApi
		){
	int rc = SQLITE_OK;
	SQLITE_EXTENSION_INIT2(pApi);


	sqlite3_create_function(db, "dm_poly_percentage_filled", 1, SQLITE_UTF8, 0, &poly_percentage_filled, 0, 0);
	sqlite3_create_function(db, "dm_poly_aspect_ratio", 1, SQLITE_UTF8, 0, &polygon_aspect_ratio, 0, 0);
	sqlite3_create_function(db, "dm_multiply_vector", 2, SQLITE_UTF8, 0, &multiply_vector, 0, 0);
	sqlite3_create_function(db, "dm_vector_addition", 2, SQLITE_UTF8, 0, &multiply_vector, 0, 0);
	sqlite3_create_function(db, "dm_vector_sum", 1, SQLITE_UTF8, 0, &vector_sum, 0, 0);
	sqlite3_create_function(db, "dm_sum_vectors", 1, SQLITE_UTF8, 0, 0, &sumVecStep, &sumVecFinalize);

	return rc;
}

}
