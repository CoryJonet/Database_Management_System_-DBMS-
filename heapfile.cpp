///////////////////////////////////////////////////////////////////////////////
//                   ALL STUDENTS COMPLETE THESE SECTIONS
// File:             heapfile.cpp
// Semester:         CS564 Fall 2014
//
// Author:           Cory Jonet jonet@wisc.edu
// CS Login:         jonet
// Lecturer's Name:  Professor Naughton
//
//////////////////////////// 80 columns wide //////////////////////////////////

#include "heapfile.h"
#include "error.h"

// routine to create a heapfile
/**
 * This function creates an empty (well, almost empty) heap file. To do this 
 * create a db level file by calling db->createFile(). Then, allocate an empty 
 * page by invoking bufMgr->allocPage() appropriately. As you know allocPage()
 * will return a pointer to an empty page in the buffer pool along with the page 
 * number of the page. Take the Page* pointer returned from allocPage() and cast 
 * it to a FileHdrPage*. Using this pointer initialize the values in the header 
 * page. Then make a second call to bufMgr->allocPage(). This page will be the 
 * first data page of the file. Using the Page* pointer returned, invoke its 
 * init() method to initialize the page contents. Finally, store the page number 
 * of the data page in firstPage and lastPage attributes of the FileHdrPage. When 
 * you have done all this unpin both pages and mark them as dirty.
 *
 * @param fileName Name of the file
 * @return Status
 */
const Status createHeapFile(const string fileName)
{
    File* 		file;
    //Status 		status; I like descriptive names
    FileHdrPage*	hdrPage;
    int			hdrPageNo;
    int			newPageNo;
    Page*		newPage;

    Status createFileReturned; //What did the call to createFile return?
    Status openFileReturned; //What did the call to openFile return?
    Status allocPage1Returned; //What did the first call to allocPage return?
    Status allocPage2Returned; //What did the second call to allocPage return?
    Status unPinPage1Returned; //What did the first call to unPinPage return?
    Status unPinPage2Returned; //What did the second call to unPinPage return?
    
    //Try to open the file. This should return an error
    openFileReturned = db.openFile(fileName, file);

    if (openFileReturned != OK)
    {
         // file doesn't exist. First create it and allocate
	 // an empty header page and data page.
	 
	     //This function creates an empty (well, almost empty) heap file
	     //To do this, create a db level file by calling db->createFile()
	     createFileReturned = db.createFile(fileName);
	     
	     //Check what createFile returned
	     //File was created? then continue
	     if(createFileReturned == OK)
	       {
		 //Well now we need to open it...
		 openFileReturned = db.openFile(fileName, file);
		 
		 //Check status of opening file
		 if(openFileReturned == OK || openFileReturned == FILEEXISTS)
		   {
	     
		     //Then, allocate an empty page by invoking bufMgr->allocPage() appropriately
		     //As you know, allocPage() will return a pointer to an empty page in the buffer
		     //pool along with the page number of the page.
		     allocPage1Returned = bufMgr->allocPage(file, newPageNo, newPage);
	     
		     if(allocPage1Returned == OK)
		       {

			 //Take the Page* pointer returned from allocPage() and cast it to a FileHdrPage*
			 hdrPage = (FileHdrPage*) newPage;

			 //Using this pointer (above), initialize the values in the header page
			 hdrPage->pageCnt = 1;
			 hdrPage->recCnt = 0;

			 hdrPageNo = newPageNo;

			 //Extract string from fileName, put in header page
			 strcpy(hdrPage->fileName, fileName.c_str());

			 //Then make a second call to bufMgr->allocPage(). This page will be the first data 
			 //page of the file
			 allocPage2Returned = bufMgr->allocPage(file, newPageNo, newPage);
		 
			 if(allocPage2Returned == OK)
			   {

			     //Using the Page* pointer returned, invoke its init() method to initialize the page contents
			     newPage->init(newPageNo);

			     //Finally, store the page number of the data page in firstPage and lastPage attributes 
			     //of the FileHdrPage	
			     hdrPage->firstPage = newPageNo;
			     hdrPage->lastPage = newPageNo;

			     //When you have done all this, unpin both pages and mark them as dirty
			     unPinPage1Returned = bufMgr->unPinPage(file, newPageNo, true);
			     
			     //Bump up error from unPinPage
			     if(unPinPage1Returned != OK)
			       {
				 return unPinPage1Returned;
			       }

			     unPinPage2Returned = bufMgr->unPinPage(file, hdrPageNo, true);	
			     
			     //Bump up error from unPinPage
			     if(unPinPage2Returned != OK)
			       {
				 return unPinPage2Returned;
			       }
			   }
			 
			 //Bump up error from using allocPage
			 else
			   {
			     return allocPage2Returned;
			   }
		       }

		     //Bump up error from using allocPage
		     else
		       {
			 return allocPage1Returned;
		       }
		   }

		 //Bump up error from using openFile
		 else
		   {
		     return openFileReturned;
		   }
	       }

	     //Bump up error from using createFile
	     else
	       {
		 return createFileReturned;
	       }

	 //Should close the file...good practice
	 db.closeFile(file);

	 //Everything should be ok and worked out...
	 return OK;
    }

    //File already exists
    else
      {
	//Should close the file...good practice
	db.closeFile(file);
	return (FILEEXISTS);
      }
}
// routine to destroy a heapfile
 const Status destroyHeapFile(const string fileName)
{
	return (db.destroyFile (fileName));
}

