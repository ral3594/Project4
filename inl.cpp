#include "catalog.h"
#include "query.h"
#include "sort.h"
#include "index.h"
#include <cstdlib>
#include <cstring>
/* 
 * Indexed nested loop evaluates joins with an index on the 
 * inner/right relation (attrDesc2)
 */

Status Operators::INL(const string& result,           // Name of the output relation
                      const int projCnt,              // Number of attributes in the projection
                      const AttrDesc attrDescArray[], // The projection list (as AttrDesc)
                      const AttrDesc& attrDesc1,      // The left attribute in the join predicate
                      const Operator op,              // Predicate operator
                      const AttrDesc& attrDesc2,      // The left attribute in the join predicate
                      const int reclen)               // Length of a tuple in the output relation
{
  cout << "Algorithm: Indexed NL Join" << endl;
    Status resultstatus;
    HeapFile resultheap(result, resultstatus);
    // AttrDesc *attr1array;
    // AttrDesc *attr2array;
    // int attr1size = 0;
    // int attr2size = 0;
    Status heapfilestatusa1;
    Status heapfilestatusa2;
    Status IndStatus;
    if (attrDesc2.indexed == true){
      Index attr2index(attrDesc2.relName, attrDesc2.attrOffset, attrDesc2.attrLen, static_cast<Datatype>(attrDesc2.attrType), 0, IndStatus);
      HeapFileScan attr1hfscan(attrDesc1.relName, attrDesc1.attrOffset, attrDesc1.attrLen, static_cast<Datatype>(attrDesc1.attrType),NULL, op, heapfilestatusa1);
      HeapFileScan attr2hfscan(attrDesc2.relName, attrDesc2.attrOffset, attrDesc2.attrLen, static_cast<Datatype>(attrDesc2.attrType), NULL, op, heapfilestatusa2);
      if (heapfilestatusa1 == OK && IndStatus == OK && heapfilestatusa2 == OK){
        Status scanstat;
        RID ridattr1;
        Record recattr1;
        while ((scanstat = attr1hfscan.scanNext(ridattr1,recattr1)) == OK){
          char *attr1val = new char [attrDesc1.attrLen];
          memcpy(attr1val, (char*)recattr1.data + attrDesc1.attrOffset, attrDesc1.attrLen);
          Status indscanstat;
          indscanstat = attr2index.startScan(attr1val);
          RID ridattr2;
          Record recattr2;
          while ((indscanstat = attr2index.scanNext(ridattr2)) ==  OK){
            attr2hfscan.getRandomRecord(ridattr2, recattr2);
            RID newrid;
            Record newrec;
            newrec.data = malloc(reclen);
            newrec.length = reclen;
            int location = 0;
            for (int i = 0; i < projCnt; i++){
              if (strcmp(attrDesc1.relName, attrDescArray[i].relName) == 0){
                memcpy((char *)newrec.data + location, (char *)recattr1.data + attrDescArray[i].attrOffset, attrDescArray[i].attrLen);
                location = location + attrDescArray[i].attrLen;
              }
              else if (strcmp(attrDesc2.relName, attrDescArray[i].relName) == 0){
                memcpy((char *)newrec.data + location, (char *)recattr2.data + attrDescArray[i].attrOffset, attrDescArray[i].attrLen);
                location = location + attrDescArray[i].attrLen;
              }
              else{
                continue;
              }
            }
            free(newrec.data);
          }
        }
        attr2index.endScan();
      }
    }
    else{
      Index attr1index(attrDesc1.relName, attrDesc1.attrOffset, attrDesc1.attrLen, static_cast<Datatype>(attrDesc1.attrType), 0, IndStatus);
      HeapFileScan attr1hfscan(attrDesc1.relName, attrDesc1.attrOffset, attrDesc1.attrLen, static_cast<Datatype>(attrDesc1.attrType),NULL, op, heapfilestatusa1);
      HeapFileScan attr2hfscan(attrDesc2.relName, attrDesc2.attrOffset, attrDesc2.attrLen, static_cast<Datatype>(attrDesc2.attrType), NULL, op, heapfilestatusa2);
      if (heapfilestatusa1 == OK && IndStatus == OK && heapfilestatusa2 == OK){
        Status scanstat;
        RID ridattr2;
        Record recattr2;
        while ((scanstat = attr2hfscan.scanNext(ridattr2,recattr2)) == OK){
          //char *attr2val = new char [attrDesc2.attrLen];
          //char attr2val[recattr2.length];
          void *attr2val;
          attr2val = malloc(attrDesc2.attrLen);
          memcpy(attr2val, (char*)recattr2.data + attrDesc2.attrOffset, attrDesc2.attrLen);//issue is not getting 5 in attr2val...should be though
          Status indscanstat;
          indscanstat = attr1index.startScan(attr2val);//filtering for something incorrect so never finds that..currently getting /006..not an id
          RID ridattr1;
          Record recattr1;
          while ((indscanstat = attr1index.scanNext(ridattr1)) ==  OK){//can't get this to work because attr2val is not right
            attr1hfscan.getRandomRecord(ridattr1, recattr1);
            RID newrid;
            Record newrec;
            newrec.data = malloc(reclen);
            newrec.length = reclen;
            int location = 0;
            for (int i = 0; i < projCnt; i++){
              if (strcmp(attrDesc1.relName, attrDescArray[i].relName) == 0){
                memcpy((char *)newrec.data + location, (char *)recattr1.data + attrDescArray[i].attrOffset, attrDescArray[i].attrLen);
                location = location + attrDescArray[i].attrLen;
              }
              else if (strcmp(attrDesc2.relName, attrDescArray[i].relName) == 0){
                memcpy((char *)newrec.data + location, (char *)recattr2.data + attrDescArray[i].attrOffset, attrDescArray[i].attrLen);
                location = location + attrDescArray[i].attrLen;
              }
              else{
                continue;
              }
            }
            free(newrec.data);
          }
        }
        attr1index.endScan();
      }
    }

  return OK;
}

