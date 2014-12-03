#include <string.h>
#include "catalog.h"
#include "query.h"
#include "sort.h"
#include "index.h"



Status Operators::SNL(const string& result,           // Output relation name
                      const int projCnt,              // Number of attributes in the projection
                      const AttrDesc attrDescArray[], // Projection list (as AttrDesc)
                      const AttrDesc& attrDesc1,      // The left attribute in the join predicate
                      const Operator op,              // Predicate operator
                      const AttrDesc& attrDesc2,      // The left attribute in the join predicate
                      const int reclen)               // The length of a tuple in the result relation
{
  cout << "Algorithm: Simple NL Join" << endl;
  
//************
//************
//DECLARATIONS
//************
//************

  string r1 = attrDesc1.relName;
  
  string r2 = attrDesc2.relName;
  
  RID ro;
  
  int nb;
  
  char *src;
  
  Record rr;
  
  char *end;
  
  RID ri;
  
  Record rro;
  
  Record rri;
  
  char *rd;
  
  Status s;
  
  int ros;
  
  char *v;
  
//Main Part  
  
  HeapFileScan file_scan_outer(r2, s);
  
  if (s != OK) 
  {
    return s;
  }
  
  HeapFile result_heap_file (result, s);
  
  if (s != OK)
  {
    return s;
  }

  while (file_scan_outer.scanNext(ro, rro) == OK)
  {
      v = new char[attrDesc2.attrLen];
      
      src = ((char*)rro.data) + attrDesc2.attrOffset;
      
      memcpy(v, src, attrDesc2.attrLen);
      
      HeapFileScan file_scan_inner(r1, attrDesc1.attrOffset, attrDesc1.attrLen, static_cast<Datatype>(attrDesc1.attrType), v, op, s);
				   
      if (s != OK) 
      {
        delete [] v;
        
	      return s;
      }
      
      while (file_scan_inner.scanNext(ri, rri) == OK)
      {
	      rd = new char[reclen];
	      
	      rr.data = rd;
	      
	      rr.length = reclen;

	      ros = 0;
	      
	      int pcn = 0;
    	  
    	  while (pcn < projCnt)
    	  {
    	      src = NULL;
    	      
    	      if (attrDescArray[pcn].relName == r1)
    	      {
                src = ((char*)rri.data) + attrDescArray[pcn].attrOffset;
    	      }
    	      
    	      else
    	      {
                src = ((char*)rro.data) + attrDescArray[pcn].attrOffset;
    	      }
    	      
    	      end = rd + ros;
    	      
    	      nb = attrDescArray[pcn].attrLen;
    	      
    	      memcpy(end, src, nb);
    	      
    	      ros = ros + nb;
    	      
    	      pcn++;
    	  }

  	  RID rrd;
  	  
  	  s = result_heap_file.insertRecord(rr, rrd);
  	  
  	  delete [] rd;
  	  
  	  if (s != OK)
  	  {
  	      return s;
  	  }

    }
      
  }
  
  return OK;
  
}