// constructor opens the underlying file
/**
 * This method first opens the appropriate file by calling db->openFile() (do 
 * not forget to save the File* returned in the filePtr data member). Next, it 
 * reads and pins the header page for the file in the buffer pool, initializing 
 * the private data members headerPage, headerPageNo, and hdrDirtyFlag. You might 
 * be wondering how you get the page number of the header page. This is what 
 * file->getFirstPage() is used for (see description of the I/O layer)! Finally, 
 * read and pin the first page of the file into the buffer pool, initializing the 
 * values of curPage, curPageNo, and curDirtyFlag appropriately. Set curRec to 
 * NULLRID.
 *
 * @param fileName The name of the file
 * @param returnStatus The status to return
 */
HeapFile::HeapFile(const string & fileName, Status& returnStatus)
{
    //Status 	status; I like descriptive names
    Page*	pagePtr;
    
    Status openFileReturned;
    Status getFirstPageReturned;
    Status readPageReturned;
    cout << "opening file " << fileName << endl;
    
    //This method first opens the appropriate file by calling db->openFile()
    //(do not forget to save the File* returned in the filePtr data member)
    // open the file and read in the header page and the first data page
    if ((openFileReturned = db.openFile(fileName, filePtr)) == OK)
    { 
      //You may be wondering how you get the page number of the header
      //page. This is what getFirstPage() is used for (see description of
      //the I/O layer)!
      getFirstPageReturned = filePtr->getFirstPage(headerPageNo);

      //Check status of getFirstPage
      //Went ok so continue...
      if(getFirstPageReturned == OK)
	{
	
	  //Next, it reads and pins the header page for the file in the buffer pool...
	  readPageReturned = bufMgr->readPage(filePtr, headerPageNo, pagePtr);

	  //Check what readPage returned
	  //Went ok so continue
	  if(readPageReturned == OK)
	    {

	      //...initializing the private data members headerPage, headerPageNo, 
	      //and hdrDirtyFlag
	      headerPage = (FileHdrPage*)pagePtr;
	      curPageNo = headerPage->firstPage;
	      hdrDirtyFlag = false;
            
	      //No records yet
	      curRec = NULLRID;
	      
	      //Finally, read and pin the first page of the file into the buffer pool,
	      //initializing the values of curPage, curPageNo, and curDirtyFlag
	      //appropriately
	      returnStatus = bufMgr->readPage(filePtr, curPageNo, curPage);
	  
	      //This causes a seg fault?? But good practice to close file...
	      //db.closeFile(filePtr);
	      return;
	    }

	  //Bump up error from call to readPage
	  else
	    {
	      returnStatus = readPageReturned;
	      return;
	    }
	}
      
      //Bump up error from call to getFirstPage
      else
	{
	  returnStatus = getFirstPageReturned;
	  return;
	}
    }
    
    //Bump up error from call to openFile
    else
    {
    	cerr << "open of heap file failed\n";
		returnStatus = openFileReturned;
		return;
    }
}

