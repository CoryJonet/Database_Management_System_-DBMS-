///////////////////////////////////////////////////////////////////////////////
//                   ALL STUDENTS COMPLETE THESE SECTIONS
// Main Class File:  (name of main application class)
// File:             buf.cpp
// Semester:         CS564 Fall 2014
//
// Author:           Cory Jonet (jonet@wisc.edu)
// CS Login:         jonet
// Lecturer's Name:  Professor Naughton
//
//                   STUDENTS WHO GET HELP FROM ANYONE OTHER THAN THEIR PARTNER
// Credits:          No one
//////////////////////////// 80 columns wide //////////////////////////////////

#include <memory.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include "page.h"
#include "buf.h"

#define ASSERT(c)  { if (!(c)) { \
		       cerr << "At line " << __LINE__ << ":" << endl << "  "; \
                       cerr << "This condition should hold: " #c << endl; \
                       exit(1); \
		     } \
                   }

//----------------------------------------
// Constructor of the class BufMgr
//----------------------------------------

BufMgr::BufMgr(const int bufs)
{
    numBufs = bufs;

    bufTable = new BufDesc[bufs];
    memset(bufTable, 0, bufs * sizeof(BufDesc));
    for (int i = 0; i < bufs; i++) 
    {
        bufTable[i].frameNo = i;
        bufTable[i].valid = false;
    }

    bufPool = new Page[bufs];
    memset(bufPool, 0, bufs * sizeof(Page));

    int htsize = ((((int) (bufs * 1.2))*2)/2)+1;
    hashTable = new BufHashTbl (htsize);  // allocate the buffer hash table

    clockHand = bufs - 1;
}

/**
 * ~BufMgr() flushes out all dirty pages and deallocates the buffer pool and
 * the BufDesc table.
 */
BufMgr::~BufMgr() {
	// TODO: Implement this method by looking at the description in the writeup.
	
	//Need to allocate a temporary array in order to "flush all dirty pages" or write the dirty pages to disk
	BufDesc* tempArray;

	int i; //Counter for looping through buffer frames' descriptions

	i = 0;

	//Begin process for looping through pool of buffer frames' descriptions up to numBufs
	while(i < numBufs)
	{ 
		tempArray = &(bufTable[i]);
		
		//Make sure these pages that are being written back to disk are "dirty" i.e. different than page on disk
		//then "write to disk" or just a UNIX file.
		if(tempArray->dirty == true)
		{
			tempArray->file->writePage(tempArray->pageNo, &(bufPool[i]));
		}

		i++;
	}
	
	//"Deallocate the buffer pool and the BufDesc table" or basically delete everything
	delete [] bufPool;
	delete [] bufTable;
}


/**
 * allocBuf allocates a free frame using the clock algorithm; if necessary, writing
 * a dirty page back to disk. If the buffer frame allocated has a valid page in it,
 * then the appropriate entry is removed from the hash table. Called by readPage() 
 * and allocPage() functions
 * @param frame the frame in question
 * @return Returns BUFFEREXCEEDED if all buffer frames are pinned, UNIXERR if the 
 * call to the I/O layer returned an error when a dirty page was being written, or
 * OK otherwise
 */
