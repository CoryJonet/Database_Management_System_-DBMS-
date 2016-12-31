SELECT STATE.NAME STATE_NAME, --State Name
       STATE.NETMIG2012 NET_MIGRATION --Net migration for each state
  FROM POP_ESTIMATE_NATION_STATE_PR STATE
 WHERE STATE.STATE > '0' --No United States as a whole or divisions
       AND
       STATE.NAME != 'Puerto Rico Commonwealth'; --No Puerto Rico