// the destructor closes the file
HeapFile::~HeapFile()
{
    Status status;
    cout << "invoking heapfile destructor on file " << headerPage->fileName << endl;

    // see if there is a pinned data page. If so, unpin it 
    if (curPage != NULL)
    {
    	status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);
		curPage = NULL;
		curPageNo = 0;
		curDirtyFlag = false;
		if (status != OK) cerr << "error in unpin of date page\n";
    }
	
	 // unpin the header page
    status = bufMgr->unPinPage(filePtr, headerPageNo, hdrDirtyFlag);
    if (status != OK) cerr << "error in unpin of header page\n";
	
	// status = bufMgr->flushFile(filePtr);  // make sure all pages of the file are flushed to disk
	// if (status != OK) cerr << "error in flushFile call\n";
	// before close the file
	status = db.closeFile(filePtr);
    if (status != OK)
    {
		cerr << "error in closefile call\n";
		Error e;
		e.print (status);
    }
}

// Return number of records in heap file

const int HeapFile::getRecCnt() const
{
  return headerPage->recCnt;
}

// retrieve an arbitrary record from a file.
// if record is not on the currently pinned page, the current page
// is unpinned and the required page is read into the buffer pool
// and pinned.  returns a pointer to the record via the rec parameter

/**
 * This method returns a record (via the rec structure) given the RID 
 * of the record. The private data members curPage and curPageNo should 
 * be used to keep track of the current data page pinned in the buffer 
 * pool. If the desired record is on the currently pinned page, simply 
 * invoke curPage->getRecord(rid, rec) to get the record. Otherwise, 
 * you need to unpin the currently pinned page (assuming a page is pinned) 
 * and use the pageNo field of the RID to read the page into the buffer pool
 *
 * @param rid Record id
 * @param rec Record
 * @return Status
 */
const Status HeapFile::getRecord(const RID &  rid, Record & rec)
{
    //Status status; I like descriptive names

    Status getRecordReturned;
    Status unPinPageReturned;
    Status readPageReturned;
    
    //This method returns a record (via the rec structure) given the RID of the record.
    //The private data members curPage and curPageNo should be used to keep track of 
    //the current data page pinned in the buffer pool.

    // cout<< "getRecord. record (" << rid.pageNo << "." << rid.slotNo << ")" << endl; 
   
    //If the desired record is on the currently pinned page, simply invoke 
    //curPage->getRecord(rid, rec) to get the record.
    if(rid.pageNo == curPageNo)
      {
	//Obtain the record
	getRecordReturned = curPage->getRecord(rid, rec);
	
	//Check what getRecord returned
	//Went fine so keep going
	if(getRecordReturned == OK)
	  {
	    return getRecordReturned;
	  }

	//Bump up error from getRecord
	else
	  {
	    return getRecordReturned;
	  }
      }

    //Otherwise, you need to unpin the currently pinned page (assuming a page is 
    //pinned)...
    //The record isn't on this page in a particular file
    else
      {
	//Unpin the page
	unPinPageReturned = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);

	//Check what unPinPage returned
	if(unPinPageReturned == OK)
	  {
	    //...and use the pageNo field of the RID to read the page into the
	    //buffer pool
	    readPageReturned = bufMgr->readPage(filePtr, rid.pageNo, curPage);

	    //Check what readPageReturned
	    if(readPageReturned == OK)
	      {
		//Give this new page in the buffer pool the RID  pageNo
		curPageNo = rid.pageNo;
        
		//Return the record via readPageReturned
		readPageReturned = curPage->getRecord(rid,rec);

		return readPageReturned;
	      }
	    
	    //Bump up error from readPage
	    else
	      {
		return readPageReturned;
	      }
	  }

	//Bump up error from unPinPage 
	else
	  {
	    return unPinPageReturned;
	  }
      }
}

