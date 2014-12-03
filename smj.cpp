#include "catalog.h"
#include "query.h"
#include "sort.h"
#include "index.h"
#include <cstdlib>
#include <cstring>
#include <string.h>

#define x 1024
#define dmx 0.8

/* Consider using Operators::matchRec() defined in join.cpp
 * to compare records when joining the relations */
  
Status Operators::SMJ(const string& result,           // Output relation name
                      const int projCnt,              // Number of attributes in the projection
                      const AttrDesc attrDescArray[], // Projection list (as AttrDesc)
                      const AttrDesc& attrDesc1,      // The left attribute in the join predicate
                      const Operator op,              // Predicate operator
                      const AttrDesc& attrDesc2,      // The left attribute in the join predicate
                      const int reclen)               // The length of a tuple in the result relation
{
  cout << "Algorithm: SM Join" << endl;
  
//************
//************
//DECLARATIONS
//************
//************

  Status r1;
  
  Status r2;

  Record rr1;
  
  Record rr2;
  
  Record rr3;
  
  string rn1(attrDesc1.relName);
  
  string rn2(attrDesc2.relName);

  int rl1 = 0;
  
  int rl2 = 0;

  RID rid1;
  
  RID rid2;
  
  RID rid3;
  
  HeapFile* hf;
  
  RelDesc rd1;
  
  RelDesc rd2;
  
  SortedFile* sf1;
  
  SortedFile* sf2;
  
  AttrDesc* l;
  
  AttrDesc* r;
  
  int ac = 0;

  int al = 0;
  
  int mi1;
  
  int mi2;
  
  int upp = bufMgr->numUnpinnedPages();

  double sp = dmx*upp;

//************
//************
//**Get Rels**
//************
//************

  relCat->getInfo(rn2, rd1);
  
  relCat->getInfo(rn2, rd2);

  attrCat->getRelInfo(rn2, rd1.attrCnt, l);
  
  attrCat->getRelInfo(rn2, rd2.attrCnt, r);
  
//Main Stuff

  while (ac < rd1.attrCnt)
  {
  	rl1 = rl1 + l[ac].attrLen;
  	
  	ac++;
  }
  
  
  while (al < rd2.attrCnt)
  {
  	rl2 = rl2 + r[al].attrLen;
  	
  	al++;
  }

  mi1 = (sp/rl1) * x;
  
  mi2 = (sp/rl2) * x;


  sf1 = new SortedFile(attrDesc1.relName, attrDesc1.attrOffset, 
  							attrDesc1.attrLen, (Datatype) attrDesc1.attrType, 
							mi1, r1);

  if(r1 != OK)
  {
    return r1;
  }

  sf2 = new SortedFile(attrDesc2.relName, attrDesc2.attrOffset, 
  							attrDesc2.attrLen, (Datatype) attrDesc2.attrType, 
							mi2, r2);

  if(r2 != OK)
  {
    return r2;
  }


  hf = new HeapFile(result, r1);
  
  if(r1 != OK)
  {
    return r1;
  }

  r1 = sf1->next(rr1);

  if(r1 != OK)
  {
    return r1;
  }

  r2 = sf2->next(rr2);

  if(r2 != OK)
  {
    return r2;
  }


  while(1<2)
  {

    if(matchRec(rr1, rr2, attrDesc1, attrDesc2) < 0)
    {
      r1 = sf1->next(rr1);
    }
    
    else if ((matchRec(rr1, rr2, attrDesc1, attrDesc2) < 0) && (r1 != OK))
    {
        break;
    }
    
    else if (matchRec(rr1, rr2, attrDesc1, attrDesc2) > 0)
    {
      r2 = sf2->next(rr2);
    }
    
    else if ((matchRec(rr1, rr2, attrDesc1, attrDesc2) > 0) && (r2 != OK))
    {
      break;
    }

    else
    {
      rr3 = rr1;
      
      r1 = sf1->setMark();
      
      if (r1 != OK)
      {
        return r1;
      }

      bool working = true;
      
      while (matchRec(rr3, rr2, attrDesc1, attrDesc2) == 0 && working)
      {
        Record rcd;
        
        rcd.data = malloc(reclen);
        
        rcd.length = reclen;
        
        RID rid;

        int ous = 0;

        int i = 0;
        
        while (i < projCnt)
        {

          if (!strcmp(attrDescArray[i].relName, attrDesc1.relName))
          {
            memcpy ((char *)rcd.data + ous, (char *)rr1.data + attrDescArray[i].attrOffset, attrDescArray[i].attrLen);
            
            ous += attrDescArray[i].attrLen;   
          }
          
          else if(!strcmp(attrDescArray[i].relName, attrDesc2.relName))
          {
            memcpy((char *)rcd.data + ous, (char *)rr2.data + attrDescArray[i].attrOffset, attrDescArray[i].attrLen);
            
            ous += attrDescArray[i].attrLen;   
          }
          
          i++;
          
        }

        r1 = hf->insertRecord(rcd, rid);
        
        if (r1 != OK)
        {
          return r1;
        }
        
        r1 = sf1->next(rr3);
        
        if(r1 != OK)
        {
          working = false;
        }
        
        free (rcd.data);
      }
      
      r2 = sf2->next(rr2);
      
      if(r2 != OK)
      {
        break;
      }

      r1 = sf1->gotoMark();
      
      if(r1 != OK)
      {
        return r1;
      }
      
    }
    
  }

  delete sf1;
  
  delete sf2;
  
  delete hf;
  
  return OK;
}

