#include "catalog.h"
#include "query.h"
#include "stdlib.h"

// forward declaration
const Status ScanSelect(const string & result, 
			const int projCnt, 
			const AttrDesc projNames[],
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen);

/*
 * Selects records from the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Select(const string & result, 
		       const int projCnt, 
		       const attrInfo projNames[],
		       const attrInfo *attr, 
		       const Operator op, 
		       const char *attrValue)
{
   // Qu_Select sets up things and then calls ScanSelect to do the actual work
    cout << "Doing QU_Select " << endl;
    
    //A slection is implemented using a filtered HeapFileScan. The result of the selection
    //is stored in the result relation called result(a heapfile with this name will be
    //created by the parser before QU_Select() is called). The project list is defined by the 
    //parameters projCnt and projNames. Projection should be done on the fly as each result
    //tuple is being appended to the result table. A final note: the search value is always
    //supplied as the character string attrValue. You should convert it to the proper type
    //based on the type of attr. You can use the atoi() function to convert a char* to an
    //integer and atof() to convert it to a float. If attr is NULL, an unconditional scan
    //of the input table should be performed.
 
    Status getInfoReturned; //What did getInfo() return?
    Status getInfo2Returned; //What did getInfo() return?
    AttrDesc attrDesc; //Attr descriptions
    AttrDesc* theProjections; //Projections in query
    const char* change; //Conversion of type

    int i; //Loop counter
    int lengthOfAttrs; //Length of all attrs added up
    int attrType; //Type of the attr
    int fAttrType; //Float attr
    int iAttrType; //Int attr
    
    //Projection should be done on the fly as each result tuple is being appended to the result table.
    //The project list is defined by the parameters projCnt and projnames.
    theProjections = new AttrDesc[projCnt];
    
    //Loop over all attrDescs up until the last one
    while(i < projCnt)
      {
	getInfoReturned = attrCat->getInfo(projNames[i].relName, projNames[i].attrName, theProjections[i]);

	//Check what getInfo() returned
	if(getInfoReturned == OK)
	  {
	    
	  }
	
	//Error using getInfo()
	else
	  {
	    return getInfoReturned;
	  }

	i++;
      }

    //Attr is null...best error code I could find in error.h...
    if(attr == NULL)
      {
	return ATTRTYPEMISMATCH;
      }

    //Attr isn't NULL so continue...
    else
      {
	//Get all attr info
	getInfo2Returned = attrCat->getInfo(attr->relName, attr->attrName, attrDesc);
	
	//Check what getInfo returned
	if(getInfo2Returned == OK)
	  {
	    //Beginning
	    lengthOfAttrs = 0;

	    //Add up all attr lengths
	    while(i < projCnt)
	      {
		lengthOfAttrs = lengthOfAttrs + theProjections[i].attrLen;

		i++;
	      }

	    //Retrieve attrType to see what type it is
	    attrType = attrDesc.attrType;

	    //attrType is of type FLOAT
	    if(attrType == FLOAT)
	      {
		//Convert string->float
		fAttrType = atof(attrValue);

		//Store in temp holder
		change = (char*) &fAttrType;
	      }

	    //attrType is of type INTEGER
	    else if(attrType == INTEGER)
	      {
		//Convert string->int
		iAttrType = atoi(attrValue);

		//Store in temp holder
		change = (char*) &iAttrType;
	      }

	    //attrType is of type STRING
	    else
	      {
		//Stor in temp holdery
		change = attrValue;
	      }

	    //Lastly, return the ScanSelect() with all descriptors
	    return ScanSelect(result, projCnt, theProjections, &attrDesc, op, change, lengthOfAttrs);
	  }
	
	//Error using getInfo()
	else
	  {
	    return getInfo2Returned;
	  }
      }
    
    //Compiler complaining
    return OK;
}


const Status ScanSelect(const string & result, 
			const int projCnt, 
			const AttrDesc projNames[],
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen)
{
    cout << "Doing HeapFileScan Selection using ScanSelect()" << endl;


}
