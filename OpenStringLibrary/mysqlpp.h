/***************************************************************
 * (C) Copyright 1998-2010 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#ifndef __MYSQLPP__H
#define __MYSQLPP__H

#include <mysql/mysql.h>
#define MYSQLSOCKET NULL


class cMysqlInterface {

	protected:
		MYSQL conn;
		MYSQL_RES *result;
		MYSQL_ROW row;
		MYSQL_FIELD *field;
		
		
		cString host,username,password, database;
		unsigned int port;
		unsigned long flags;
		bool debug, connected;


		MYSQL_RES * GenericQuery(cString query) {
			mysql_query(&conn,query);
			result=mysql_store_result(&conn);
			return result;
		}


	public:
		cMysqlInterface() {
			SetDefaults();
		}
		~cMysqlInterface() { if(IsConnected()) Disconnect(); }
		

		void SetDefaults() {
			
			#ifdef DB_HOST
				host=DB_HOST;
			#else
				host="localhost";
			#endif
			
			#ifdef DB_USERNAME
				username=DB_USERNAME;
			#else
				!! error !! need to define DB_USERNAME
			#endif
			
			#ifdef DB_PASSWORD
				password=DB_PASSWORD;
			#else
				!! error !! need to define DB_PASSWORD
			#endif
			
			#ifdef DB_PORT
				//3306
				port=DB_PORT;
			#else
				port=0;
			#endif
			
			#ifdef DB_NAME
				database=DB_NAME;
			#else
				!! error !! need to define DB_NAME
			#endif	
			
			#ifdef DB_FLAGS
				flags=DB_FLAGS;
			#else
				flags=0;
			#endif

			debug=false;
			
			connected=false;
		}

				
		void SetUserName(cString _username) {
			username=_username;
		}
		
		void SetPassword(cString _password) {
			password=_password;
		}
		
		void SetUserNameandPassword(cString _username, cString _password)	{
			SetUserName(_username);
			SetPassword(_password);
		}
		void SetDatabase(cString _database) { database=_database; }
		void SetPort(int _port) { port=_port; }
		void SetHost(cString _host)  { host=_host; }
		//void SetSocket(cString _socket) { socket=_socket; }
		
		void SetDebug(bool _debug) { debug=_debug; };
		
		bool Connect() {
			if(connected) return true;
			if(mysql_init(&conn)==NULL) return false;
			
			result=NULL;
			field=NULL;
			
			//cerr<<"host="<<host<<endl;
			//cerr<<"username="<<username<<endl;
			//cerr<<"password="<<password<<endl;
			//cerr<<"database="<<database<<endl;
			//cerr<<"port="<<port<<endl;
			//cerr<<"flags="<<flags<<endl;
			
			MYSQL *connPtr;
			connPtr=mysql_real_connect(&conn,host,username,password,database,port,MYSQLSOCKET,flags);
			if(connPtr==NULL) return false;
			connected=true;
			return connected;
		}
		void Disconnect() {
			if(debug) cerr<<"disconnecting mysql"<<endl;
			mysql_close(&conn);
			connected=false;
		}
	
		bool IsConnected() { return connected; }
	

		//also used for delete
		bool GenericQuerySave(cString query) {
			if(!connected) return false;
			MYSQL_RES *result=GenericQuery(query);
			if(result!=NULL) mysql_free_result(result);
			return true;
		}
		
	//retrieve multiple rows
		bool ExistsInQuery(cString query) {
			if(!connected) return false;
			MYSQL_RES *result=GenericQuery(query);
			MYSQL_ROW row;
			if(result==NULL) return false;

			int affected=mysql_field_count(&conn);
	
			return mysql_affected_rows(&conn)>0;
		}
	
			//retrieve multiple rows
		bool GenericQueryGetRows(cString query, cVectorList & matrix) {
			if(!connected) return false;
			MYSQL_RES *result=GenericQuery(query);
			MYSQL_ROW row;
			if(result==NULL) return false;

			int field_count=mysql_field_count(&conn);
			int affected_rows=mysql_affected_rows(&conn);
		
			//cerr<<"field count="<<field_count<<" and affected rows="<<affected_rows<<endl;
			cStringList temp;
			
			for(int i=0; i<affected_rows; i++) {
				row=mysql_fetch_row(result);
				temp.ClearAll();
				for(int j=0; j<field_count; j++) temp+=cString(row[j]);
				matrix+=temp;
			}
			mysql_free_result(result);
			return true;
		}
		
		//retrieve just 1 row
		bool GenericQueryGetRow(cString query,cStringList & singlerow) {
			if(!connected) return false;
			cVectorList matrix;
			if(!query.Contains("LIMIT")) query+=" LIMIT 1";
			bool ret=GenericQueryGetRows(query,matrix);
			if(matrix.Length()>0) singlerow=matrix[0];
			return ret;
		}
		
		//retrieve just 1 specific element
		bool GenericQueryGetSingleElement(cString query,cString & element) {
			if(!connected) return false;
			cStringList singlerow;
			if(!query.Contains("LIMIT")) query+=" LIMIT 1";
			bool ret=GenericQueryGetRow(query,singlerow);
			if(singlerow.Length()>0) element=singlerow[0];
			return ret;
		}
		
		cString GetFromTable(cString table, cString column, cString tableKey, cString ID) {
			cString element;
			GenericQueryGetSingleElement("SELECT "+column+" FROM "+table+" WHERE "+tableKey+" = \'"+ID+"\'",element);
			
			return element;			
		}
		bool ExistsInTable2(cString table, cString column, cString tableKey, cString ID) {
			cString element;
			GenericQueryGetSingleElement("SELECT "+column+" FROM "+table+" WHERE "+tableKey+" = \'"+ID+"\'",element);
			
			return element!="";			
		}
		
		bool InsertEverything(cString table, cString tableKey, cDualList &data) {	
			cString query="INSERT INTO "+table+" (";
			
			cString gfq = "SELECT * FROM "+table+" LIMIT 1";
			MYSQL_RES *gfr=GenericQuery(gfq);
			
			int num_fields = mysql_num_fields(gfr);

			MYSQL_FIELD* fields=mysql_fetch_fields(gfr);

			for (int i=0; i<num_fields; i++) {
				if(cString(fields[i].name)!=tableKey) {
					query+=cString(fields[i].name);
					if(i==num_fields-1) query+=")";
					else query+=",";
				}
			}
			
			query+=" VALUES(";
			
			for (int i=0; i<num_fields; i++) {
				if(cString(fields[i].name)!=tableKey) {
					query+="'"+data[fields[i].name]+"'";
					if(i==num_fields-1) query+=");";
					else query+=",";
				}
			}
			
			MYSQL_RES *results=GenericQuery(query);
			return true;
		}
	
		
		bool SelectEverything(cString table, cString tableKey,cDualList &data) {
			if(data[tableKey]=="-1") data[tableKey]=1;

			cString gfq="SELECT * FROM "+table;
			
			MYSQL_RES *gfr=GenericQuery(gfq);

			int num_fields = mysql_num_fields(gfr);
	
			if(data[tableKey]=="") {
				cerr<<"error: missing tableKey in data array"<<endl;
				return false;
			}
			
			cString query="SELECT * FROM "+table+" WHERE "+tableKey+" = \'"+data[tableKey]+"\'";
			
			MYSQL_RES *results=GenericQuery(query);
			
			MYSQL_FIELD* fields=mysql_fetch_fields(results);
			row=mysql_fetch_row(result);
		
			for (int i=0; i<num_fields; i++) {
				data[fields[i].name]=row[i];
			}
			mysql_free_result(results);
			mysql_free_result(gfr);
			//mysql_free


			return true;
		}	
		bool SaveEverything(cString table, cString tableKey,cDualList &data) {
			//	global $insertAllowed;
			//	if(!isset($data[$tableKey]) || $data[$tableKey]=="" && $insertAllowed) {
			//if(DEBUG_MODE)  echo "(save everything) missing tablekey = ".$tableKey."<BR>";
			if(data[tableKey]=="") return InsertEverything(table,tableKey,data);
			cString gfq = "SELECT * FROM "+table+" LIMIT 1";
			
			
			MYSQL_RES *gfr =NULL;
			
			
			gfr=GenericQuery(gfq);
			
			int num_fields = mysql_num_fields(gfr);
			
		
			MYSQL_FIELD* fields=mysql_fetch_fields(gfr);
			
			//build query	
			cString query="UPDATE "+table+" SET ";
			for (int i=0; i<num_fields; i++) {
				if(cString(fields[i].name)!=tableKey && data[fields[i].name]!="") {
					query+=" "+cString(fields[i].name)+" = \'"+data[fields[i].name]+"\'";
					if(i!=num_fields-1) query+=",";
				}
			}
			
			if(query[-1]==',') query=query.ChopRt(1);
			
			query+=" WHERE "+tableKey+"=\'"+data[tableKey]+"\'";	
	
			result =GenericQuery(query);
			
			mysql_free_result(result);
			mysql_free_result(gfr);
		
			return true;
		}

		
};

#endif
