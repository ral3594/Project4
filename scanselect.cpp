#include "catalog.h"
#include "query.h"
#include "index.h"
#include <cstring>
#include <cstdlib>
#include "utility.h"
/* 
 * A simple scan select using a heap file scan
 */

Status Operators::ScanSelect(const string& result,       // Name of the output relation
                             const int projCnt,          // Number of attributes in the projection
                             const AttrDesc projNames[], // Projection list (as AttrDesc)
                             const AttrDesc* attrDesc,   // Attribute in the selection predicate
                             const Operator op,          // Predicate operator
                             const void* attrValue,      // Pointer to the literal value in the predicate
                             const int reclen)           // Length of a tuple in the result relation
{
  cout << "Algorithm: File Scan" << endl;
  Status resultstatus;
  HeapFile resultheap(result, resultstatus);
  Status relationstatus;
  if (!attrValue){
    Status heapstatus;
    HeapFileScan relationheap(projNames[0].relName, heapstatus);
    if (heapstatus == OK){
      if (relationheap.startScan(projNames[0].attrOffset, projNames[0].attrLen, static_cast<Datatype>(projNames[0].attrType), NULL, op) == OK){
        Status scanstat;
        RID rid;
        Record rec;
        while ((scanstat = relationheap.scanNext(rid)) == OK){
          relationheap.getRandomRecord(rid, rec);
          //Record newrec;
          //newrec.length = reclen;
          //newrec.data = malloc(reclen);
          //for (int i = 0; i < projCnt; i++){
          //  memcpy((char *)newrec.data + projNames[i].attrOffset, (char *)rec.data + projNames[i].attrOffset, projNames[i].attrLen);
          //}
          //RID newrid;
          resultheap.insertRecord(rec, rid);
          //free(newrec.data);
        }
      }
    }
    relationheap.endScan();
    return OK;
  }
  HeapFileScan relationheap(attrDesc->relName, attrDesc->attrOffset, attrDesc->attrLen, static_cast<Datatype>(attrDesc->attrType),(char *)attrValue, op, relationstatus);
  if (relationstatus == OK && resultstatus == OK){
      if (relationheap.startScan(attrDesc->attrOffset, attrDesc->attrLen, static_cast<Datatype>(attrDesc->attrType), (char *)attrValue, op)  == OK){
      Status scanstat;
      RID rid;
      Record rec;
      while ((scanstat = relationheap.scanNext(rid)) == OK){
        relationheap.getRandomRecord(rid, rec);
        Record newrec;
        newrec.length = reclen;
        newrec.data = malloc(reclen);
        int offset = 0;
        for (int i = 0; i < projCnt; i++){
          memcpy((char *)newrec.data + offset, (char *)rec.data + projNames[i].attrOffset, projNames[i].attrLen);
          offset = offset + projNames[i].attrLen;
        }
        RID newrid;
        resultheap.insertRecord(newrec, newrid);
        free(newrec.data);
      }
      relationheap.endScan();
    }
  }
  return OK;
}
