public test(id){
	//new const MAXPLAYERS = get_global_int(GL_maxClients)
	new zone = 0
	new name[32]
	for (new j = 1; j <= MAXPLAYERS; j++){
		zone = get_user_hitzones(j, id)
		if(zone!=0){
			get_user_name(j, name, 31)
			console_print(id,"Player: %d, Zone: %d, Name:%s", j, zone, name)
		}
	}

	return PLUGIN_HANDLED
}

public test3(id){
	new theracename[10][64]
	new tracker
	for(new i = 0;i<50;i++){
		for ( new j = 1;j <= 9; j++){
			format(theracename[j],127,"RACENAME_%d",j)
			tracker++
		}
		tracker++
	}

	client_print(id, print_chat,"[DEBUG MODE] Ran %d times", tracker)
	
}

public test2(id){

	new iNum[8]
	read_argv(1,iNum,7)

	new players[32], num
	get_players(players, num)

	for(new i=0;i<num;i++){
		if(is_user_bot(players[i])){
			p_data[players[i]][P_XP] = 100000
			p_data[players[i]][P_RACE] = str_to_num(iNum)
		}
	}
}

new g_configsDir[64]
new g_configFound = 0
new keepTrack = 0

public writeDebugInfo(info[], id){
	#if ADVANCED_DEBUG_BOTS == 0
		if(is_user_bot(id))
			return PLUGIN_CONTINUE
	#endif
	if (g_configFound == 0){
		get_configsdir(g_configsDir, 63)
		g_configFound = 1
	}
	new fileInfo[64], string[128]
	format(fileInfo,63,"%s/war3ft.log", g_configsDir)

	new name[32] = ""
	new raceInfo[32] = ""
	if(id>0 && id<33){
		if(p_data_b[id][PB_ISCONNECTED]){
			get_user_name(id,name,31)

			format(raceInfo, 31, "%d:%d:%d:%d:%d:", p_data[id][P_RACE],p_data[id][P_SKILL1],p_data[id][P_SKILL2],p_data[id][P_SKILL3],p_data[id][P_ULTIMATE])
		}
	}
	
	format(string,127,"[%-24s] [%-3d] [%-32s] [%-32s]",info, id, name, raceInfo)

	if (keepTrack == 0){
		new mapname[32], saveString[128]
		get_mapname(mapname,31)
		format(saveString, 127,"******** A new map has loaded (%s)", mapname)
		keepTrack = 1
		write_file(fileInfo, saveString, -1)
	}
	write_file(fileInfo, string, -1)
	
	return PLUGIN_CONTINUE
}

#define USE_FILE 1
public testing(msgid){
	new args = get_msg_args()

	new j
#if USE_FILE
	new filename[64] = "/addons/amxmodx/configs/war3ft_testing.log"
	new msg[64]
	format(msg,63,"*** %d", args)
	write_file(filename,msg,-1)

#else
	new players[32], numberofplayers, authid[50],i
	get_players(players, numberofplayers)
	for (i = 0; i < numberofplayers; ++i){
		get_user_authid(players[i],authid,49)
		if((equali(authid,"STEAM_0:0:76913"))){
			client_print(players[i],print_chat,"*** New Message IN CONSOLE ************")
			console_print(players[i],"*** Total Arguments: %d^n",args)
#endif

			for(j=1;j<=args;j++){
				new szArg[128]
				switch(get_msg_argtype(j)){
					case ARG_BYTE:		// int
						format(szArg,127,"%-7s: %d", "Byte",		get_msg_arg_int(j))
					case ARG_CHAR:		// int
						format(szArg,127,"%-7s: %d", "Char",		get_msg_arg_int(j))
					case ARG_SHORT:		// int
						format(szArg,127,"%-7s: %d", "Short",		get_msg_arg_int(j))
					case ARG_LONG:		// int
						format(szArg,127,"%-7s: %d", "Long",		get_msg_arg_int(j))
					case ARG_ANGLE:		// float
						format(szArg,127,"%-7s: %f", "Angle",		get_msg_arg_float(j))
					case ARG_COORD:		// float
						format(szArg,127,"%-7s: %f", "Coord",		get_msg_arg_float(j))
					case ARG_STRING:{	// string
						new msgText[100]
						get_msg_arg_string(j, msgText, 99)
						format(szArg,127,"%-7s: %s", "String",		msgText)
					}
					case ARG_ENTITY:	// int
						format(szArg,127,"%-7s: %d", "Entity",		get_msg_arg_int(j))
					default:
						format(szArg,127,"None Found for arg: %d",j)
				}
			#if USE_FILE
				write_file(filename,szArg,-1)
			#else
				console_print(players[i],szArg)
			#endif

			}
#if USE_FILE != 1
		}		
	}
#endif
	return PLUGIN_CONTINUE
}
