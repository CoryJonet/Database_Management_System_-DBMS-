SELECT STATE.NAME STATE_NAME, --State name
       COUNT( COUNTY.CTYNAME )  COUNT_OF_COUNTIES --Count of every county in a state
  FROM POP_ESTIMATE_NATION_STATE_PR STATE, 
       POP_ESTIMATE_STATE_COUNTY COUNTY
 WHERE STATE.STATE = COUNTY.STATE 
       AND
       STATE.NAME != 'Puerto Rico Commonwealth' --No Puerto Rico
       AND
       COUNTY.CTYNAME != COUNTY.STNAME --County table has state with state entries and don't want that
 GROUP BY STATE.NAME;
