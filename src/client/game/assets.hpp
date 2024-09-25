#pragma once

namespace game
{
	enum XAssetType
	{
		ASSET_TYPE_PHYSPRESET = 0x0,
		ASSET_TYPE_PHYSCONSTRAINTS = 0x1,
		ASSET_TYPE_DESTRUCTIBLEDEF = 0x2,
		ASSET_TYPE_XANIMPARTS = 0x3,
		ASSET_TYPE_XMODEL = 0x4,
		ASSET_TYPE_XMODELMESH = 0x5,
		ASSET_TYPE_MATERIAL = 0x6,
		ASSET_TYPE_COMPUTE_SHADER_SET = 0x7,
		ASSET_TYPE_TECHNIQUE_SET = 0x8,
		ASSET_TYPE_IMAGE = 0x9,
		ASSET_TYPE_SOUND = 0xA,
		ASSET_TYPE_SOUND_PATCH = 0xB,
		ASSET_TYPE_CLIPMAP = 0xC,
		ASSET_TYPE_COMWORLD = 0xD,
		ASSET_TYPE_GAMEWORLD = 0xE,
		ASSET_TYPE_MAP_ENTS = 0xF,
		ASSET_TYPE_GFXWORLD = 0x10,
		ASSET_TYPE_LIGHT_DEF = 0x11,
		ASSET_TYPE_LENSFLARE_DEF = 0x12,
		ASSET_TYPE_UI_MAP = 0x13,
		ASSET_TYPE_FONT = 0x14,
		ASSET_TYPE_FONTICON = 0x15,
		ASSET_TYPE_LOCALIZE_ENTRY = 0x16,
		ASSET_TYPE_WEAPON = 0x17,
		ASSET_TYPE_WEAPONDEF = 0x18,
		ASSET_TYPE_WEAPON_VARIANT = 0x19,
		ASSET_TYPE_WEAPON_FULL = 0x1A,
		ASSET_TYPE_CGMEDIA = 0x1B,
		ASSET_TYPE_PLAYERSOUNDS = 0x1C,
		ASSET_TYPE_PLAYERFX = 0x1D,
		ASSET_TYPE_SHAREDWEAPONSOUNDS = 0x1E,
		ASSET_TYPE_ATTACHMENT = 0x1F,
		ASSET_TYPE_ATTACHMENT_UNIQUE = 0x20,
		ASSET_TYPE_WEAPON_CAMO = 0x21,
		ASSET_TYPE_CUSTOMIZATION_TABLE = 0x22,
		ASSET_TYPE_CUSTOMIZATION_TABLE_FE_IMAGES = 0x23,
		ASSET_TYPE_CUSTOMIZATION_TABLE_COLOR = 0x24,
		ASSET_TYPE_SNDDRIVER_GLOBALS = 0x25,
		ASSET_TYPE_FX = 0x26,
		ASSET_TYPE_TAGFX = 0x27,
		ASSET_TYPE_NEW_LENSFLARE_DEF = 0x28,
		ASSET_TYPE_IMPACT_FX = 0x29,
		ASSET_TYPE_IMPACT_SOUND = 0x2A,
		ASSET_TYPE_PLAYER_CHARACTER = 0x2B,
		ASSET_TYPE_AITYPE = 0x2C,
		ASSET_TYPE_CHARACTER = 0x2D,
		ASSET_TYPE_XMODELALIAS = 0x2E,
		ASSET_TYPE_RAWFILE = 0x2F,
		ASSET_TYPE_STRINGTABLE = 0x30,
		ASSET_TYPE_STRUCTURED_TABLE = 0x31,
		ASSET_TYPE_LEADERBOARD = 0x32,
		ASSET_TYPE_DDL = 0x33,
		ASSET_TYPE_GLASSES = 0x34,
		ASSET_TYPE_TEXTURELIST = 0x35,
		ASSET_TYPE_SCRIPTPARSETREE = 0x36,
		ASSET_TYPE_KEYVALUEPAIRS = 0x37,
		ASSET_TYPE_VEHICLEDEF = 0x38,
		ASSET_TYPE_ADDON_MAP_ENTS = 0x39,
		ASSET_TYPE_TRACER = 0x3A,
		ASSET_TYPE_SLUG = 0x3B,
		ASSET_TYPE_SURFACEFX_TABLE = 0x3C,
		ASSET_TYPE_SURFACESOUNDDEF = 0x3D,
		ASSET_TYPE_FOOTSTEP_TABLE = 0x3E,
		ASSET_TYPE_ENTITYFXIMPACTS = 0x3F,
		ASSET_TYPE_ENTITYSOUNDIMPACTS = 0x40,
		ASSET_TYPE_ZBARRIER = 0x41,
		ASSET_TYPE_VEHICLEFXDEF = 0x42,
		ASSET_TYPE_VEHICLESOUNDDEF = 0x43,
		ASSET_TYPE_TYPEINFO = 0x44,
		ASSET_TYPE_SCRIPTBUNDLE = 0x45,
		ASSET_TYPE_SCRIPTBUNDLELIST = 0x46,
		ASSET_TYPE_RUMBLE = 0x47,
		ASSET_TYPE_BULLETPENETRATION = 0x48,
		ASSET_TYPE_LOCDMGTABLE = 0x49,
		ASSET_TYPE_AIMTABLE = 0x4A,
		ASSET_TYPE_ANIMSELECTORTABLESET = 0x4B,
		ASSET_TYPE_ANIMMAPPINGTABLE = 0x4C,
		ASSET_TYPE_ANIMSTATEMACHINE = 0x4D,
		ASSET_TYPE_BEHAVIORTREE = 0x4E,
		ASSET_TYPE_BEHAVIORSTATEMACHINE = 0x4F,
		ASSET_TYPE_TTF = 0x50,
		ASSET_TYPE_SANIM = 0x51,
		ASSET_TYPE_LIGHT_DESCRIPTION = 0x52,
		ASSET_TYPE_SHELLSHOCK = 0x53,
		ASSET_TYPE_XCAM = 0x54,
		ASSET_TYPE_BG_CACHE = 0x55,
		ASSET_TYPE_TEXTURE_COMBO = 0x56,
		ASSET_TYPE_FLAMETABLE = 0x57,
		ASSET_TYPE_BITFIELD = 0x58,
		ASSET_TYPE_ATTACHMENT_COSMETIC_VARIANT = 0x59,
		ASSET_TYPE_MAPTABLE = 0x5A,
		ASSET_TYPE_MAPTABLE_LOADING_IMAGES = 0x5B,
		ASSET_TYPE_MEDAL = 0x5C,
		ASSET_TYPE_MEDALTABLE = 0x5D,
		ASSET_TYPE_OBJECTIVE = 0x5E,
		ASSET_TYPE_OBJECTIVE_LIST = 0x5F,
		ASSET_TYPE_UMBRA_TOME = 0x60,
		ASSET_TYPE_NAVMESH = 0x61,
		ASSET_TYPE_NAVVOLUME = 0x62,
		ASSET_TYPE_BINARYHTML = 0x63,
		ASSET_TYPE_LASER = 0x64,
		ASSET_TYPE_BEAM = 0x65,
		ASSET_TYPE_STREAMER_HINT = 0x66,
		ASSET_TYPE_COUNT = 0x67,
		ASSET_TYPE_STRING = 0x68,
		ASSET_TYPE_ASSETLIST = 0x69,
		ASSET_TYPE_REPORT = 0x6A,
		ASSET_TYPE_DEPEND = 0x68,
		ASSET_TYPE_FULL_COUNT = 0x6C,
	};

