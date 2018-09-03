#include "my_extension.h"

#include "../../3rdparty/sqlite3/sqlite3ext.h"
SQLITE_EXTENSION_INIT1

#include <iostream>
//Methods
//#include "cgalgeometryhelper.h"
#include  "my_extension.h"
#include <vector>
#include <sstream>

#include <dmcompilersettings.h>

extern "C"{



void BinaryToDoubleVector(sqlite3_value * blob, std::vector<double> & return_vec) {

	int size = sqlite3_value_bytes(blob);
	const double * data = static_cast<const double*>( sqlite3_value_blob(blob));
	return_vec = std::vector<double>(data, data + size/8);
}

void DoubleToSQLITE3(sqlite3_context *context, const std::vector<double> & vec) {

	std::stringbuf bytebuffer;
	for (size_t i = 0; i < vec.size(); i++) {
		bytebuffer.sputn(reinterpret_cast<const char *>(&vec[i]), 8);
	}

	sqlite3_result_blob(context,bytebuffer.str().c_str(), vec.size()*8,SQLITE_TRANSIENT);
}

/* calculate percentage filled */
/*void DM_HELPER_DLL_EXPORT poly_percentage_filled(sqlite3_context *context, int argc, sqlite3_value **argv) {
	const unsigned char * geometry = sqlite3_value_text(argv[0]);
	double res = CGALGeometryHelper::percentageFilled(geometry);
	if (res < 0) {
		sqlite3_result_null(context);
		return;
	}
	sqlite3_result_double(context, res);        // save the result
}*/

/* calculate aspect ratio */
/*void DM_HELPER_DLL_EXPORT polygon_aspect_ratio(sqlite3_context *context, int argc, sqlite3_value **argv) {
	const unsigned char * geometry = sqlite3_value_text(argv[0]);
	double res = CGALGeometryHelper::aspectRationBB(geometry);
	if (res < 0) {
		sqlite3_result_null(context);
		return;
	}
	sqlite3_result_double(context, res);        // save the result
}*/


/* multiply vector */
void DM_HELPER_DLL_EXPORT multiply_vector(sqlite3_context *context, int argc, sqlite3_value **argv) {
	if( sqlite3_value_type(argv[0])==SQLITE_NULL){
		sqlite3_result_null(context);
		return;
	}

	std::vector<double> vec;
	BinaryToDoubleVector(argv[0], vec);
	double value = 0;
	if( sqlite3_value_type(argv[1])!=SQLITE_NULL){
		value  = sqlite3_value_double(argv[1]);
	}

	my_extension::multiply_vector(vec, value, vec);

	DoubleToSQLITE3(context, vec);
}

/* addition vector */
void DM_HELPER_DLL_EXPORT addition_vector(sqlite3_context *context, int argc, sqlite3_value **argv) {
	if( sqlite3_value_type(argv[0])==SQLITE_NULL || sqlite3_value_type(argv[0])==SQLITE_NULL){
		sqlite3_result_null(context);
		return;
	}
	std::vector<double> vec1, vec2;
	BinaryToDoubleVector(argv[0], vec1);
	BinaryToDoubleVector(argv[1], vec2);

	my_extension::addition_vector(vec1, vec2, vec1);
	DoubleToSQLITE3(context, vec1);

}

/* multiply vector */
void DM_HELPER_DLL_EXPORT vector_multiply(sqlite3_context *context, int argc, sqlite3_value **argv) {
	if( sqlite3_value_type(argv[0])==SQLITE_NULL || sqlite3_value_type(argv[0])==SQLITE_NULL){
		sqlite3_result_null(context);
		return;
	}
	std::vector<double> vec1, vec2;
	BinaryToDoubleVector(argv[0], vec1);
	BinaryToDoubleVector(argv[1], vec2);

	my_extension::vector_multiply(vec1, vec2, vec1);
	DoubleToSQLITE3(context, vec1);

}

/* vector sum*/
void DM_HELPER_DLL_EXPORT vector_sum(sqlite3_context *context, int argc, sqlite3_value **argv) {
	if( sqlite3_value_type(argv[0])==SQLITE_NULL){
		sqlite3_result_null(context);
		return;
	}

	std::vector<double> data;
	BinaryToDoubleVector(argv[0], data);
	double ress = my_extension::vector_sum(data);

	sqlite3_result_double(context, ress);        // save the result
}

/* string*/
void DM_HELPER_DLL_EXPORT vector_to_string(sqlite3_context *context, int argc, sqlite3_value **argv) {
	if( sqlite3_value_type(argv[0])==SQLITE_NULL){
		sqlite3_result_null(context);
		return;
	}

	std::vector<double> data;
	BinaryToDoubleVector(argv[0], data);
	std::string ress = my_extension::dm_vector_to_string(data);
	sqlite3_result_text(context, ress.c_str(),ress.size(),SQLITE_TRANSIENT);
}

typedef struct SumCtx SumCtx;
struct SumCtx {
	std::vector<double> rSum;      /* Floating point sum */
    bool initalised;
    SumCtx() : initalised(false)
    {
    }
};

static void sumVecStep(sqlite3_context *context, int argc, sqlite3_value **argv){
	SumCtx *p;
	p = reinterpret_cast<SumCtx*>(sqlite3_aggregate_context(context, sizeof(*p)));
    if (sqlite3_value_type(argv[0])==SQLITE_NULL) {
		return;
    }
    if (!p) {
        return;
    }

	std::vector<double> vec;
	BinaryToDoubleVector(argv[0], vec);
    if (!p->initalised) {
        for (int i = 0; i < vec.size(); i++) {
            p->rSum.push_back(0.0);
        }
        p->initalised = true;
    }
	my_extension::vector_addition(p->rSum, vec);

}

static void sumVecFinalize(sqlite3_context *context){
	SumCtx *p;
	p = reinterpret_cast<SumCtx*>(sqlite3_aggregate_context(context, 0));
	if (!p) {
		return;
	}
	DoubleToSQLITE3(context, p->rSum);
}

int DM_HELPER_DLL_EXPORT sqlite3_dmsqliteplugin_init(
		sqlite3 *db,
		char **pzErrMsg,
		const sqlite3_api_routines *pApi
		){
	int rc = SQLITE_OK;

    SQLITE_EXTENSION_INIT2(pApi);

	sqlite3_create_function(db, "dm_multiply_vector", 2, SQLITE_UTF8, 0, &multiply_vector, 0, 0);
	sqlite3_create_function(db, "dm_vector_addition", 2, SQLITE_UTF8, 0, &addition_vector, 0, 0);
	sqlite3_create_function(db, "dm_vector_sum", 1, SQLITE_UTF8, 0, &vector_sum, 0, 0);
	sqlite3_create_function(db, "dm_vector_to_string", 1, SQLITE_UTF8, 0, &vector_to_string, 0, 0);
	sqlite3_create_function(db, "dm_vector_multiply", 1, SQLITE_UTF8, 0, &vector_multiply, 0, 0);

    sqlite3_create_function( db, "dm_sum_vectors", 1, SQLITE_UTF8,
                NULL, NULL, &sumVecStep, &sumVecFinalize );



	return rc;
}

}