HeapFileScan::HeapFileScan(const string & name,
			   Status & status) : HeapFile(name, status)
{
    filter = NULL;
}

const Status HeapFileScan::startScan(const int offset_,
				     const int length_,
				     const Datatype type_, 
				     const char* filter_,
				     const Operator op_)
{
    if (!filter_) {                        // no filtering requested
        filter = NULL;
        return OK;
    }
    
    if ((offset_ < 0 || length_ < 1) ||
        (type_ != STRING && type_ != INTEGER && type_ != FLOAT) ||
        ((type_ == INTEGER && length_ != sizeof(int))
         || (type_ == FLOAT && length_ != sizeof(float))) ||
        (op_ != LT && op_ != LTE && op_ != EQ && op_ != GTE && op_ != GT && op_ != NE))
    {
        return BADSCANPARM;
    }

    offset = offset_;
    length = length_;
    type = type_;
    filter = filter_;
    op = op_;

    return OK;
}


const Status HeapFileScan::endScan()
{
    Status status;
    // generally must unpin last page of the scan
    if (curPage != NULL)
    {
        status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);
        curPage = NULL;
        curPageNo = 0;
		curDirtyFlag = false;
        return status;
    }
    return OK;
}

HeapFileScan::~HeapFileScan()
{
    endScan();
}

const Status HeapFileScan::markScan()
{
    // make a snapshot of the state of the scan
    markedPageNo = curPageNo;
    markedRec = curRec;
    return OK;
}

const Status HeapFileScan::resetScan()
{
    Status status;
    if (markedPageNo != curPageNo) 
    {
		if (curPage != NULL)
		{
			status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);
			if (status != OK) return status;
		}
		// restore curPageNo and curRec values
		curPageNo = markedPageNo;
		curRec = markedRec;
		// then read the page
		status = bufMgr->readPage(filePtr, curPageNo, curPage);
		if (status != OK) return status;
		curDirtyFlag = false; // it will be clean
    }
    else curRec = markedRec;
    return OK;
}

/**
 * Returns (via the outRid parameter) the RID of the next record that 
 * satisfies the scan predicate. The basic idea is to scan the file one 
 * page at a time. For each page, use the firstRecord() and nextRecord() 
 * methods of the Page class to get the rids of all the records on the 
 * page. Convert the rid to a pointer to the record data and invoke matchRec() 
 * to determine if record satisfies the filter associated with the scan.
 * If so, store the rid in curRec and return curRec. To make things fast, 
 * keep the current page pinned until all the records on the page have been
 * processed. Then continue with the next page in the file. Since the 
 * HeapFileScan class is derived from the HeapFile class, it also has all 
 * the methods of the HeapFile class as well. Returns OK if no errors occured. 
 * Otherwise, return the error code of the first error that occurred.
 *
 * @param outRid 
 * @return Status
 */
