/*��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.
*	Race: Blood Mage Functions
��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.*/

#define IMMOLATE_DAMAGE			35		// Initial damage done to target players
#define IMMOLATE_DOT_DAMAGE		5		// Damage done on each tick of the ultimate
#define IMMOLATE_DOT			4		// Number of times ultimate ticks

#define BM_PHEONIX_RANGE		750		// Range to award money


public BM_ULT_Immolate( iCaster, iTarget )
{
	emit_sound( iCaster, CHAN_STATIC, SOUND_IMMOLATE, 0.5, ATTN_NORM, 0, PITCH_NORM );

	// Emit sound and create the fire exploding effect on the player

	new vTargetOrigin[3];
	get_user_origin( iTarget, vTargetOrigin );

	Create_TE_EXPLOSION( vTargetOrigin, vTargetOrigin, g_sImmolate, 20, 24, 4 );
	Create_TE_EXPLOSION( vTargetOrigin, vTargetOrigin, g_sBurning, 30, 24, 4 );

	Create_ScreenShake( iTarget, (10<<12), (2<<12), (5<<12) );

	// Do initial immolate damage and make their screen shake a little

	WAR3_damage( iTarget, iCaster, IMMOLATE_DAMAGE, CSW_IMMOLATE, -1 )

	new parm_DoT[3];
	parm_DoT[0] = iCaster;
	parm_DoT[1] = iTarget;
	parm_DoT[2] = 1;

	// Start the ultimate DoT

	new TaskId = TASK_BURNING + iTarget;
	set_task( 1.0, "BM_ULT_Immolate_DoT", TaskId, parm_DoT, 3 );
	
	return PLUGIN_HANDLED;
}

public BM_ULT_Immolate_DoT( parm_DoT[3] )
{
	new iCaster = parm_DoT[0];
    new iTarget = parm_DoT[1];
	new iCounter = parm_DoT[2];
	
	// Stop DoT if the max number of ticks is reached

	if ( iCounter > IMMOLATE_DOT ) 
	{
		BM_ULT_Immolate_Remove( iTarget );
		return PLUGIN_HANDLED;
	}

	// Emit sound and show the burning effect on the player

    new vTargetOrigin[3];
    get_user_origin( iTarget, vTargetOrigin );

	emit_sound( iTarget, CHAN_STATIC, SOUND_IMMOLATE_BURNING, 0.5, ATTN_NORM, 0, PITCH_NORM );

	Create_TE_SPRITE( vTargetOrigin, g_sFire, 3, 200 );

	// Do the DoT damage

	WAR3_damage( iTarget, iCaster, IMMOLATE_DOT, CSW_IMMOLATE, -1 )

	// If the target is still alive after this, make their screen glow orange and start the task again

    if ( is_user_alive( iTarget ) )
    {
		p_data[iTarget][PB_ISBURNING] = true;

        Create_ScreenFade( iTarget, (1<<10), (1<<10), (1<<12), 255, 108, 0, 160 );

		parm_DoT[2]++;

		new TaskId = TASK_BURNING + iTarget;
		set_task( 1.0, "BM_ULT_Immolate_DoT", TaskId, parm_DoT, 3 );

	}

	return PLUGIN_HANDLED;
}


BM_ULT_Immolate_Remove( iTarget ) 
{
	// Clear global variables and stop the DoT task

	p_data[iTarget][PB_ISBURNING] = false;

	new TaskId = TASK_BURNING + iTarget;
    remove_task( TaskId, 0 );

    return PLUGIN_HANDLED;
}

// Check to see if a player will become a Phoenix
BM_PheonixCheck( id )
{
	if ( Verify_Skill(id, RACE_BLOOD, SKILL1) )
	{
		
		// Should the user be a Phoenix
		if ( random_float( 0.0, 1.0 ) <= p_pheonix[p_data[id][P_SKILL1]-1] ) 
		{
			p_data_b[id][PB_PHOENIX] = true;
		}
	}
}

// This function is called when a user dies
BM_PhoenixSpawn( id )
{
	
	// If this user is about to respawn, then we don't even want to check
	if ( p_data[id][P_RESPAWNBY] )
	{
		return;
	}

	new iTeam = get_user_team( id );
	new iPhoenixID = BM_PhoenixExists( iTeam );
	
	// Then we have a Phoenix!!
	if ( iPhoenixID != -1 )
	{

		// Lets make sure they're not going to revive themself
		if ( iPhoenixID != id )
		{

			// The caster is no longer a Phoenix
			p_data_b[iPhoenixID][PB_PHOENIX] = false;
			
			// The user is about to respawn
			p_data[id][P_RESPAWNBY] = RESPAWN_PHOENIX;
			
			// Respawn the user
			set_task( SPAWN_DELAY, "_SHARED_Spawn", TASK_SPAWN + id );

			new szCastername[32], szSpawnerName[32];

			get_user_name( iPhoenixID, szCastername,31 )
			get_user_name( id, szSpawnerName, 31 )

			client_print( id, print_chat, "%s %L", g_MODclient, id, "HAS_REVIVED_YOU", szCastername );
			client_print( iPhoenixID, print_chat, "%s %L", g_MODclient, iPhoenixID, "YOU_HAVE_REVIVED", szSpawnerName );
		}
	}

	return;
}