	struct RawFile
	{
		const char* name;
		int len;
		const char* buffer;
	};

	enum ddlPermissionTypes_e : size_t
	{
		DDL_PERMISSIONS_UNSPECIFIED,
		DDL_PERMISSIONS_CLIENTONLY,
		DDL_PERMISSIONS_SERVERONLY,
		DDL_PERMISSIONS_BOTH,
		DDL_PERMISSIONS_COUNT,
	};

	enum DDLType
	{
		DDL_INVALID_TYPE = 0xFFFFFFFF,
		DDL_BYTE_TYPE = 0x0,
		DDL_SHORT_TYPE = 0x1,
		DDL_UINT_TYPE = 0x2,
		DDL_INT_TYPE = 0x3,
		DDL_UINT64_TYPE = 0x4,
		DDL_FLOAT_TYPE = 0x5,
		DDL_FIXEDPOINT_TYPE = 0x6,
		DDL_STRING_TYPE = 0x7,
		DDL_STRUCT_TYPE = 0x8,
		DDL_ENUM_TYPE = 0x9,
		DDL_PAD_TYPE = 0xA,
		DDL_TYPE_COUNT = 0xB,
	};

	struct DDLMember
	{
		const char* name;
		int index;
		void* parent;
		int bitSize;
		int limitSize;
		int offset;
		int type;
		int externalIndex;
		unsigned int rangeLimit;
		unsigned int serverDelta;
		unsigned int clientDelta;
		int arraySize;
		int enumIndex;
		int permission;
	};

	struct DDLHash
	{
		int hash;
		int index;
	};

	struct DDLHashTable
	{
		DDLHash* list;
		int count;
		int max;
	};

	struct DDLStruct
	{
		const char* name;
		int bitSize;
		int memberCount;
		DDLMember* members;
		DDLHashTable hashTableUpper;
		DDLHashTable hashTableLower;
	};

