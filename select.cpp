#include "catalog.h"
#include "query.h"
#include "index.h"

/*
 * Selects records from the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */
 #include <cstring>
Status Operators::Select(const string & result,      // name of the output relation
	                 const int projCnt,          // number of attributes in the projection
		         const attrInfo projNames[], // the list of projection attributes
		         const attrInfo *attr,       // attribute used inthe selection predicate 
		         const Operator op,         // predicate operation
		         const void *attrValue)     // literal value in the predicate
{

AttrDesc *arrayofattr;
int num = 0;
AttrDesc *predattr;
attrCat->getRelInfo(attr->relName, num, arrayofattr);
for (int i = 0; i < num; i++){
	if (strcmp(arrayofattr[i].attrName, attr->attrName) == 0){
		predattr = &arrayofattr[i];
	}
}
if (predattr->indexed == true && op == EQ){
	//do something
}
else{
	//scan select

	
}
return OK;
}

