
	FORM_START	AHIM
	
	CHUNK_START	AUDN
	.asciz		"envy24"
	CHUNK_END
	
	CHUNK_START	AUDM
1:	
	LONG2		AHIDB_AudioID,	0x00370001
	LONG2		AHIDB_Volume,	TRUE
	LONG2		AHIDB_Panning,	FALSE
	LONG2		AHIDB_Stereo,	FALSE
	LONG2		AHIDB_HiFi,	TRUE
	LONG2		AHIDB_MultTable,FALSE
	LONG2		AHIDB_Name,	2f-1b
	LONG		TAG_DONE
2:
	.asciz		"Envy24:HiFi 16 bit mono"
	CHUNK_END
		
	CHUNK_START	AUDM
1:	
	LONG2		AHIDB_AudioID,	0x00370002
	LONG2		AHIDB_Volume,	TRUE
	LONG2		AHIDB_Panning,	TRUE
	LONG2		AHIDB_Stereo,	TRUE
	LONG2		AHIDB_HiFi,	TRUE
	LONG2		AHIDB_MultTable,FALSE
	LONG2		AHIDB_Name,	2f-1b
	LONG		TAG_DONE
2:
	.asciz		"Envy24:HiFi 16 bit stereo++"
	CHUNK_END
		
	CHUNK_START	AUDM
1:	
	LONG2		AHIDB_AudioID,	0x00370003
	LONG2		AHIDB_Volume,	TRUE
	LONG2		AHIDB_Panning,	FALSE
	LONG2		AHIDB_Stereo,	FALSE
	LONG2		AHIDB_HiFi,	FALSE
	LONG2		AHIDB_MultTable,FALSE
	LONG2		AHIDB_Name,	2f-1b
	LONG		TAG_DONE
2:
	.asciz		"Envy24:16 bit mono"
	CHUNK_END
		
	CHUNK_START	AUDM
1:	
	LONG2		AHIDB_AudioID,	0x00370004
	LONG2		AHIDB_Volume,	TRUE
	LONG2		AHIDB_Panning,	TRUE
	LONG2		AHIDB_Stereo,	TRUE
	LONG2		AHIDB_HiFi,	FALSE
	LONG2		AHIDB_MultTable,FALSE
	LONG2		AHIDB_Name,	2f-1b
	LONG		TAG_DONE
2:
	.asciz		"Envy24:16 bit stereo++"
	CHUNK_END
		
	FORM_END

	.END
	
