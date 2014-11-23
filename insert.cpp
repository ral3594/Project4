#include "catalog.h"
#include "query.h"
#include "index.h"
#include "utility.h"
#include <cstdlib>
#include <cstring>
/*
 * Inserts a record into the specified relation
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

Status Updates::Insert(const string& relation,      // Name of the relation
                       const int attrCnt,           // Number of attributes specified in INSERT statement
                       const attrInfo attrList[])   // Value of attributes specified in INSERT statement
{
    Status relstatus;
    HeapFile relhf(relation, relstatus);
    if (relstatus != OK){
        return relstatus;
    }
    else{
        AttrDesc *arrayofattr;
        int num = attrCnt;
        attrCat->getRelInfo(relation, num, arrayofattr);
        int sumsizes = 0;
        for (int i = 0; i < num; i++){
            sumsizes = sumsizes + arrayofattr[i].attrLen;
        }
        Record rec;
        rec.length = sumsizes;
        rec.data = (char *)malloc(sumsizes);
        //*(char *)rec.data = 'h';
        //attrdesc tells you where it should be, attrinfo how it was passed to you
        //for loop - use memcpy (rec.data + offset, attrList[i].attrValue, length);
        for (int i = 0; i < num; i++){
           // if (arrayofattr[i].attrName == attrList[i].attrName){
             //   memcpy((char *)rec.data + arrayofattr[i].attrOffset, attrList[i].attrValue, arrayofattr[i].attrLen);
            //}
            //else{
                for (int j = 0; j < num; j++){
                    if (strcmp(arrayofattr[i].attrName, attrList[j].attrName) == 0){
                        memcpy((char *)rec.data + arrayofattr[i].attrOffset, attrList[j].attrValue, arrayofattr[i].attrLen);
                    }
                }
            //}
        }
        RID oRID;
        //*(char *)rec.data = 'h';
        Status checker = relhf.insertRecord(rec, oRID);
        if (checker != OK){
            return relstatus;
        }
        //if there is an index on that attribute then i should insert it into that index -- attr desc for loop
        //insert record first cause need record id..number and type of index 
        //open
        //insert on any index
        //loop through array of attrDesc check if there is an index and then open the file if there is and insert the record
        //check out index.h
        for (int i = 0; i < num; i++){
            if (arrayofattr[i].indexed == true){
                Status indstatus;
                Index ind(relation, arrayofattr[i].attrOffset, arrayofattr[i].attrLen, static_cast<Datatype>(arrayofattr[i].attrType), 0, indstatus);
                if (indstatus == OK){
                    for (int j = 0; j < num; j++){
                        if (strcmp(arrayofattr[i].attrName, attrList[j].attrName) == 0){
                            ind.insertEntry(attrList[j].attrValue, oRID);
                        }
                    }
                }
            }
        }
        Utilities::Print(relation);
    }
    return OK;
}
//char *x = malloc(reclen)
//free(x);
//check dynamic vs. status
//where do i free?