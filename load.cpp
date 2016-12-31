#include <unistd.h>
#include <fcntl.h>
#include "catalog.h"
#include "utility.h"


//
// Loads a file of (binary) tuples from a standard file into the relation.
// Any indices on the relation are updated appropriately.
//
// Returns:
// 	OK on success
// 	an error code otherwise
//

const Status UT_Load(const string & relation, const string & fileName)
{
  Status status;
  RelDesc rd;
  AttrDesc *attrs;
  int attrCnt;
  InsertFileScan * iFile;
  int width = 0;
  int records = 0;

  Status getRelInfoReturned;
  Status getInfoReturned;
  Status insertFileScanReturned;
 
  int i = 0;

  if (relation.empty() || fileName.empty() || relation == string(RELCATNAME)
      || relation == string(ATTRCATNAME))
    return BADCATPARM;

  // open Unix data file

  int fd;
  if ((fd = open(fileName.c_str(), O_RDONLY, 0)) < 0)
    return UNIXERR;

  // get relation data

  getInfoReturned = relCat->getInfo(relation, rd);
  
  //Check what getInfo() returned
  if(getInfoReturned == OK)
    {
      getRelInfoReturned = attrCat->getRelInfo(rd.relName, attrCnt, attrs);

      //Check what getRelInfo() returned
      if(getRelInfoReturned == OK)
	{
	  //Continue on...
	  // start insertFileScan on relatio
	  iFile = new InsertFileScan(rd.relName, insertFileScanReturned);

	  //Check what InsertFileScan() returned
	  if(insertFileScanReturned == OK)
	    {
	      while(i < attrCnt)
		{
		  width = width + attrs[i].attrLen;
		  i++;                                                                       
		}   
	    }

	  //Error using InsertFileScan()
	  else
	    {
	      return insertFileScanReturned;
	    }
	}

      //Error using getRelInfo()
      else
	{
	  //Don't continue
	  return getRelInfoReturned;
	}
    }

  //Error using getInfo()
  else
    {
      return getInfoReturned;
    }

  char *record;
  if(!(record = new char [width])) return INSUFMEM;

  //  int records = 0;
  int nbytes;
  Record rec;

  // read next input record from Unix file and insert it into relation
  while((nbytes = read(fd, record, width)) == width) {
    RID rid;
    rec.data = record;
    rec.length = width;
    if ((status = iFile->insertRecord(rec, rid)) != OK) return status;
    records++;
  }

  // close heap file and unix file
  if (close(fd) < 0) return UNIXERR;

  return OK;
}

