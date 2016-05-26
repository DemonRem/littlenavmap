/*****************************************************************************
* Copyright 2015-2016 Alexander Barthel albar965@mailbox.org
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "infoquery.h"

#include <sql/sqldatabase.h>

using atools::sql::SqlQuery;
using atools::sql::SqlDatabase;
using atools::sql::SqlRecord;
using atools::sql::SqlRecordVector;

InfoQuery::InfoQuery(QObject *parent, SqlDatabase *sqlDb)
  : QObject(parent), db(sqlDb)
{

}

InfoQuery::~InfoQuery()
{
  deInitQueries();
}

const SqlRecord *InfoQuery::getAirportInformation(int airportId)
{
  return cachedRecord(airportCache, airportQuery, airportId);
}

const SqlRecordVector *InfoQuery::getComInformation(int airportId)
{
  return cachedRecordVector(comCache, comQuery, airportId);
}

const SqlRecordVector *InfoQuery::getRunwayInformation(int airportId)
{
  return cachedRecordVector(runwayCache, runwayQuery, airportId);
}

const atools::sql::SqlRecord* InfoQuery::getRunwayEndInformation(int runwayEndId)
{
  return cachedRecord(runwayEndCache, runwayEndQuery, runwayEndId);
}

const SqlRecord *InfoQuery::getVorInformation(int vorId)
{
  return cachedRecord(vorCache, vorQuery, vorId);
}

const SqlRecord *InfoQuery::getNdbInformation(int ndbId)
{
  return cachedRecord(ndbCache, ndbQuery, ndbId);
}

const SqlRecord *InfoQuery::getWaypointInformation(int waypointId)
{
  return cachedRecord(waypointCache, waypointQuery, waypointId);
}

const SqlRecord *InfoQuery::getAirwayInformation(int airwayId)
{
  return cachedRecord(airwayCache, airwayQuery, airwayId);
}

const SqlRecord *InfoQuery::cachedRecord(QCache<int, SqlRecord>& cache, SqlQuery *query, int id)
{
  SqlRecord *rec = cache.object(id);
  if(rec != nullptr)
  {
    if(rec->isEmpty())
      return nullptr;
    else
      return rec;
  }
  else
  {
    query->bindValue(":id", id);
    query->exec();
    if(query->next())
    {
      rec = new SqlRecord(query->record());
      cache.insert(id, rec);
      return rec;
    }
    else
      // Add empty record to indicate nothing found for this id
      cache.insert(id, new SqlRecord());
  }
  return nullptr;
}

const SqlRecordVector *InfoQuery::cachedRecordVector(QCache<int, SqlRecordVector>& cache,
                                                     SqlQuery *query, int id)
{
  SqlRecordVector *rec = cache.object(id);
  if(rec != nullptr)
  {
    if(rec->isEmpty())
      return nullptr;
    else
      return rec;
  }
  else
  {
    query->bindValue(":id", id);
    query->exec();

    rec = new SqlRecordVector;

    while(query->next())
      rec->append(query->record());

    cache.insert(id, rec);

    if(rec->isEmpty())
      return nullptr;
    else
      return rec;
  }
  return nullptr;
}

void InfoQuery::initQueries()
{
  deInitQueries();

  // TODO limit number of columns
  airportQuery = new SqlQuery(db);
  airportQuery->prepare("select * from airport "
                        "join bgl_file on airport.file_id = bgl_file.bgl_file_id "
                        "join scenery_area on bgl_file.scenery_area_id = scenery_area.scenery_area_id "
                        "where airport_id = :id");

  comQuery = new SqlQuery(db);
  comQuery->prepare("select * from com where airport_id = :id");

  vorQuery = new SqlQuery(db);
  vorQuery->prepare("select * from vor "
                    "join bgl_file on vor.file_id = bgl_file.bgl_file_id "
                    "join scenery_area on bgl_file.scenery_area_id = scenery_area.scenery_area_id "
                    "where vor_id = :id");

  ndbQuery = new SqlQuery(db);
  ndbQuery->prepare("select * from ndb "
                    "join bgl_file on ndb.file_id = bgl_file.bgl_file_id "
                    "join scenery_area on bgl_file.scenery_area_id = scenery_area.scenery_area_id "
                    "where ndb_id = :id");

  waypointQuery = new SqlQuery(db);
  waypointQuery->prepare("select * from waypoint "
                         "join bgl_file on waypoint.file_id = bgl_file.bgl_file_id "
                         "join scenery_area on bgl_file.scenery_area_id = scenery_area.scenery_area_id "
                         "where waypoint_id = :id");

  airwayQuery = new SqlQuery(db);
  airwayQuery->prepare("select * from airway where airway_id = :id");

  runwayQuery = new SqlQuery(db);
  runwayQuery->prepare("select * from runway where airport_id = :id");

  runwayEndQuery = new SqlQuery(db);
  runwayEndQuery->prepare("select * from runway_end where runway_end_id = :id");
}

void InfoQuery::deInitQueries()
{
  delete airportQuery;
  airportQuery = nullptr;

  delete comQuery;
  comQuery = nullptr;

  delete vorQuery;
  vorQuery = nullptr;

  delete ndbQuery;
  ndbQuery = nullptr;

  delete waypointQuery;
  waypointQuery = nullptr;

  delete airwayQuery;
  airwayQuery = nullptr;

  delete runwayQuery;
  runwayQuery = nullptr;

  delete runwayEndQuery;
  runwayEndQuery = nullptr;
}