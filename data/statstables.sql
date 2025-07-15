drop table scorched3d_main; 
drop table scorched3d_prefixs;
drop table scorched3d_servers;
drop table scorched3d_series; 
drop table scorched3d_players;
drop table scorched3d_stats;
drop table scorched3d_statssource;
drop table scorched3d_events;
drop table scorched3d_eventtypes;
drop table scorched3d_names;
drop table scorched3d_weapons;
drop table scorched3d_binary;
drop table scorched3d_ipaddress;

create table if not exists scorched3d_prefixs (
	prefixid INTEGER auto_increment,
	
	prefix varchar(64),
	
	PRIMARY KEY (prefixid)
);

create table if not exists scorched3d_servers (
	serverid INTEGER auto_increment,
	
	name varchar(64),
	published varchar(64),
	displaystats INTEGER NOT NULL DEFAULT 1,
	
	PRIMARY KEY (serverid),
	UNIQUE(name)
);

create table if not exists scorched3d_eventtypes (
	eventtype INTEGER,
	
	name varchar(32),
	
	PRIMARY KEY (eventtype)
);

create table if not exists scorched3d_series (
	seriesid INTEGER auto_increment,
	
	started DATETIME,
	ended DATETIME,
	name varchar(128),
	games INTEGER NOT NULL DEFAULT 0,
	rounds INTEGER NOT NULL DEFAULT 0,
	type INTEGER NOT NULL DEFAULT 0,
	
	PRIMARY KEY (seriesid)	
);

create table if not exists scorched3d_players (
	playerid INTEGER auto_increment,
	uniqueid varchar(64),
	
	name varchar(32),
	ipaddress varchar(32),
	osdesc varchar(32) NOT NULL DEFAULT '',
	avatarid INTEGER NOT NULL DEFAULT 0,
	
	PRIMARY KEY (playerid),
	UNIQUE (uniqueid),
	INDEX uniqueid_index (uniqueid),
	INDEX avatarid_index (avatarid)
);

create table if not exists scorched3d_statssource (
	serverid INTEGER,
	prefixid INTEGER,
	seriesid INTEGER,
	
	PRIMARY KEY (serverid, prefixid, seriesid),
	FOREIGN KEY (serverid) REFERENCES scorched3d_servers(serverid) on delete cascade,
	FOREIGN KEY (prefixid) REFERENCES scorched3d_prefixs(prefixid) on delete cascade,
	FOREIGN KEY (seriesid) REFERENCES scorched3d_series(seriesid) on delete cascade
);

create table if not exists scorched3d_stats (
	playerid INTEGER,
	prefixid INTEGER,
	seriesid INTEGER,
	
	connects INTEGER NOT NULL DEFAULT 0,
	lastconnected DATETIME,
	kills INTEGER NOT NULL DEFAULT 0,
	deaths INTEGER NOT NULL DEFAULT 0,
	selfkills INTEGER NOT NULL DEFAULT 0,
	teamkills INTEGER NOT NULL DEFAULT 0,
	shots INTEGER NOT NULL DEFAULT 0,
	wins INTEGER NOT NULL DEFAULT 0,
	overallwinner INTEGER NOT NULL DEFAULT 0,
	resigns INTEGER NOT NULL DEFAULT 0,
	gamesplayed INTEGER NOT NULL DEFAULT 0,
	timeplayed INTEGER NOT NULL DEFAULT 0,
	roundsplayed INTEGER NOT NULL DEFAULT 0,
	moneyearned INTEGER NOT NULL DEFAULT 0,
	scoreearned INTEGER NOT NULL DEFAULT 0,
	skill INTEGER NOT NULL DEFAULT 1000,
	rank INTEGER NOT NULL DEFAULT 0,
	
	PRIMARY KEY (playerid, prefixid, seriesid),
	FOREIGN KEY (playerid) REFERENCES scorched3d_players(playerid) on delete cascade,
	FOREIGN KEY (prefixid) REFERENCES scorched3d_prefixs(prefixid) on delete cascade,
	FOREIGN KEY (seriesid) REFERENCES scorched3d_series(seriesid) on delete cascade
);

