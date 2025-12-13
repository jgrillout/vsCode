i have a sqllite3 database with this schema

CREATE TABLE 'CustomersScreen1' ('CUST' PRIMARY KEY, 'STAT', 'NAME', 'ADD1', 'ADD2', 'ADD3', 'CITY', 'ST', 'ZIP', 'CTRY', 'SZON', 'ATTN', 'PHONE', 'FAX', 'CONT', 'CLS', 'SREP1', 'SREP2', 'TER', 'TCOD', 'GCOD', 'SFTYP', 'IFTYP', 'CUSLV', 'ACTP', 'DIST', 'PSHP', 'TXGRP', 'TXBL', 'WEB')

I wan to change it to this
CREATE TABLE 'CustomersScreen1' ('CUST' PRIMARY KEY, , 'NAME', 'ADD1', 'ADD2', 'ADD3', 'CITY', 'ST', 'ZIP', 'CTRY', 'SZON', 'ATTN', 'PHONE', 'FAX', 'CONT', 'CLS', 'SREP1', 'SREP2', 'TER', 'STAT','TCOD', 'GCOD', 'SFTYP', 'IFTYP', 'CUSLV', 'ACTP', 'DIST', 'PSHP', 'TXGRP', 'TXBL', 'WEB')

I have data in this table that i need to retain. Can you explain to me how to do that and provide the sql statements where possible?