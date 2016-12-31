SELECT STATE.STATE STATE_CODE, --State Code
       STATE.BIRTHS2012 BIRTHS --Births
  FROM POP_ESTIMATE_NATION_STATE_PR STATE
 WHERE STATE.BIRTHS2012 > '80000' --Birth > 80,000
       AND
       STATE.STATE > '0' --No United State as whole or division
       AND
       STATE.NAME != 'Puerto Rico Commonwealth'; --No Puerto Rico
