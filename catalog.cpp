#include "catalog.h"
#include "stdlib.h"

RelCatalog::RelCatalog(Status &status) :
	 HeapFile(RELCATNAME, status)
{
// nothing should be needed here
}

/**
 * Open a scan on the relCat relation by invoking the startScan() method on itself. You want to look
 * for the tuple whose first attribute matches the string <relName>. Then call scanNext() and getRecord()
 * to get the desired tuple. Finally, you need to memcpy() the tuple out of the buffer pool into the
 * return parameter record.
 *
 * @param relation
 * @param record
 * @return const Status
 */
const Status RelCatalog::getInfo(const string & relation, RelDesc &record)
{
  if (relation.empty())
    return BADCATPARM;

  Status status;
  Record rec;
  RID rid;
  HeapFileScan*  scan;
  
  Status endScanReturned; //What did endScan() return?
  Status heapFileScanReturned; //What did heapFileScan() return?
  Status startScanReturned; //What did startScan() return?
  Status scanNextReturned; //What did scanNext() return?
  Status getRecordReturned; //What did getRecord() return?

  //Open a scan on the relCat relation... 
  scan = new HeapFileScan(RELCATNAME, heapFileScanReturned);

  //Check what HeapFileScan() returned
  if(heapFileScanReturned == OK)
    {
      //...by invoking the startScan() method on itself. You want to look for the tuple whose first attribute matches the string <relName>...
      startScanReturned = scan->startScan(0, (relation.length() + 1), STRING, relation.c_str(), EQ);
      
      //Check what startScan() returned
      if(startScanReturned == OK)
	{
	  //...Then call scanNext()...
	  status = scan->scanNext(rid);
	  
	  //Check what scanNext() returned
	  if(status == OK)
	    {
	      //...and getRecord() to get the desired tuple...
	      getRecordReturned = scan->getRecord(rec);

	      //Finally, you need to memcpy() the tuple out of the buffer pool into the return parameter record.
	      if(getRecordReturned == OK)
		{
		  memcpy(&record, rec.data, rec.length);
		}

	      //Error using getRecord()
	      else
		{
		  return getRecordReturned;
		}
	    }

	  //At the end but no relation found...
	  else if(status == FILEEOF)
	    {
	      status = RELNOTFOUND;
	    }
	  
	  //Need to notify when end of scan is (here)
	  endScanReturned = scan->endScan();

	  //Check what endScanReturned
	  if(status == OK)
	    {
	      status = endScanReturned;
	    }
	  
	  //Good practice to release memory
	  free(scan);

	  return status;

	}
      
      //Error using startScan()
      else
	{
	  //Good practice to release memory
	  free(scan);

	  return startScanReturned;
	}
    }

  //Error using HeapFileScan
  else
    {
      return heapFileScanReturned;
    }

  //Compiler complaining
  return OK;
}

/**
 * Adds the relation descriptor contained in record to the relCat relation. RelDesc represents both the 
 * in-memory format and on-disk format of a tuple in relCat. First, create an InsertFileScan object on the
 * relation catalog table. Next, create a record and then insert it into the relation catalog table using 
 * the method insertRecord of InsertFileScan.
 *
 * @param record
 * @return const Status
 */
const Status RelCatalog::addInfo(RelDesc & record)
{
  RID rid;
  InsertFileScan*  ifs;
  Status status;
  
  int lengthOfRecord; //Length of the record to insert
  Record recordToInsert; //The actual record to insert

  Status insertFileScanReturned; //What did insertFileScan() return?
  Status insertRecordReturned; //What did insertRecord() return?

  //Adds the relation descriptor contained in record to the relCat relation.
  //RelDesc represents both the in-memory format and on-disk format of a tuple 
  //in relCat.

  //First, create an insertFileScan object on the relation catalog table.
  ifs = new InsertFileScan(RELCATNAME, insertFileScanReturned);

  //Check what InsertFileScan() returned
  if(insertFileScanReturned == OK)
    {
      //Next, create a record and...
      recordToInsert.length = sizeof(RelDesc);

      //Give record's data
      recordToInsert.data = &record;
      
      //...then insert it into the relation catalog table using the method 
      //insertRecord of InsertFileScane
      insertRecordReturned = ifs->insertRecord(recordToInsert, rid);
      
      //Good practice to free memory
      //Free was giving errors???
      //free(ifs);
      delete ifs;

      return insertRecordReturned;
    }

  //Error using InsertFileScan()
  else
    {
      return insertFileScanReturned;
    }

  //Compiler complaining
  return OK;
}

