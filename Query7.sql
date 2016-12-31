SELECT DIV.DIVISION_DESC DIVISION_NAME, --Division's name
       GROUP_CONCAT( POP.NAME, ', ' )  STATE_LIST --List of states by division
  FROM DIVISION DIV, 
       POP_ESTIMATE_NATION_STATE_PR POP
 WHERE POP.STATE > 0 --No United States or divisions in state list
       AND
       POP.NAME != 'PUERTO RICO COMMONWEALTH' --No Puerto Rico
       AND
       DIV.DIVISION_CD = POP.DIVISION --Join tables needed
 GROUP BY DIV.DIVISION_DESC; --Group into division names