const Status BufMgr::allocBuf(int & frame) {
	// TODO: Implement this method by looking at the description in the writeup.
 	
        bool notDoneEver = true;

        BufDesc* currentPosition;

	uint start = clockHand;

	bool isBufferFull = true;

	Status flushReturned = OK;
	Status removeReturned = OK;
	
	//Assume we loop forever as is asserted by the clock algorithm. Until replacement
	//found of course
	while(notDoneEver)
	{
		//Step One: Advance the "clock"
		advanceClock();
		
		//Give the pointer the "clock hand" position
		currentPosition = &(bufTable[clockHand]);
	      
		//if((isBufferFull == true || isBufferFull == false) && clockHand != start)
		//{
			//Conditional: Is valid set?
			if(currentPosition->valid == true)
			{
			
				//Conditional: Is refbit set?
				if(currentPosition->refbit == true)
				{
					//Refbit was set so clear it and start over
					currentPosition->refbit = false;
				}
				
				//Refbit wasn't set
				else
				{
				        //So I had a hard time figuring where to put this logic
				        //but if buffer is determined to be full after the clock
				        //rotation and it's back at the start, then a full buffer...but I figured it 
				        //needed to be by the pinCnt checking logic...
				        //If Buffer full of pinned pages? Otherwise continue on
				        if(start == clockHand && isBufferFull == true)
					{
					        //Buffer is full
					        return BUFFEREXCEEDED;
					}

					//Conditional: Is the page pinned?
					//No then continue
					else if(currentPosition->pinCnt == 0)
					{
						
						//Have at least one page so no BUFFEREXCEEDED
						isBufferFull = false;
						
						//Page wasn't pinned
						//Conditional: Is the dirty bit set?
						if(currentPosition->dirty == true)
						{
							//Flush Page to disk
							flushReturned = currentPosition->file->writePage(currentPosition->pageNo, &(bufPool[clockHand]));
							
							//Flush to disk was fine
							if(flushReturned == OK)
							{
								//Remove frame page from hashtable
								removeReturned = hashTable->remove(currentPosition->file, currentPosition->pageNo);
								
								//If removal of frame's page Ok, return
								//if(removeReturned == OK)
								//{
									//Invoke Set() on Frame and Use frame
								        //Ok I spent a while figuring out this issue but why does .Set cause a seg fault???
								        //program works without it???
									//bufPool[frame].Set(currentPosition->file, currentPosition->pageNo);
								        frame = currentPosition->frameNo;
									
									return OK;
									
								//}
								
								//Else there was a problem removing page from new frame...
								//Not in spec but added in case...
								//else
								//{
								//	return HASHTBLERROR;
								//}
								
						         }
							
							//Flush to disk caused an error on UNIX side...
							else
							{
								return UNIXERR;
						        }
						}
						
						//Don't flush page to disk but continue on...
						else
						{
							//Remove frame page from hashtable
							removeReturned = hashTable->remove(currentPosition->file, currentPosition->pageNo);
								
							//If removal of frame's page Ok, return
							//if(removeReturned == OK)
							//{
								//Invoke Set() on Frame and Use frame. Set didn't work? caused set fault...
								//bufTable[frame].Set(currentPosition->file, currentPosition->pageNo);
								frame = currentPosition->frameNo;      
								return OK;
								
							//}
					       
							//Else there was a problem removing page from new frame...								
							//Not in spec but added in case...
							//else
							//{
							//	return HASHTBLERROR;
							//}
						}
					}    
				}
			}
			
			//If valid is false: Invoke Set() on Frame and use frame
			else
			{
			        //bufTable[frame].Set(currentPosition->file, currentPosition->pageNo);
			        frame = currentPosition->frameNo;
				return OK;
			}
		//}
		
		//Back at beginning and all pages pinned so BUFFEREXCEEDED
		//else
		//{
		//	return BUFFEREXCEEDED;
		//}
	}

	//Will never get here but compiler was throwing a warning...
	return OK;
}

/**
 * readpage first checks whether the page is already in the buffer pool by invoking
 * invoking the lookup() method on the hashtable to get a frame number. There are two 
 * cases to be handled depending on the outcome of the lookup() call. Case 1: Page is 
 * not in the buffer pool. Calls allocBuf() to allocate a buffer frame and then calls 
 * the method file->readPage() to read the page from disk into the buffer pool frame. 
 * Next, inserts the page into the hashtable. Finally, invokes Set() on the frame to 
 * set it up properly. Set() will leave the pinCnt for the page set to 1. 
 * Return a pointer to the frame containing the page via the page parameter.
 * Case 2: Page is in the buffer pool. In this case set the appropriate refbit, 
 * increment the pinCnt for the page, and then return a pointer to the frame containing 
 * the page via the page parameter
 *
 * @param file the file with pages
 * @param pageNo page number in directory
 * @param page the page
 * @return Returns OK if no errors occurred, UNIXERR if a UNIX error occurred, 
 * BUFFEREXCEEDED if all buffer frames are pinned, or HASHTBLERROR if a hash error occurred
 */
const Status BufMgr::readPage(File* file, const int PageNo, Page*& page) {
	// TODO: Implement this method by looking at the description in the writeup.
	
        Status hashLookUpReturned = OK; //Hash look up returned what?
	Status allocBufReturned = OK;   //Allocating buffer returned what?
	Status readPageReturned = OK;   //Reading page returned what?
	Status insertReturned = OK;     //Inserting returned what?
	
	int frame; //Temp frame
	
	//Check whether the page is already in the buffer pool by invoking the lookup() method on the hashtable to get a frame number
	hashLookUpReturned = hashTable->lookup(file,PageNo,frame);
	
	//CASE 1: Page isn't in buffer pool
	if(hashLookUpReturned == HASHNOTFOUND)
	{
		//Call allocBuf()
		allocBufReturned = allocBuf(frame);
		
		//Call to allocBuf returned fine
		if(allocBufReturned == OK)
		{
			//Call readPage
			readPageReturned = file->readPage(PageNo, &(bufPool[frame]));
			
			//Call to readPage returned fine
			if(readPageReturned == OK)
			{	
				//Call insert
				insertReturned = hashTable->insert(file, PageNo, frame);
				
				//Call to insert returned fine
				if(insertReturned == OK)
				{
					//Invoke Set() on the frame and return a pointer to the frame containing
					//the page via a page parameter and return OK
					bufTable[frame].Set(file, PageNo);
					page = &(bufPool[frame]);
					return OK;
				}
				
				//Something wrong with insert into hash table...
				else
				{
					return HASHTBLERROR;
				}
			}
			
			//Something wrong on UNIX side when using file function...
			else
			{
				return UNIXERR;
			}
		}
		
		//Something wrong with allocBuf, BUFFEREXCEEDED
		else if(allocBufReturned == BUFFEREXCEEDED)
		{
		       return BUFFEREXCEEDED;
		}
		
		//Something wrong with allocBuf, UNIXERR
		else
		{
		       return UNIXERR;
		}
	}
	
	//CASE 2: Page is in the buffer pool (hashLookUpReturned == OK)
	else
	{
		//Set appropriate refbit, increment pinCnt for the page, return 
		//pointer to frame containing page and return OK
		bufTable[frame].refbit = true;
		bufTable[frame].pinCnt = bufTable[frame].pinCnt + 1;
		page = &(bufPool[frame]);
		return OK;
		
	}
}