/**
 * Remove the tuple corresponding to <relName> from relCat. Once again, you have to start a filter scan on
 * relCat to locate the rid of the desired tuple. Then you can call deleteRecord() to remove it.
 *
 * @param relation
 * @return const Status
 */
const Status RelCatalog::removeInfo(const string & relation)
{
  Status status;
  RID rid;
  HeapFileScan*  hfs;

  Record recordToFind;
  RelDesc recordToFindDesc;
  Status heapFileScanReturned; //What did heapFileScan() return?
  Status startScanReturned; //What did startScan() return?
  Status scanNextReturned; //What did scanNext() return?
  Status endScanReturned; //What did endScan() return?
  Status getRecordReturned; //What did getRecord() return?

  if (relation.empty()) return BADCATPARM;
  
  //Remove the tuple corresponding to <relName> from relCat. Once again, you
  //have to start a filter scan on relcat to locate the rid of the desired
  //tuple
  //Initiate a scan
  hfs = new HeapFileScan(RELCATNAME, heapFileScanReturned);
  
  //Check what HeapFileScan() returned
  if(status == OK)
    {
      //Start the scan for the tuple to be removed
      startScanReturned = hfs->startScan(0, relation.length() + 1, STRING, relation.c_str(), EQ);

      //Check what startScan() returned
      if(startScanReturned == OK)
	{
	  //Use scanNext to find the tuple
	  scanNextReturned = hfs->scanNext(rid);
	  
	  while(scanNextReturned != FILEEOF)
	    {
	      //Check iteration
	      if(scanNextReturned == OK)
		{
		  //Then you can call deleteRecord() to remove it.
		  //Check what scanNext() returned
		  getRecordReturned = hfs->getRecord(recordToFind);
		  
		  //Check what getRecord() returned
		  if(getRecordReturned == OK)
		    {
		      //Copy over to description to delete it
		      memcpy(&recordToFindDesc, recordToFind.data, sizeof(RelDesc));
		      
		      //If this is the one we want to delete, delete it
		      if(strcmp(recordToFindDesc.relName, relation.c_str()) == 0)
			{
			  hfs->deleteRecord();
			  
			  //Good practice to free up memory
			  free(hfs);
			  
			  return OK;
			}
		    }
		  
		  //Error using getRecord()
		  else
		    {
		      return getRecordReturned;
		    }
		}
	      
	      //Error using scanNext()
	      else
		{
		  return scanNextReturned;
		}
	      
	      scanNextReturned = hfs->scanNext(rid);
	    }
	}

      //Error using startScan()
      else
	{
	  //Good practice to free memory
	  free(hfs);
	  
	  return startScanReturned;
	}
    }

  //Error using HeapFileScan()
  else
    {
      return heapFileScanReturned;
    }
}


RelCatalog::~RelCatalog()
{
// nothing should be needed here
}


AttrCatalog::AttrCatalog(Status &status) :
	 HeapFile(ATTRCATNAME, status)
{
// nothing should be needed here
}

/**
 * Returns the attribute descriptor record for attribute <attrName> in relation <relName>. Uses a scan
 * over the underlying heapfile to get all tuples for relation and check each tuple to find whether it
 * corresponds to attrName. (Or maybe do it the other way around!) This has to be done this way because
 * a predicated HeapFileScan does not allow conjuncted predicates. Note that the tuples in attrCat are
 * of type AttrDesc (structure given above).
 *
 * @param relation
 * @param attrName
 * @param record
 * @return const Status
 */
