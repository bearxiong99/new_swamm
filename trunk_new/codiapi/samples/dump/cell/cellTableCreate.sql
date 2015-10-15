--
-- Create CellPlanning Table
--

drop table if exists atc_codi_tbl;
drop table if exists atc_cell_tbl;

create table if not exists atc_codi_tbl
( 
 _id TEXT not null primary key,     -- eui64id
 latitude       REAL,               -- ddmm.mmmmmm 2307.1256
 nsIndicator    TEXT,               -- N=north / S=south
 longitude      REAL,               -- dddmm.mmmm 12016.4438
 ewIndicator    TEXT                -- E=east / W=west
);

create table if not exists atc_cell_tbl
( 
 _id            TEXT not null
        primary key,                -- eui64id
 _codi          TEXT not null
        references atc_codi_tbl(_id) on delete cascade,
 latitude       REAL,               -- ddmm.mmmmmm 2307.1256
 nsIndicator    TEXT,               -- N=north / S=south
 longitude      REAL,               -- dddmm.mmmm 12016.4438
 ewIndicator    TEXT,               -- E=east / W=west
 lastDate       TEXT,               -- ddmmyy 260406
 lastUtcTime    REAL,               -- hhmmss.sss 064951.000
 lqi            INTEGER,
 rssi           INTEGER
);

create trigger if not exists atc_codi_d_tr
  before delete on atc_codi_tbl
  for each row 
    begin
     delete from atc_cell_tbl where _codi = OLD._id; 
    end;


/**
insert into atc_codi_tbl (_id)
values
(
 "000D6F0000E49ACE"
);

insert into atc_cell_tbl (_id, _codi)
values
(
 "000D6F0000E4ACF", "000D6F0000E49ACE"
);
*/
/**
insert into atc_cell_tbl (_id, _codi)
values
(
 "000D6F0000E4ACC", "INVALID"
);
*/
