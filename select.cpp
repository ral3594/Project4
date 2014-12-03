#include "catalog.h"
#include "query.h"
#include "index.h"
#include "utility.h"
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

AttrDesc arrayofattr[projCnt];

//int num = 0;
for (int i = 0; i < projCnt; i++){
	attrCat->getInfo(projNames[i].relName, projNames[i].attrName,arrayofattr[i]);
}

AttrDesc attrpreddesc;
if (attrValue){
	attrCat->getInfo(attr->relName, attr->attrName, attrpreddesc);
}
//sums up reclen
int reclen = 0;
for (int i = 0; i < projCnt; i++){
	reclen = reclen + arrayofattr[i].attrLen;
}

//for (int i = 0; i < projCnt; i++){
	//if ((strcmp(arrayofattr[i].attrName, attr->attrName) == 0) && (arrayofattr[i].indexed == true) &&(op == EQ)){
		//if (arrayofattr[i].indexed == true && op == EQ){
		if ((attrpreddesc.indexed == true) && (op == EQ)){
			Operators::IndexSelect(result, projCnt, arrayofattr, &attrpreddesc, op,attrValue, reclen);
		// }
		// else{
			// Operators::ScanSelect(result, projCnt, arrayofattr, &attrpreddesc, op, attrValue, reclen);
		// }
	}
	else{
		//if (strcmp(arrayofattr[i].attrName, attr->attrName) == 0){
			Operators::ScanSelect(result, projCnt, arrayofattr, &attrpreddesc, op, attrValue, reclen);
		//}
	}
//}
//print table
return OK;
}