const Status AttrCatalog::getInfo(const string & relation, 
				  const string & attrName,
				  AttrDesc &record)
{

  Status status;
  RID rid;
  Record rec;
  HeapFileScan*  hfs;

  Status heapFileScanReturned; //What did heapFileScan() return?
  Status startScanReturned; //What did startScan() return?
  Status scanNextReturned; //What did scanNext() return?
  Status getRecordReturned; //What did getRecord() return?

  
  if (relation.empty() || attrName.empty()) return BADCATPARM;

  
  //Returns the attribute descriptor record for attribute <attrName> in relation <relName>.
  //Uses a scan over the underlying heapfile to get all tuples for relation and check each
  //tuple to find whether it corresponds to attrName. (Or maybe do it the other way around!)
  //This has to be done this way because a predicated HeapFileScan does not allow conjuncted
  //predicates. Note that the tuples in attrCat are of type AttrDesc (structure given above).

  //Uses a scan over the underlying heapfile to get all tuples for relation and...
  hfs = new HeapFileScan(ATTRCATNAME, heapFileScanReturned);

  //Check what HeapFileScan() returned
  if(heapFileScanReturned == OK)
    {
      startScanReturned = hfs->startScan(0, relation.length() + 1, STRING, relation.c_str(), EQ);

      //Check what startScan() returned
      if(startScanReturned == OK)
	{
	  scanNextReturned = hfs->scanNext(rid);
	  
	  //...check each tuple to find whether it corresponds to attrName.
	  while(scanNextReturned == OK)
	    {
	      getRecordReturned = hfs->getRecord(rec);
	      
	      //Check what getRecord() returned
	      if(getRecordReturned == OK)
		{
		  //Copy over contents that's needed...
		  memcpy(&record, rec.data, rec.length);

		  //Is this the tuple we're looking for?
		  if(record.attrName == attrName)
		    {
		      //Have a match so escape from loop
		      break;
		    }
		}
	      
	      //Error using getRecord()
	      else
		{
		  return getRecordReturned;
		}

	      //Next loop iteration
	      scanNextReturned = hfs->scanNext(rid);
	    }
	  
	  //Check what scanNext() returned
	  if(scanNextReturned != FILEEOF)
	    {
	      status = scanNextReturned;
	      status = hfs->endScan();
	    }

	  //The attribute couldn't be found
	  else
	    {
	      status = ATTRNOTFOUND;
	    }
	  
	  //Good practice to free memory
	  free(hfs);
	  
	  return status;
	}

      //Error using startScan()
      else
	{
	  //Good practice to free memory
	  free(hfs);
	  
	  return startScanReturned;
	}
    }
  
  //Error using HeapFileScan
  else
    {
      return heapFileScanReturned;
    }
  
  //Compiler complaining
  return OK;
}

/**
 * Adds a tuple (corresponding to an attribute of a relation) to the attrCat relation.
 *
 * @param record
 * @return const Status
 */
const Status AttrCatalog::addInfo(AttrDesc & record)
{
  RID rid;
  InsertFileScan*  ifs;
  Status status;

  int lengthOfRecord;
  Record recordToInsert;

  Status insertFileScanReturned; //What did insertFileScan() return?
  Status insertRecordReturned; //What did insertRecord() return?

  //Adds a tuple (corresponding to an attribute of a relation) to the attrCat relation
  //////////////////////////
  //VERY SIMILAR TO RELCAT//
  //////////////////////////
  ifs = new InsertFileScan(ATTRCATNAME, insertFileScanReturned);

  //Check what InsertFileScan() returned
  if(insertFileScanReturned == OK)
    {
      //Add info for relCat
      recordToInsert.length = sizeof(AttrDesc);

      recordToInsert.data = &record;
     
      insertRecordReturned = ifs->insertRecord(recordToInsert, rid);
      
      //Check what insertRecord() returned
      if(insertRecordReturned == OK)
      {
	//Good practice to free memory
	//Free was giving errors?
	//free(ifs);
	delete ifs;

	return insertRecordReturned;
      }
      
      //Error using insertRecord()
      else
	{
	  //Good practice to free memory
	  //Free was giving errors?
	  //free(ifs);
	  delete ifs;

	  return insertRecordReturned;
	}
    }

  //Error using InsertFileScan()
  else
  {
    return insertFileScanReturned;
  }
  
  //Compiler complaining
  return OK;
 
}

/**
 * Removes the tuple from attrCat that corresponds to attribute <attrName> of relation.
 *
 * @param relation
 * @param attrName
 * @return const Status
 */
const Status AttrCatalog::removeInfo(const string & relation, 
			       const string & attrName)
{
  Status status;
  Record rec;
  RID rid;
  AttrDesc record;
  HeapFileScan*  hfs;

  Status heapFileScanReturned; //What did heapFileScan() return?
  Status startScanReturned; //What did startScan() return?
  Status scanNextReturned; //What did scanNext() return?
  Status getRecordReturned; //What did getRecord() return?
  Status deleteRecordReturned; //What did deleteRecord() return?

  if (relation.empty() || attrName.empty()) return BADCATPARM;

  //Removes the tuple from attrCat that corresponds to attribute <attrName> of relation.
  hfs = new HeapFileScan(ATTRCATNAME, heapFileScanReturned);

  //Check what HeapFileScan() returned
  if(heapFileScanReturned == OK)
    {
      startScanReturned = hfs->startScan(0, (relation.length() + 1), STRING, relation.c_str(), EQ);
						
      //Check what startScan() returned
      if(startScanReturned == OK)
	{
	  scanNextReturned = hfs->scanNext(rid);

	  //Loop through if scanNext has a next value
	  while(scanNextReturned == OK)
	    {
	      getRecordReturned = hfs->getRecord(rec);

	      //Check what getRecord() returned
	      if(getRecordReturned == OK)
		{
		  //Copy data over
		  memcpy(&record, rec.data, rec.length);

		  //If this record's att name is the attr name being looked for, found
		  if(record.attrName == attrName)
		    {
		      break;
		    }
		}

	      //Error using getRecord()
	      else
		{
		  return getRecordReturned;
		}
	    }

	  //Check what scanNext() returned
	  if(scanNextReturned == OK)
	    {
	      deleteRecordReturned = hfs->deleteRecord();
	    }

	  //Couldn't find the relation
	  else
	    {
	      deleteRecordReturned = RELNOTFOUND;
	    }

	  //Complete the scan
	  hfs->endScan();

	  //Good practice to free memory
	  free(hfs);

	  return deleteRecordReturned;
	}

      //Error using startScan()
      else
	{
	  //Good practice to free memory
	  free(hfs);

	  return startScanReturned;
	}
    }

  //Error using HeapFileScan
  else
    {
      return heapFileScanReturned;
    }
  
  //Compiler complaining
  return OK;
}