/**
 * unPinPage decrements the pinCnt of the frame containing (file, PageNo)
 * and, if dirty == true, sets the dirty bit. 
 *
 * @param file file with pages
 * @param pageNo page number in directory
 * @param dirty is page dirty? or not?
 * @return OK if no errors occurred, HASHNOTFOUND if the page is not in 
 * the buffer pool hash table, PAGENOTPINNED if the pin count is already 0
 */
const Status BufMgr::unPinPage(File* file, const int PageNo, 
			       const bool dirty) {
	// TODO: Implement this method by looking at the description in the writeup.
	
	Status hashLookUpReturned; //Status of the hash look up
	
	int frame; //Frame to "return"
	
	hashLookUpReturned = hashTable->lookup(file, PageNo, frame);
	
	//If there was a hash and page exists
	if(hashLookUpReturned == OK)
	{
		//Returns PAGENOTPINNED if the pin count is already 0 so no computations wasted
	        // and no negative pin count (not sure what would happen with that)
		if(bufTable[frame].pinCnt == 0)
		{
			return PAGENOTPINNED;
		}
		
		//Set dirty bit if dirty is true
		if(dirty == true)
		{
		        bufTable[frame].dirty = true;
		}

		//Decrement the pinCnt of the frame
		bufTable[frame].pinCnt = (bufTable[frame].pinCnt - 1);	

		return OK;
	}
	
	//Returns HASHNOTFOUND if the page is not in the buffer pool hash table
	else
	{
		return HASHNOTFOUND;
	}
}

/**
 * allocPage first allocates an empty page in the specified file by invoking 
 * the file->allocatePage() method. This function will return the page number 
 * of the newly allocated page. Then allocBuf() is called to obtain a buffer 
 * pool frame. Next, an entry is inserted into the hash table and Set() is 
 * invoked on the frame to set it up properly. The function returns both the 
 * page number of the newly allocated page to the caller via the pageNo parameter
 * and a pointer to the buffer frame allocated for the page via the page parameter
 *
 * @param file file with pages
 * @param pageNo page number in directory
 * @param page the page
 * @return OK if no errors occurred, UNIXERR if a UNIX error occurred, BUFFEREXCEEDED
 * if all buffer frames are pinned, or HASHTBLERROR if a hash table error occurred
 */
const Status BufMgr::allocPage(File* file, int& pageNo, Page*& page)  {
	// TODO: Implement this method by looking at the description in the writeup.
	
        Status allocPageReturned; //Allocating a page returned what?
	Status allocBufReturned;  //Allocating buffer returned what?
	Status insertReturned;    //Inserting returned what?
	
	int frame; //Frame to "return"
	
	//Allocate an empty page in the specified file by invoking the file->allocatePage() method
        allocPageReturned = file->allocatePage(pageNo);
	
	//allocPage returned OK
	if(allocPageReturned == OK)
	{
		//allocBuf() is called to obtain a buffer pool frame
		allocBufReturned = allocBuf(frame);
		
		//If allocating the frame was done without error
		if(allocBufReturned == OK)
		{
			//Insert entry into hashtable
			insertReturned = hashTable->insert(file, pageNo, frame);
			
			//If the entry is inserted into the hashtable successfully
			if(insertReturned == OK)
			{
				//Set() is invoked on the frame to set it up properly and
				//method returns a page number of the newly allocated page 
				//to the caller via the pageNo parameter and a pointer to the
				//buffer frame allocated for the page via the page parameter
				bufTable[frame].Set(file, pageNo);
				page = &(bufPool[frame]);
				return OK;
			}
			
			//Entry couldn't be inserted into hashtable
			else
			{
				return HASHTBLERROR;
			}
		}
		
		//Returns BUFFEREXCEEDED if all buffer frames are pinned
		else if(allocBufReturned == BUFFEREXCEEDED)
		{
			return BUFFEREXCEEDED;
		}
		
		//Returns UNIXERR if a Unix error occurred
		else
		{
			return UNIXERR;
		}
	}
	
	//allocPage didn't return OK so something wrong on UNIX side...
	else
	{
		return UNIXERR;
	}
}

