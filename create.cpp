#include "catalog.h"

/**
 * First, make sure that a relation with the same name doesn't already exit (by using the getInfo()
 * function described below). Next, add a tuple to the relCat relation. Do this by filling in an
 * instance of the RelDesc structure above and then invoking the RelCatalog::addInfo() method.
 * Third, for each of the attrCnt attributes, invoke the AttrCatalog::addInfo() method of the 
 * attribute catalog table (remember that this table is referred to by the global variable attrCat),
 * passing the appropriate attribute information from the attrList[] array as an instance of the 
 * AttrDesc structure (see below). Finally, create a HeapFile instance to hold tuples of the relation
 * (hint: there is a procedure to do this which we have seen in the last project stage; you need to 
 * give it a string that is the relation name).
 *
 * @param relation
 * @param attrCnt
 * @param attrList[]
 * @return const Status
 */
const Status RelCatalog::createRel(const string & relation, 
				   const int attrCnt,
				   const attrInfo attrList[])
{
  Status status;
  RelDesc rd;
  AttrDesc ad;

  Status getInfoReturned; //What did getInfo() return?
  Status addInfoReturned; //What did addInfo() return?
  Status createHeapFileReturned;
  int ordering = 0; //The offset for attrOffset
  int i = 0; //Loop iterations

  if (relation.empty() || attrCnt < 1)
    return BADCATPARM;


  //Through testbench, determined need to check lengnth
  if(relation.length() >= sizeof(rd.relName))
    {
      return NAMETOOLONG;
    }

  //First, make sure that a relation with the same name doesn't already exit (by using the getInfo()
  //function described below.
  getInfoReturned = getInfo(relation, rd);

  //Check what getInfo() returned
  if(getInfoReturned == OK)
    {
      return RELEXISTS;
    }

  //Relation not found...which is good in this case!
  else if(getInfoReturned == RELNOTFOUND)
    {
      //Next, add a tuple to the relCat relation. Do this by filling in an instance of the RelDesc 
      //structure above and then invoking the RelCatalog::addInfo() method.

      strcpy(rd.relName, relation.c_str());
      rd.attrCnt = attrCnt;

      addInfoReturned = addInfo(rd);
      
      //Check what addInfo() returned
      if(addInfoReturned == OK)
	{
	  //Third, for each of the attrCnt attributes, invoke the AttrCatalog::addInfo() method of the 
	  //attribute catalog table (remember that this table is referred to by the global variable
	  //attrCat), passing the appropriate attribute information from the attrList[] array as an
	  //instance of the AttrDesc structure (see below).
	  while(i < attrCnt)
	    {
	      //Create attr
	      strcpy(ad.attrName, attrList[i].attrName);
	      strcpy(ad.relName, attrList[i].relName);
	     
	      //attr attributes
	      ad.attrOffset = ordering;
	      ad.attrLen = attrList[i].attrLen;
	      ad.attrType = attrList[i].attrType;
	      
	      //Basically lengths
	      ordering = ordering + ad.attrLen;
	      
	      //Add it
	      addInfoReturned = attrCat->addInfo(ad);
	      
	      i++;
	    }
	  
	  //Finally, create a HeapFile instance to hold tuples of the relation (hint: there is a procedure
	  //to do this which we have seen in the last project stage; you need to give it a string that is
	  //the relation name)
	  createHeapFileReturned = createHeapFile(relation.c_str());
	  
	  //Check what createHeapFile returned
	  if(createHeapFileReturned == OK)
	    {
	      return createHeapFileReturned;
	    }
	  
	  //Error using createHeapFile()
	  else
	    {
	      return createHeapFileReturned;
	    }
	}
      
      //Error using addInfo()
      else
	{
	  return addInfoReturned;
	}
    }

  //Relation not found due to some possible error
  else
    {
      return getInfoReturned;
    }

 //Compiler complained
 return OK;
}

