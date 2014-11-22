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
//AttrDesc *predattr;
attrCat->getRelInfo(attr->relName, num, arrayofattr);
AttrDesc projnamesdesc[projCnt];
AttrDesc attrpreddesc;
int reclen = 0;
for (int i = 0; i < num; i++){
	if (strcmp(arrayofattr[i].attrName, projNames[i].attrName) == 0){
		projnamesdesc[i] = arrayofattr[i];
		reclen = reclen + arrayofattr[i].attrLen;
	}
	if (strcmp(arrayofattr[i].attrName, attr->attrName) == 0){
		attrpreddesc = arrayofattr[i];
	}
}
for (int i = 0; i < num; i++){
	if (strcmp(arrayofattr[i].attrName, attr->attrName) == 0){
		if (arrayofattr[i].indexed == true && op == EQ){
			Operators::IndexSelect(result, projCnt, projnamesdesc, &attrpreddesc, op,attrValue, reclen);
		}
	}
}
return OK;
}

