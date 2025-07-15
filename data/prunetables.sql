delete from scorched3d_events where TO_DAYS(NOW()) - TO_DAYS(scorched3d_events.eventtime) > 7;

create temporary table tmp select playerid from scorched3d_stats group by playerid having sum(shots) = 0;

delete from scorched3d_players using scorched3d_players, tmp where scorched3d_players.playerid = tmp.playerid;

