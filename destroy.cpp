#include "catalog.h"
#include "stdlib.h"
//
// Destroys a relation. It performs the following steps:
//
// 	removes the catalog entry for the relation
// 	destroys the heap file containing the tuples in the relation
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status RelCatalog::destroyRel(const string & relation)
{
  //Status status;
  Status removeInfoReturned; //What did removeInfo return?
  Status dropRelationReturned; //What did dropRelation return?
  Status destroyHeapFileReturned; //What did destroyHeapFile return?

  if (relation.empty() || 
      relation == string(RELCATNAME) || 
      relation == string(ATTRCATNAME))
    return BADCATPARM;
  
  //First remove all relevant schema information from both the relCat...
  removeInfoReturned = removeInfo(relation);

  //Check removeInfo() return
  if(removeInfoReturned == OK)
    {
      //...and attrCat relations.
      dropRelationReturned = attrCat->dropRelation(relation);

      //Check dropRelation() return
      if(dropRelationReturned == OK)
	{
	  //Then destory the heapfile corresponding to the relation (hint: again, there is a procedure to destroy
	  //heap file that we have seen in the last project stage; you need to give it a string that is the relation name).
	  destroyHeapFileReturned = destroyHeapFile(relation);

	  //Check what destroyHeapFile() returned
	  if(destroyHeapFileReturned == OK)
	    {
	      return destroyHeapFileReturned;
	    }

	  //Error using destroyHeapFile()
	  else
	    {
	      return destroyHeapFileReturned;
	    }
	}

      //Error using dropRelation() 
      else
	{
	  return dropRelationReturned;
	}
    }

  //Error using removeInfo()
  else
    {
      return removeInfoReturned;
    }

  //Compiler complaining
  return OK;
}


//
// Drops a relation. It performs the following steps:
//
// 	removes the catalog entries for the relation
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status AttrCatalog::dropRelation(const string & relation)
{
  //Status status;
  AttrDesc *attrs;
  int attrCnt, i;
 
  Status heapFileScanReturned; //What did heapFileScan() return?
  Status startScanReturned; //What did startScan() return?
  Status scanNextReturned; //What did scanNext() return?
  Status getRecordReturned; //What did getRecord() return?
  Status endScanReturned; //What did endScan() return?
  Status heapFileScanReturned2; //...
  Status startScanReturned2; //...
  Status scanNextReturned2; //...
  Status getRecordReturned2; //...
  Status endScanReturned2; //...
  HeapFileScan* scan; //heapfilescan
  Record recordToDrop; 
  RID recordToDropID;

  //Status getRelInfoReturned; //What did getRelInfo return?
  //Status removeInfoReturned; //What did removeInfo return?

  if (relation.empty()) return BADCATPARM;
  
  
  //Deletes all tuples in attrCat for the relation <relName>. Again use a scan to find all tuples whose <relName>
  //attribute equals relation and then call deleteRecord() on each one.
  //First need to delete tuples in relCat then onto attrCat...
  scan = new HeapFileScan(RELCATNAME, heapFileScanReturned);
  
  //Check what HeapFileScan() returned
  if(heapFileScanReturned == OK)
    {
      //Start the scan over all tuples
      startScanReturned = scan->startScan(0, 0, STRING, NULL, EQ);

      //Check what startScan() returned
      if(startScanReturned == OK)
	{
	  //Now begin iterations with the scan over the tuples
	  scanNextReturned = scan->scanNext(recordToDropID);
	  
	  //While there's still a tuple left
	  while(scanNextReturned != FILEEOF)
	    {
	      //Check what scanNext() returned
	      if(scanNextReturned == OK)
		{
		  //Get the tuple
		  getRecordReturned = scan->getRecord(recordToDrop);

		  //Check what getRecord() returned
		  if(getRecordReturned == OK)
		    {
		      //Make sure it's the record that needs to be deleted
		      if (strcmp((char*) recordToDrop.data, relation.c_str()) == 0) 
			{
			  //Delete the record
			  scan->deleteRecord();
			}
		    }
		  
		  //Error using getRecord()
		  else
		    {
		      return getRecordReturned;
		    }
		  
		  //Get next tuple if possible 
		  scanNextReturned = scan->scanNext(recordToDropID);
		}

	      //Error using scanNext()
	      else
		{
		  return scanNextReturned;
		}
	    }

	  //End the current scan
	  endScanReturned = scan->endScan();

	  //Check what endScan() returned

	  ///////////////////////////////////
	  // REPEAT FROM HERE ESSENTIALLY  //
	  ///////////////////////////////////
	  if (endScanReturned == OK)
	    {
	      //Good practice to free memory
	      free(scan);
  
	      //Begin scan over attrCat now
	      scan = new HeapFileScan(ATTRCATNAME, heapFileScanReturned2);

	      //Check what HeapFileScan returned
	      if (heapFileScanReturned2 == OK)
		{

		  //Initiate iterations over attrCat
		  startScanReturned2 = scan->startScan(0, 0, STRING, NULL, EQ);
		  
		  //Check what startScan() returned
		  if (startScanReturned2 == OK)
		    {
		      //Now actually iterate over attrCat
		      scanNextReturned2 = scan->scanNext(recordToDropID);
		      
		      while(scanNextReturned2 != FILEEOF)
			{
			  //Check what scanNext() returned
			  if(scanNextReturned2 == OK)
			    {
			      //Now get the record in attrCat
			      getRecordReturned2 = scan->getRecord(recordToDrop);
			      
			      //Check what getRecord returned
			      if(getRecordReturned2 == OK)
				{
				  //Make sure this is the record in attrCat we want to delete
				  if(strcmp((char*) recordToDrop.data, relation.c_str()) == 0) 
				    {
				      //Delete it
				      scan->deleteRecord();
				    }
				}

			      //Error using getRecrd()
			      else
				{
				  return getRecordReturned2;
				}
			 
			      //Next iteration if possible
			      scanNextReturned2 = scan->scanNext(recordToDropID);
			    }

			  //Error using getRecord()
			  else
			    {
			      return getRecordReturned2;
			    }
			}

		      //Close the scan
		      endScanReturned2 = scan->endScan();

		      if (endScanReturned2 == OK)
			{
			  free(scan);
  
			  return OK;
			}

		      //Error using endScan()
		      else
			{
			  return endScanReturned2;
			}
		    }

		  //Error using startScan()
		  else
		    {
		      return startScanReturned2;
		    }
		}

	      //Error using HeapFileScan()
	      else
		{
		  return heapFileScanReturned2;
		}
	    }

	  //Error using endScan()
	  else
	    {
	      return endScanReturned2;
	    }    
	}
  
      //Error using startScan()
      else
	{
	  return startScanReturned;
	}
    }

  //Error using HeapFileScan()
  else
    {
      return heapFileScanReturned;
    }

  //Compiler complaining
  return OK;
}