const Status HeapFileScan::scanNext(RID& outRid)
{
    Status 	status = OK; 
    RID		nextRid;
    RID		tmpRid;
    int 	nextPageNo;
    Record      rec;

    bool notDoneEver = true;
    bool endOfFile = false;

    Status unPinPageReturned;
    Status readPageReturned;
    Status getNextPageReturned;

    //Scan file one page at a time
    //Need to actually get the first page
    nextPageNo = curPageNo;
    
    //The basic idea is to scan the file one page at a time
    //So basically loop for "forever" through pages until a return is hit
    //Also will probably need another loop to loop through records...
    while(notDoneEver)
      {
	//Make sure not at end of file already
	if (nextPageNo == -1) 
	  {
	    return FILEEOF;
	  }
	
	//Being process for scanning one page at a time in a file by unpinning this page to allow
	//reading/pinning of next page
	unPinPageReturned = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);
	
	//Make sure unpinning of this page went ok
	//Unpinning went fine
	if(unPinPageReturned == OK)
	  {
	    //Read in the next new page from this file
	    readPageReturned = bufMgr->readPage(filePtr, nextPageNo, curPage);
	    
	    //Make sure reading in of this next new page went ok
	    //Reading in went fine
	    if(readPageReturned == OK)
	      {
		//Update previous this current page stats to this previous next page stats
		curPageNo = nextPageNo;
		curDirtyFlag = false;

		//Use the firstRecord()...
		//First read of record on this new page
		if(curRec.pageNo == -1) 
		  {
		    status = curPage->firstRecord(nextRid);
		  } 
		
		//Did reading of first record on this new page in this file go ok?
		//Yes it did
		//if(firstRecordReturned == OK)
		//Have to use status. need one variable ONLY for following logic
		if(status == OK)
		  {
		    //...and the nextRecord() methods of the Page class to get the rids of
		    //all the records on the page
		    //No longer a first record read
		    if(curRec.pageNo != -1) 
		      {
			status = curPage->nextRecord(curRec, nextRid);
			tmpRid = nextRid;
		      }
		    
		    //If getting the nextRecord on this new page in this file went ok
		    //nextRecord went fine
		    if(status == OK)
		      { 
			//Now need to loop through records, convert them, and compare
			while(notDoneEver && !endOfFile) 
			  {
			    //Convert the rid to a pointer to the record data and...
			    status = curPage->getRecord(nextRid, rec);
			    
			    //Check conversion
			    //Conversion went fine
			    if(status == OK)
			      {
				//...invoke matchRec() to determine if record satisfies
				//the filter associated with the scan. If so, store the rid
				//in curRec and return curRec
				if(matchRec(rec))
				  {
				    curRec = nextRid;
				    outRid = curRec;
				    return OK;
				  }
				
				tmpRid = nextRid;
				
				//Try to obtain the next record for purposes of 
				//directing whether to get a new page or not and
				//just continue converting/comparing
				status = curPage->nextRecord(tmpRid, nextRid);
			
				//At end of page, break from this loop to get to
				//outer loop to get a new page if there is a next page
				if(status == ENDOFPAGE) 
				  {
				    curRec = NULLRID;
				    //endOfFile = true; 
				    break;
				  }
			      }
			    
			    //Bump up error from getRecord (error w/ conversion)
			    else
			      {
				return status;
			      }
			}
		      }
		    
		    //Bump up error from getting nextRecord
		    else if(status != ENDOFPAGE) 
		      {
			return status;
		      }

		    //There wasn't a next record so loop back and get a new page from
		    //this file
		    else
		      {
			curRec = NULLRID;
		      }
		  }
		
		//Attempt to get the next page in this file
		getNextPageReturned = curPage->getNextPage(nextPageNo);
		
		//If there is a next page
		if(getNextPageReturned == OK) 
		  {
		    
		  }

		//Otherwise at the end of the file (no more pages)
		else
		  {
		    return getNextPageReturned;
		  }
	      }

	    //Bump up error from readPage
	    else
	      {
		return readPageReturned;
	      }
	  }
	
	//Bump up error from unPinPage
	else
	  {
	    return unPinPageReturned;
	  }
      }

    return OK;
}


// returns pointer to the current record.  page is left pinned
// and the scan logic is required to unpin the page 

const Status HeapFileScan::getRecord(Record & rec)
{
    return curPage->getRecord(curRec, rec);
}

// delete record from file. 
const Status HeapFileScan::deleteRecord()
{
    Status status;

    // delete the "current" record from the page
    status = curPage->deleteRecord(curRec);
    curDirtyFlag = true;

    // reduce count of number of records in the file
    headerPage->recCnt--;
    hdrDirtyFlag = true; 
    return status;
}


// mark current page of scan dirty
const Status HeapFileScan::markDirty()
{
    curDirtyFlag = true;
    return OK;
}