	struct DDLEnum
	{
		const char* name;
		int memberCount;
		const char** members;
		DDLHashTable hashTable;
	};

	struct DDLDef
	{
		char* name;
		unsigned short version;
		unsigned int checksum;
		char flags;
		int bitSize;
		int byteSize;
		DDLStruct* structList;
		int structCount;
		DDLEnum* enumList;
		int enumCount;
		DDLDef* next;
		int headerBitSize;
		int headerByteSize;
		int reserveSize;
		int userFlagsSize;
		bool paddingUsed;
	};

	struct DDLRoot
	{
		const char* name;
		DDLDef* ddlDef;
	};

	union XAssetHeader
	{
		/*PhysPreset* physPreset;
		PhysConstraints* physConstraints;
		DestructibleDef* destructibleDef;
		XAnimParts* parts;
		XModel* model;
		XModelMesh* modelMesh;
		Material* material;
		MaterialComputeShaderSet* computeShaderSet;
		MaterialTechniqueSet* techniqueSet;
		GfxImage* image;
		SndBank* sound;
		SndPatch* soundPatch;
		clipMap_t* clipMap;
		ComWorld* comWorld;
		GameWorld* gameWorld;
		MapEnts* mapEnts;
		GfxWorld* gfxWorld;
		GfxLightDef* lightDef;
		GfxLensFlareDef* lensFlareDef;
		Font* font;
		FontIcon* fontIcon;
		LocalizeEntry* localize;
		WeaponVariantDef* weapon;
		WeaponAttachment* attachment;
		WeaponAttachmentUnique* attachmentUnique;
		WeaponCamo* weaponCamo;
		CustomizationTable* customizationTable;
		CustomizationColorInfo* customizationColorInfo;
		SndDriverGlobals* sndDriverGlobals;
		FxEffectDefHandleRaw fx;
		TagFxSet* tagFX;
		FxLensFlareDefPtr newLensFlareDef;
		FxImpactTable* impactFx;
		SoundsImpactTable* impactSounds;
		CgMediaTable* cgMedia;
		PlayerSoundsTable* playerSounds;
		PlayerFXTable* playerFX;
		SharedWeaponSounds* sharedWeaponSounds;
		RawFile* rawfile;
		StringTable* stringTable;
		StructuredTable* structuredTable;
		LeaderboardDef* leaderboardDef;*/
		DDLRoot* ddlRoot;
		/*Glasses* glasses;
		TextureList* textureList;
		ScriptParseTree* scriptParseTree;
		KeyValuePairs* keyValuePairs;
		VehicleDef* vehicleDef;
		AddonMapEnts* addonMapEnts;
		TracerDef* tracerDef;
		Qdb* qdb;
		Slug* slug;
		SurfaceFXTableDef* surfaceFXTableDef;
		SurfaceSoundDef* surfaceSoundDef;
		FootstepTableDef* footstepTableDef;
		EntitySoundImpacts* entitySoundImpacts;
		EntityFxImpacts* entityFxImpacts;
		ZBarrierDef* zbarrierDef;
		VehicleFxDef* vehicleFxDef;
		VehicleSoundDef* vehicleSoundDef;
		ArchiveTypeInfoArray* typeInfo;
		ScriptBundle* scriptBundle;
		ScriptBundleList* scriptBundleList;
		RumbleInfo* rumble;
		BulletPenetrationTable* bulletPenetration;
		LocDmgTable* locDmgTable;
		AimTable* aimTable;
		XModelAlias* xModelAlias;
		Character* character;
		AIType* aiType;
		PlayerCharacter* player_character;
		AnimSelectorTableSet* animSelectorTableSet;
		AnimMappingTable* animMappingTable;
		AnimStateMachine* animStateMachine;
		BehaviorTree* behaviorTree;
		BehaviorStateMachine* behaviorStateMachine;
		TTFDef* ttfDef;
		GfxSiegeAnim* sanim;
		GfxLightDescription* lightDescription;
		ShellshockParams* shellshock;
		XCam* xcam;
		BGCacheInfo* bgCache;
		TextureCombo* textureCombo;
		FlameTable* flameTable;
		Bitfield* bitfield;
		AttachmentCosmeticVariant* attachmentCosmeticVariant;
		MapTable* mapTable;
		Medal* medal;
		MedalTable* medalTable;
		Objective* objective;
		ObjectiveList* objectiveList;
		NavMeshData* navMesh;
		NavVolumeData* navVolume;
		BinaryHTML* binaryHTML;
		LaserDef* laserDef;
		BeamDef* beamDef;
		StreamerHint* streamerHint;*/
		void* data;
		RawFile* luaFile;
	};

	struct XAsset
	{
		XAssetType type;
		XAssetHeader header;
	};
}