// Check to see if a Phoenix exists on a given team
BM_PhoenixExists( iTeam )
{
	new players[32], numberofplayers, id, i;
	get_players(players, numberofplayers, "a");

	// Loop through all the players to find someone with phoenix
	for ( i = 0; i < numberofplayers; i++ )
	{
		id = players[i];

		if ( get_user_team( id ) == iTeam )
		{
			if ( p_data_b[id][PB_PHOENIX] )
			{
				return id;
			}
		}
	}

	return -1;
}

BM_PheonixDOD( id )
{

	// Award the player money for having Phoenix
	SHARED_SetUserMoney( id, SHARED_GetUserMoney( id ) + p_pheonix_dod[p_data[id][P_SKILL1]-1] );

	new szUserName[32], iTeam, iTargetID;
	new i, vTargetOrigin[3], vOrigin[3];

	// Get the "caster's" name
	get_user_name( id, szUserName, 31 );

	// Determine the caster's team
	iTeam = get_user_team( id );

	// Get the caster's origin
	get_user_origin( id, vOrigin );
	
	// Get a list of the current alive players
	new players[32], numberofplayers;
	get_players( players, numberofplayers, "a" );

	new iMoney = p_pheonix_dod[p_data[id][P_SKILL1]-1] / 2;

	for ( i = 0; i < numberofplayers; i++ )
	{
		iTargetID = players[i];
		
		// Make sure player is on the same team
		if ( iTargetID != id && get_user_team( iTargetID ) == iTeam )
		{
			
			// Get the target's origin
			get_user_origin( iTargetID, vTargetOrigin );
			
			// See if they're close enough
			if ( get_distance( vOrigin, vTargetOrigin ) <= BM_PHEONIX_RANGE )
			{
				// Give them some money
				SHARED_SetUserMoney( iTargetID, SHARED_GetUserMoney( iTargetID ) + iMoney );

				client_print( iTargetID, print_chat, "%s %L", g_MODclient, iTargetID, "DOD_PHOENIX", iMoney, szUserName )
			}
		}
	}
}

BM_SkillsOffensive( iAttacker, iVictim, iWeapon, iDamage, iHitPlace )
{
	// Banish
	if ( Verify_Skill( iAttacker, RACE_BLOOD, SKILL2 ) )
	{

		if ( random_float( 0.0, 1.0 ) <= p_banish[p_data[iAttacker][P_SKILL2]-1] )
		{
			
			// Do 9 damage then....
			new iAdditionalDamage = ( ( p_data[iAttacker][P_SKILL2] > 2 ) ?  9 : 7 );

			// Slap the user
			user_slap( iVictim, 0 );
			user_slap( iVictim, 0 );
			user_slap( iVictim, 0 );

			// Deal some damage
			WAR3_damage( iVictim, iAttacker, iAdditionalDamage, iWeapon, iHitPlace );
			
			// Play the Banish sound
			emit_sound( iVictim, CHAN_STATIC, SOUND_BANISH, 1.0, ATTN_NORM, 0, PITCH_NORM );

			// Make the user glow!
			SHARED_Glow( iVictim, 0, 0, 0, 100 );
			
			// Create a screen fade
			Create_ScreenFade( iVictim, (1<<10), (1<<10), (1<<12), 255, 255, 255, g_GlowLevel[iVictim][3] );
		}
	}

	// Siphon Mana
	if ( Verify_Skill( iAttacker, RACE_BLOOD, SKILL3 ) )
	{
		new iMoney = floatround( p_mana[p_data[iAttacker][P_SKILL3]-1] * SHARED_GetUserMoney(iVictim) );
		
		// Remove the money from the victim
		SHARED_SetUserMoney( iVictim, SHARED_GetUserMoney( iVictim ) - iMoney, 1 );
		
		// Give the money to the attacker
		SHARED_SetUserMoney( iAttacker, SHARED_GetUserMoney( iAttacker ) + iMoney, 1 );

		// Make the user glow!
		SHARED_Glow( iVictim, 0, iDamage, 0, 0 );
		
		// Create a screen fade (purplish)
		Create_ScreenFade( iAttacker, (1<<10), (1<<10), (1<<12), 144, 58, 255, g_GlowLevel[iAttacker][1] );
	}
}

BM_SkillsDefensive( iVictim, iDamage )
{

	// Resistant Skin
	if ( Verify_Race( iVictim, RACE_BLOOD ) )
	{
		new iBonusHealth = floatround( float( iDamage ) * p_resistant[p_data[iVictim][P_LEVEL]] );

		set_user_health( iVictim, get_user_health( iVictim ) + iBonusHealth );
	}
}