const bool HeapFileScan::matchRec(const Record & rec) const
{
    // no filtering requested
    if (!filter) return true;

    // see if offset + length is beyond end of record
    // maybe this should be an error???
    if ((offset + length -1 ) >= rec.length)
	return false;

    float diff = 0;                       // < 0 if attr < fltr
    switch(type) {

    case INTEGER:
        int iattr, ifltr;                 // word-alignment problem possible
        memcpy(&iattr,
               (char *)rec.data + offset,
               length);
        memcpy(&ifltr,
               filter,
               length);
        diff = iattr - ifltr;
        break;

    case FLOAT:
        float fattr, ffltr;               // word-alignment problem possible
        memcpy(&fattr,
               (char *)rec.data + offset,
               length);
        memcpy(&ffltr,
               filter,
               length);
        diff = fattr - ffltr;
        break;

    case STRING:
        diff = strncmp((char *)rec.data + offset,
                       filter,
                       length);
        break;
    }

    switch(op) {
    case LT:  if (diff < 0.0) return true; break;
    case LTE: if (diff <= 0.0) return true; break;
    case EQ:  if (diff == 0.0) return true; break;
    case GTE: if (diff >= 0.0) return true; break;
    case GT:  if (diff > 0.0) return true; break;
    case NE:  if (diff != 0.0) return true; break;
    }

    return false;
}

InsertFileScan::InsertFileScan(const string & name,
                               Status & status) : HeapFile(name, status)
{
  //Do nothing. Heapfile constructor will read the header page and the first
  // data page of the file into the buffer pool
}

InsertFileScan::~InsertFileScan()
{
    Status status;
    // unpin last page of the scan
    if (curPage != NULL)
    {
        status = bufMgr->unPinPage(filePtr, curPageNo, true);
        curPage = NULL;
        curPageNo = 0;
        if (status != OK) cerr << "error in unpin of data page\n";
    }
}

// Insert a record into the file

/**
 * Inserts the record described by rec into the file returning the RID 
 * of the inserted record in outRid. First, check to see if the current 
 * page is the last page of the file. If it is, try to insert into the 
 * current page. If the last page is full then allocate a new page and 
 * insert the record into the new page (make sure that you update the 
 * header page appropriately; you also have to make the newly allocated 
 * page into the current page; further, you have to pin and unpin pages 
 * appropriately).
 * If the current page is not the last page, bring the last page of the 
 * file into the buffer pool, make it the current page and follow the 
 * same procedure.
 *
 * @param rec Record
 * @param outRid Record id
 * @return Status
 */
