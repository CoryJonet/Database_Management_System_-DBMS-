#include <sys/types.h>
#include <functional>
#include <string.h>
#include <stdio.h>
using namespace std;

#include "error.h"
#include "utility.h"
#include "catalog.h"
#include "stdlib.h"
#include "iostream"
#include "iomanip"

// define if debug output wanted


//
// Retrieves and prints information from the catalogs about the for
// the user. If no relation is given (relation.empty() is true), then
// it lists all the relations in the database, along with the width in
// bytes of the relation, the number of attributes in the relation,
// and the number of attributes that are indexed.  If a relation is
// given, then it lists all of the attributes of the relation, as well
// as its type, length, and offset, whether it's indexed or not, and
// its index number.
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status RelCatalog::help(const string & relation)
{
  Status status;
  RelDesc rd;
  AttrDesc *attrs;
  int attrCnt;

  int i; //Loop counter
  Status getInfoReturned; //What did getInfo() return?
  Status getRelInfoReturned; //What did getRelInfo() return?
  string convertType; //Need to figure out what type attrType is

  i = 0;
  
  //If relation.empty() is true (empty() is a method on the string class)...
  if (relation.empty()) return UT_Print(RELCATNAME);

  //...,print (to standard output) a list of all the relations in relCat
  //including how many attributes it has). Otherwise, print all the 
  //tuples in attrCat that are relevant to <relName>.
  getInfoReturned = getInfo(relation, rd);
  
  //Check what getInfo() returned
  if(getInfoReturned == OK)
    {
      getRelInfoReturned = attrCat->getRelInfo(relation, attrCnt, attrs);

      //Check what getRelInfo() returned
      if(getRelInfoReturned == OK)
	{

	  //Bunch of formatting statements
	  printf("All Relations Currently in Catalog: \n\n");

	  printf("Name of Relation: %s | Number of Attributes: %i \n\n", rd.relName, rd.attrCnt);

	  printf("Attribute Name | Attribute Length | Attribute Type | Attribute Offset \n"); 

	  printf("_______________|__________________|________________|_________________\n");

	  //Now print each attr 
	  while(i < attrCnt)
 	  {

	    //Need to determine what variable type attrType is for each attr
	    //attr type is INTEGER
	    if(attrs[i].attrType == 1)
	      {
		convertType = "int";
	      }

	    //attr type is STRING
	    else if(attrs[i].attrType == 0)
	      {
		convertType = "string";
	      }

	    //attr type is FLOAT
	    else
	      {
		convertType = "float";
	      }

	    //Print out results with more formatting
	    std::cout << std::setw(8) <<  attrs[i].attrName << std::setw(20) << attrs[i].attrLen << 
	      std::setw(18) << convertType << std::setw(15) << attrs[i].attrOffset << std::endl;

	    i++;
	  }

	}
      
      //Error using getRelInfo()
      else
	{
	  return getRelInfoReturned;
	}
    }

  //Error using getInfo()
  else
    {
      return getInfoReturned;
    }
  
  //Everything printed ok
  return OK;  
}
