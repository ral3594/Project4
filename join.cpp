#include "catalog.h"
#include "query.h"
#include "sort.h"
#include "index.h"
#include <cmath>
#include <cstring>
#include <cstdlib>
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define DOUBLEERROR 1e-07

/*
 * Joins two relations
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

Status Operators::Join(const string& result,           // Name of the output relation 
                       const int projCnt,              // Number of attributes in the projection
    	               const attrInfo projNames[],     // List of projection attributes
    	               const attrInfo* attr1,          // Left attr in the join predicate
    	               const Operator op,              // Predicate operator
    	               const attrInfo* attr2)          // Right attr in the join predicate
{
    AttrDesc *arrayofattr1;
    AttrDesc *arrayofattr2;
    //AttrDesc arrayofattrdesc[projCnt];
    int num1 = 0;
    int num2 = 0;
    attrCat->getRelInfo(attr1->relName, num1, arrayofattr1);
    attrCat->getRelInfo(attr2->relName, num2, arrayofattr2);
    AttrDesc projattrdesc[projCnt];
    AttrDesc attrdesc2;
    AttrDesc attrdesc1;
    int reclen = 0;
    attrCat->getInfo(attr1->relName, attr1->attrName, attrdesc1);
    attrCat->getInfo(attr2->relName, attr2->attrName, attrdesc2);
    /*for (int i = 0; i < projCnt; i++){
        if ((strcmp(arrayofattr1[i].attrName, projNames[i].attrName) == 0) && (strcmp(arrayofattr1[i].relName, projNames[i].relName) == 0)){
            projattrdesc[i] = arrayofattr1[i];
            reclen = reclen + arrayofattr1[i].attrLen;
            if ((strcmp(attr1->attrName, arrayofattr1[i].attrName) == 0) && (strcmp(arrayofattr1[i].relName, attr1->relName) == 0)){
                attrdesc1 = arrayofattr1[i];
            }
            if ((strcmp(attr2->attrName, arrayofattr1[i].attrName) == 0) && (strcmp(arrayofattr1[i].relName, attr2->relName) == 0)){
                attrdesc2 = arrayofattr1[i];
            }
        }
         if ((strcmp(arrayofattr2[i].attrName, projNames[i].attrName) == 0) && (strcmp(arrayofattr2[i].relName, projNames[i].relName) == 0)){
             projattrdesc[i] = arrayofattr2[i];
             reclen = reclen + arrayofattr2[i].attrLen;
             if ((strcmp(attr1->attrName, arrayofattr2[i].attrName) == 0) && (strcmp(arrayofattr2[i].relName, attr1->relName) == 0)){
                attrdesc1 = arrayofattr2[i];
            }
            if ((strcmp(attr2->attrName, arrayofattr2[i].attrName) == 0) && (strcmp(arrayofattr2[i].relName, attr2->relName) == 0)){
                attrdesc2 = arrayofattr2[i];
            }
         }
        if ((strcmp(attr1->attrName, arrayofattr2[i].attrName) == 0) && (strcmp(arrayofattr2[i].relName, attr1->relName) == 0)){
            attrdesc1 = arrayofattr2[i];
        }
        if ((strcmp(attr2->attrName, arrayofattr2[i].attrName) == 0) && (strcmp(arrayofattr2[i].relName, attr2->relName) == 0)){
            attrdesc2 = arrayofattr2[i];
        }
        if ((strcmp(attr1->attrName, arrayofattr1[i].attrName) == 0) && (strcmp(arrayofattr1[i].relName, attr1->relName) == 0)){
            attrdesc1 = arrayofattr1[i];
        }
        if ((strcmp(attr2->attrName, arrayofattr1[i].attrName) == 0) && (strcmp(arrayofattr1[i].relName, attr2->relName) == 0)){
            attrdesc2 = arrayofattr1[i];
        }
    }*/
    for (int i = 0; i < projCnt; i++){
	    attrCat->getInfo(projNames[i].relName, projNames[i].attrName,projattrdesc[i]);
    }
    for (int i = 0; i < projCnt; i++){
    	reclen = reclen + projattrdesc[i].attrLen;
    }
    
    bool atloneindex = 0;
    for (int i = 0;i < num1 ; i++){
        if ((strcmp(arrayofattr1[i].attrName, attr1->attrName) == 0) && (arrayofattr1[i].indexed == true) && (op == EQ)){
            atloneindex = true;
        }
    }
    for (int i = 0; i < num2; i++){
        if ((strcmp(arrayofattr2[i].attrName, attr2->attrName) == 0) && (arrayofattr2[i].indexed == true) && (op == EQ)){
            atloneindex = true;
        }
    }
    if (atloneindex == true){
        Operators::INL(result, projCnt, projattrdesc, attrdesc1, op, attrdesc2, reclen);
    }
	return OK;
}

// Function to compare two record based on the predicate. Returns 0 if the two attributes 
// are equal, a negative number if the left (attrDesc1) attribute is less that the right 
// attribute, otherwise this function returns a positive number.
int Operators::matchRec(const Record& outerRec,     // Left record
                        const Record& innerRec,     // Right record
                        const AttrDesc& attrDesc1,  // Left attribute in the predicate
                        const AttrDesc& attrDesc2)  // Right attribute in the predicate
{
    int tmpInt1, tmpInt2;
    double tmpFloat1, tmpFloat2, floatDiff;

    // Compare the attribute values using memcpy to avoid byte alignment issues
    switch(attrDesc1.attrType)
    {
        case INTEGER:
            memcpy(&tmpInt1, (char *) outerRec.data + attrDesc1.attrOffset, sizeof(int));
            memcpy(&tmpInt2, (char *) innerRec.data + attrDesc2.attrOffset, sizeof(int));
            return tmpInt1 - tmpInt2;

        case DOUBLE:
            memcpy(&tmpFloat1, (char *) outerRec.data + attrDesc1.attrOffset, sizeof(double));
            memcpy(&tmpFloat2, (char *) innerRec.data + attrDesc2.attrOffset, sizeof(double));
            floatDiff = tmpFloat1 - tmpFloat2;
            return (fabs(floatDiff) < DOUBLEERROR) ? 0 : (floatDiff < 0?floor(floatDiff):ceil(floatDiff));

        case STRING:
            return strncmp(
                (char *) outerRec.data + attrDesc1.attrOffset, 
                (char *) innerRec.data + attrDesc2.attrOffset, 
                MAX(attrDesc1.attrLen, attrDesc2.attrLen));
    }

    return 0;
}