const Status InsertFileScan::insertRecord(const Record & rec, RID& outRid)
{
    Page*	newPage;
    int		newPageNo;
    Status	unpinstatus;
    RID		rid;

    Status insertRecordReturned;
    Status allocPageReturned;
    Status insertRecord2Returned;
    Status readPageReturned;
    Status insertRecord3Returned;
    Status allocPage2Returned;
    Status insertRecord4Returned;


    // check for very large records
    if ((unsigned int) rec.length > PAGESIZE-DPFIXED)
    {
        // will never fit on a page, so don't even bother looking
        return INVALIDRECLEN;
    }
  
    //Inserts the record described by rec into the file returning the RID of
    //the inserted record in outRid.
  
    //First, check to see if the current page is the last page of the file
    //If it is, try to insert into the current page.
    if(curPageNo == headerPage->lastPage)
      {
      	//If the last page is full, then allocate a new page and insert the
	//record into the new page (make sure that you update the header
	//page appropriately; you also have to make the newly allocated page 
	//into the current page; further, you have to pin and unpin pages
	//appropriately
	
	//Trying to insert into current page...
	insertRecordReturned = curPage->insertRecord(rec, outRid);

	//Last page is full, then...
	if(insertRecordReturned == NOSPACE)
	  {
	    //...allocate new page and...
	    allocPageReturned = bufMgr->allocPage(filePtr, newPageNo, newPage);
	    
	    //Check status of allocating new page
	    if(allocPageReturned == OK)
	      {
		//Allocating the new page went ok so finalize the new page
		newPage->init(newPageNo);

		//Unpin the current page to release it (Buffer Manager side of things)
		unpinstatus = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);

		//Check status of unpinning current page
		if(unpinstatus == OK)
		  {
		    //Set the nextPage from the curPage (Linked List side of things)
		    curPage->setNextPage(newPageNo);

		    //Now the (new) currentPage's info can be updated 
		    curPageNo = newPageNo;
		    curPage = newPage;
		    curDirtyFlag = false;

		    //headerPage clean up (on LL side of things)
		    headerPage->lastPage = curPageNo;
		    headerPage->pageCnt = headerPage->pageCnt + 1;
		    hdrDirtyFlag = true;
	    
		    //Finally, insert the record into the last page 
		    outRid = rid;
		    insertRecord2Returned = curPage->insertRecord(rec, outRid);

		    //Check status of inserting the record
		    if(insertRecord2Returned == OK)
		      {
			return insertRecord2Returned;
		      }
	    
		    //Bump up error from insertRecord
		    else
		      {
			return insertRecord2Returned;
		      }
		  }

		//Bump up error from unPinPage
		else
		  {
		    return unpinstatus;
		  }
	      }

	    //Bump up error from allocPage
	    else
	      {
		return allocPageReturned;
	      }
	  }

	//The last page wasn't full so simply just update headerPage info and
	//record id info
	else
	  {
	    headerPage->recCnt = headerPage->recCnt + 1;
	    curRec = outRid;
	    curDirtyFlag = true;
	
	    return insertRecordReturned;
	  }
      }

    //If the current page is not the last page, bring the last page of the file into
    //the buffer pool, make it the current page and follow the same procedure
    else
      {
	//Current page is not the last page in file so unpin to begin process
	//of getting last page
	unpinstatus = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);
	
	//Check unpinning of this current page that isn't the last page
	if(unpinstatus == OK)
	  {
	    //Get the last page and set it as the current page
	    readPageReturned = bufMgr->readPage(filePtr, headerPage->lastPage, curPage);
	    
	    if(readPageReturned == OK)
	      {

		//Update current page number from header
		curPageNo = headerPage->lastPage;

		////////////////////////////////
		//And now the same procedure....
		////////////////////////////////

		//Trying to insert into current page...
		insertRecord3Returned = curPage->insertRecord(rec, outRid);

		//There is no space
		if(insertRecord3Returned == NOSPACE)
		  {
		    //Allocate new page from buffer manager
		    allocPage2Returned = bufMgr->allocPage(filePtr, newPageNo, newPage);

		    //Check status of allocating new page
		    if(allocPage2Returned == OK)
		      {

			//Allocating the new page went ok so finalize the new page
			newPage->init(newPageNo);

			//Unpin the current page to release it (Buffer Manager side of things)
			unpinstatus = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);

			//Check status of unpinning current page
			if(unpinstatus == OK)
			  {
			    //Set the nextPage from the curPage (Linked List side of things)
			    curPage->setNextPage(newPageNo);

			    //Now the (new) currentPage's info can be updated
			    curPageNo = newPageNo;
			    curPage = newPage;
			    curDirtyFlag = false;

			    //headerPage clean up (on LL side of things)
			    headerPage->lastPage = curPageNo;
		    
			    //Now the (new) currentPage's info can be updated
			    curPageNo = newPageNo;
			    curPage = newPage;
			    curDirtyFlag = false;

			    //headerPage clean up (on LL side of things)
			    headerPage->lastPage = curPageNo;
			    headerPage->pageCnt = headerPage->pageCnt + 1;
			    hdrDirtyFlag =  true;

			    //Finally, insert the record into the last page
			    outRid = rid;
			    insertRecord4Returned = curPage->insertRecord(rec, outRid);

			    //Check status of inserting the record
			    if(insertRecord4Returned == OK)
			      {
				return insertRecord4Returned;
			      }

			    //Bump up error from insertRecord
			    else
			      {
				return insertRecord4Returned;
			      }
			  }

			//Bump up error from unPinPage
			else
			  {
			    return unpinstatus;
			  }
		      }

		    //Bump up error from allocPage
		    else
		      {
			return allocPage2Returned;
		      }
		  }

		//The last page wasn't full so simply just update headerPage info and
		//Record id info
		else
		  {
		    headerPage->recCnt = headerPage->recCnt + 1;
		    curRec = outRid;
		    curDirtyFlag = true;

		    return insertRecord3Returned;
		  }
	      }
	  }
	  
	//Bump up error from unPinPage
	else
	  {
	    return unpinstatus;
	  }
      }

    return OK;
}


