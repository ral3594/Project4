#include "catalog.h"
#include "query.h"
#include "index.h"
#include "heapfile.h"
#include <cstring>
#include <cstdlib>
Status Operators::IndexSelect(const string& result,       // Name of the output relation 
                              const int projCnt,          // Number of attributes in the projection
                              const AttrDesc projNames[], // Projection list (as AttrDesc) -- select a, b from c where ___ ..projecting a and b
                              const AttrDesc* attrDesc,   // Attribute in the selection predicate -- where a = 5.. a would be here
                              const Operator op,          // Predicate operator .. = 
                              const void* attrValue,      // Pointer to the literal value in the predicate ... pointer to 5 pass directly to index constructor
                              const int reclen)           // Length of a tuple in the output relation -- length of all of the projection attributes added up get all your lengths from attrDesc array above
{
  cout << "Algorithm: Index Select" << endl;

  /* Your solution goes here */
  //open result heapfile - to put data
  //index holds key and rid pairs
  //i'm looking for a key =  attrvalue passed in, find index with that attribute, find the records that satisfy that
  //tell it to const Status startScan(const void* value); open heapfile also...to get data .. realtion's heapfile - get that from attrDesc they all have relation names
  //check to make sure status is ok
  //then in a loop, call this so you can get each RID where the condition is held:  const Status scanNext(RID& outRid);
  //check if the status is ok
  //Status sccanStat;
  //while ((scanStat = scanNext(RID)) == OK) { keep scanning
    //i'll have an RID how do i get data
    //getRecord(const RID& rid, Record& rec); - in heapfile.h - GET RANDOM RECORD NOT THIS FUNC
    //find where the attributes are using attr desc
    //use the offset of the attr from attrdesc and add it to rec.data
    //make a new chunk of memory...put that chunk into a record and send it off to result heapfile
    //only for attributes i want it to memcpy((char *)rec.data + arrayofattr[i].attrOffset, attrList[j].attrValue, arrayofattr[i].attrLen);
    //put that info into the result 
    //insertRecord(const Record& rec, RID& outRid);
//
  //end scan
  Status resultstatus;
  HeapFile resultheap(result, resultstatus);
  Status Indstatus;
  Index attrind(attrDesc->relName, attrDesc->attrOffset, attrDesc->attrLen, static_cast<Datatype>(attrDesc->attrType), 0, Indstatus);
  if (Indstatus == OK){
    Status relationstatus;
    HeapFileScan relationheap(attrDesc->relName, attrDesc->attrOffset, attrDesc->attrLen, static_cast<Datatype>(attrDesc->attrType),(char *)attrValue, op, relationstatus);

    if (attrind.startScan(attrValue) == OK){
      Status scanstat;
      RID rid;
      Record rec;
      while ((scanstat = attrind.scanNext(rid)) == OK){
        relationheap.getRandomRecord(rid, rec);
        Record newrec;
        newrec.length = reclen;
        newrec.data = malloc(reclen);
        for (int i = 0; i < projCnt; i++){
          memcpy((char *)newrec.data + projNames[i].attrOffset, (char *)rec.data + projNames[i].attrOffset, projNames[i].attrLen);
        }
        RID newrid;
        resultheap.insertRecord(newrec, newrid);
        free(newrec.data);
      }
      attrind.endScan();
      
    }
  }
  
  return OK;
}