/**
 * disposePage checks to see if the page actually exists in the buffer pool by 
 * looking it up in the hash table. If it does exists, clears the page, removes 
 * the corresponding entry from the hash table and disposes the page in the file 
 * as well. Returns the status of the call to dispose the page in the file.
 *
 * @param file file with pages
 * @param pageNo page number in directory
 * @return Returns HASHNOTFOUND if page doesn't exist, or HASHTBLERROR if
 * removal from hash table couldn't happen
 */
const Status BufMgr::disposePage(File* file, const int pageNo) {
	// TODO: Implement this method by looking at the description in the writeup.
	
	Status hashLookUpReturned; //Status of hash look up
	
	int frame; //Frame to clear
	
	//Check to see if the page actually exists in the buffer pool
	hashLookUpReturned = hashTable->lookup(file, pageNo, frame);
	
	//Page exists
	if(hashLookUpReturned == OK)
	{
		//If it does exists, clear the page
		bufTable[frame].Clear();
		
		//Remove corresponding entry from the hash table
		hashLookUpReturned = hashTable->remove(file, pageNo);
	
		//Couldn't remove entry from hash table
		if(hashLookUpReturned != OK)
		{
			return HASHTBLERROR;
		}
		
		//Return the status of the call to dispose the page in the file
		return file->disposePage(pageNo);
	}
	
	//Page doesn't exist
	else
	{
		return HASHNOTFOUND;
	}
}

/**
 * flushFile is called by DB::closeFile() when all instances of a 
 * file have been closed (in which case all pages of the file should 
 * have been unpinned). flushFile() should scan bufTable for pages 
 * belonging to the file. For each page encountered it should:
 * a) if the page is dirty, call file->writePage() to flush the page to 
 * disk and then set the dirty bit for the page to false
 * b) remove the page from the hashtable (whether the page is clean or dirty)
 * c) invoke the Clear() method on the page frame
 *
 * @param file
 * @return OK if no errors occurred, PAGEPINNED if some page of the file
 * is pinned, or HASHTBLERROR if error removing hash
 */
const Status BufMgr::flushFile(const File* file) {
        // TODO: Implement this method by looking at the description in the writeup.

        BufDesc* pageToReturn; //Intermediate pointer to find page to return
        Status writePageReturned; //Writing a page returned what?
	Status removeReturned;    //Removing a page returned what?
	
        int i; //Loop through buffer

	i = 0;

	//Begin looping through bufTable to look for pages belonging to the File* file
	while (i < numBufs) 
	{
		pageToReturn = &(bufTable[i]);
		
		//Think we want to check if the specific page is pinned here...so computations aren't wasted
		if(pageToReturn->pinCnt > 0)
		{
		      return PAGEPINNED;
		}
			
		//a) Check if dirty bit is set for the page
		if(pageToReturn->dirty == true)
		{
		      //Call writePage to flush the page to disk and set dirty bit
		      writePageReturned = pageToReturn->file->writePage(pageToReturn->pageNo, &(bufPool[i]));
				
		      //Good practice to make sure that writePage returned 0
		      if(writePageReturned != OK)
		      {
		            pageToReturn->dirty = false;
		      }
				
		}
			
		//b) Remove the page from the hashtable (whether the page is clean or dirty)
		hashTable->remove(file, pageToReturn->pageNo);
			
		//If removal from hash table was ok
		if(removeReturned != HASHTBLERROR)
		{
		      //Lastly, c) invoke Clear() method on this page frame
		      pageToReturn->Clear();
		}
			
		//Removal from hash table wasn't ok
		else
		{
		      return HASHTBLERROR;
	        }
	      	
		i++;
	}
	
	//If everything turned out OK
	return OK;
}


void BufMgr::printSelf(void) {
    BufDesc* tmpbuf;
  
    cout << endl << "Print buffer...\n";
    for (int i=0; i<numBufs; i++) {
        tmpbuf = &(bufTable[i]);
        cout << i << "\t" << (char*)(&bufPool[i]) 
             << "\tpinCnt: " << tmpbuf->pinCnt;
    
        if (tmpbuf->valid == true)
            cout << "\tvalid\n";
        cout << endl;
    };
}