/**
 * While getInfo() above returns the description of a single attribute, this method returns (by reference)
 * descriptors for all attributes of the relation via attr, an array of AttrDesc structures, and the count
 * of the number of attributes in attrCnt. The attrs array is allocated by this function, but it should
 * be deallocated by the caller.
 *
 * @param relation
 * @param attrCnt
 * @param attrs
 * @return const Status
 */
const Status AttrCatalog::getRelInfo(const string & relation, 
				     int &attrCnt,
				     AttrDesc *&attrs)
{
  Status status;
  RID rid;
  Record rec;
  HeapFileScan*  hfs;

  RelDesc descriptor; //Descriptor "shell"
  int amount; //Attrs left to go
  Status getInfoReturned; //What did getInfo() return?
  Status heapFileScanReturned; //What did heapFileScan() return?
  Status startScanReturned; //What did startScan() return?
  Status getRecordReturned; //What did getRecord() return?
  Status scanNextReturned; //What did scanNext() return?

  if (relation.empty()) return BADCATPARM;

  //While getInfo() above returns the descriptiong of a single attribute, this method returns
  //(by reference) descriptors for all attributes of the relation via attr, an array of 
  //AttrDesc structures, and the count of the number of attributes in attrCnt. The attrs array
  //is allocated by this function, but it should be deallocated by the caller.
  getInfoReturned = relCat->getInfo(relation, descriptor);

  //Check what getInfo() returned
  if(getInfoReturned == OK)
    {
      //Need amount
      attrCnt = descriptor.attrCnt;
      
      //Create descriptor "shell"
      attrs = new AttrDesc[attrCnt];
  
      //Create new scan over attrCat
      hfs = new HeapFileScan(ATTRCATNAME, heapFileScanReturned);

      //Check what HeapFileScan() returned
      if(heapFileScanReturned == OK)
	{
	  //Start the scan over attrCat
	  startScanReturned = hfs->startScan(0, 0, STRING, NULL, EQ);
	  
	  //Check what startScan() returned
	  if(startScanReturned == OK)
	    {
	      //Start iterations over attrCat
	      scanNextReturned = hfs->scanNext(rid);
	      
	      //Attrs to go
	      amount = attrCnt - 1;
	      
	      //Scan over all of attrCat until end of file
	      while(scanNextReturned != FILEEOF)
		{
		  //Make sure there are attrCat record left...
		  if(amount < 0) 
		    {
		      break;
		    }
	
		  //Check status of scanNext()
		  if(scanNextReturned == OK)
		    {
		      //Not get the attrCat record
		      getRecordReturned = hfs->getRecord(rec);
		      
		      //Check what getRecord() returned
		      if(getRecordReturned == OK)
			{
			  //Copy over
			  memcpy(&attrs[amount], rec.data, sizeof(AttrDesc));
			  
			  //Make sure this it it
			  if(strcmp(attrs[amount].relName, relation.c_str()) == 0) 
			    {
			      //Less attrs to go
			      amount--;
			    }
			}

		      //Error using getRecord()
		      else
			{
			  return getRecordReturned;
			}
		    }
		  
		  //Error using scanNext()
		  else
		    {
		      return scanNextReturned;
		    }
		  
		  //Next iteration over attrCat
		  scanNextReturned = hfs->scanNext(rid);
		}

	      //Good practice to free memory
	      free(hfs);
	      
	      //Everything went fine
	      return OK;
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
    }

  //Error using getInfo()
  else
    {
      return getInfoReturned;
    }

  //Compiler complaining
  return OK;
}


AttrCatalog::~AttrCatalog()
{
// nothing should be needed here
}