create table if not exists scorched3d_weapons (
	weaponid INTEGER auto_increment,
	prefixid INTEGER,
	seriesid INTEGER,
	
	name varchar(64),
	icon varchar(64),
	description varchar(255) NOT NULL DEFAULT 'No Desc',
	cost INTEGER NOT NULL DEFAULT 0,
	bundlesize INTEGER NOT NULL DEFAULT 0,
	armslevel INTEGER NOT NULL DEFAULT 0,
	kills INTEGER NOT NULL DEFAULT 0,
	shots INTEGER NOT NULL DEFAULT 0,
	deathshots INTEGER NOT NULL DEFAULT 0,
	deathkills INTEGER NOT NULL DEFAULT 0,
	
	PRIMARY KEY (weaponid, prefixid, seriesid),
	FOREIGN KEY (prefixid) REFERENCES scorched3d_prefixs(prefixid) on delete cascade,
	FOREIGN KEY (seriesid) REFERENCES scorched3d_series(seriesid) on delete cascade
);

create table if not exists scorched3d_events (
	eventid INTEGER auto_increment, 
	prefixid INTEGER,
	seriesid INTEGER,
	
	eventtype INTEGER,
	playerid INTEGER,
	otherplayerid INTEGER,
	weaponid INTEGER,
	eventtime DATETIME,
	
	PRIMARY KEY (eventid),
	FOREIGN KEY (playerid) REFERENCES scorched3d_players(playerid) on delete cascade,
	FOREIGN KEY (eventtype) REFERENCES scorched3d_eventtypes(eventtype) on delete cascade,
	FOREIGN KEY (prefixid) REFERENCES scorched3d_prefixs(prefixid) on delete cascade,
	FOREIGN KEY (seriesid) REFERENCES scorched3d_series(seriesid) on delete cascade
);

create table if not exists scorched3d_names (
	playerid INTEGER NOT NULL DEFAULT 0,
	name varchar(32) BINARY NOT NULL DEFAULT "",
	
	count INTEGER NOT NULL DEFAULT 0,
	
	PRIMARY KEY (playerid, name),
	FOREIGN KEY (playerid) REFERENCES scorched3d_players(playerid) on delete cascade
);

create table if not exists scorched3d_ipaddress (
	playerid INTEGER NOT NULL DEFAULT 0,
	ipaddress varchar(32) BINARY NOT NULL DEFAULT "",
	
	count INTEGER NOT NULL DEFAULT 0,
	
	PRIMARY KEY (playerid, ipaddress),
	FOREIGN KEY (playerid) REFERENCES scorched3d_players(playerid) on delete cascade
);

create table if not exists scorched3d_binary (
	binaryid INTEGER auto_increment, 
	
	name varchar(32) BINARY NOT NULL DEFAULT "",
	crc INTEGER UNSIGNED NOT NULL DEFAULT 0,
	length INTEGER UNSIGNED NOT NULL DEFAULT 0,
	data BLOB,
	
	PRIMARY KEY(binaryid)
);

create table if not exists scorched3d_achievement_names (
	achievementid INTEGER auto_increment,

	name varchar(64) BINARY NOT NULL DEFAULT "",

	PRIMARY KEY(achievementid)
);

create table if not exists scorched3d_achievements (
	achievementid INTEGER NOT NULL DEFAULT 0,
	playerid INTEGER NOT NULL DEFAULT 0,
	achievementrank INTEGER NOT NULL DEFAULT 0,

	PRIMARY KEY(achievementid, playerid),
	FOREIGN KEY (achievementid) REFERENCES scorched3d_achievement_names(achievementrank) on delete cascade,
	FOREIGN KEY (playerid) REFERENCES scorched3d_players(playerid) on delete cascade
);
