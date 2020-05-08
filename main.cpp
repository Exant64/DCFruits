#include "pch.h"
#include "SA2ModLoader.h"
#include "alo_energyfruit.h"
#include "alo_peacefruit.h"
extern "C"
{
	NJS_TEXNAME  AL_OBJECT_DC_TEXNAME[88];
	NJS_TEXLIST AL_OBJECT_DC_TEXLIST = { arrayptrandlength(AL_OBJECT_DC_TEXNAME) };
	DataPointer(NJS_TEXLIST, AL_TOY_TEXLIST, 0x01366AFC);
	struct LOADED_TEX_INFO
	{
		char* filename;
		NJS_TEXLIST* pTexlist;
	};

	//This function is just a copy for a function at address 0x, i was just lazy to write the functionpointer code for it so i just copied it out
	void LoadChaoTexlist(const char* a2, NJS_TEXLIST* texlist, unsigned __int16 a1)
	{
		int v3; // eax
		const char** v4; // esi
		char** v5; // edi

		v3 = a1 << 11;
		v4 = (const char**)((char*)0x1DCD600 + v3);
		v5 = (char**)((char*)0x1DCDE00 + v3);
		LoadTextureList(a2, texlist);
		if (v4 < v5)
		{
			while (v4[1])
			{
				v4 += 2;
				if (v4 >= v5)
				{
					return;
				}
			}
			v4[1] = (char*)texlist;
			*v4 = a2;
		}
	}
	
	void LoadTexturesHook()
	{
		LoadChaoTexlist("AL_TOY", &AL_TOY_TEXLIST, 0);
		LoadChaoTexlist("AL_OBJECT_DC", &AL_OBJECT_DC_TEXLIST, 0);
	}
	ChaoItemStats GenericStats = { 0,0,0,0,0,0,40,0 };
	struct ItemChance
	{
		Sint8 item;
		Sint8 chance;
	};
	int EnergyFruitID;
	int PeaceFruitID;

	int (*RegisterChaoFruit)(NJS_OBJECT* model, NJS_TEXLIST* texlist, ChaoItemStats* stats, void* attrib, void* Func, const char* name, const char* description);
	void (*RegisterDataFunc)(void* ptr);
	void (*RegisterBlackMarketRareFruit)(ItemChance* chance);
	void __cdecl EnergyFruitFunc(ChaoData* data, ObjectMaster* fruit)
	{
		//technically you dont need to check for this but im doing it just to be sure
		if (fruit->Data1.Entity->Rotation.x == EnergyFruitID)
		{
			//kinda unorthodox to do it like this, you would need to use addemotion but i dont wanna clutter this project with a lot of function pointers and shit
			//you can look at addemotion on CWE source (if it ever gets released)
			data->data.Emotion.Normal_Curiosity += 10;
			if (data->data.Emotion.Normal_Curiosity > 100)
				data->data.Emotion.Normal_Curiosity = 100;

			data->data.Emotion.CryBaby_Energetic += 10;
			if (data->data.Emotion.CryBaby_Energetic > 100)
				data->data.Emotion.CryBaby_Energetic = 100;

			data->data.Emotion.Normal_Carefree -= 10;
			if (data->data.Emotion.Normal_Carefree < -100)
				data->data.Emotion.Normal_Carefree = -100;

			data->data.Emotion.Naive_Normal -= 10;
			if (data->data.Emotion.Naive_Normal < -100)
				data->data.Emotion.Naive_Normal = -100;
		}
	}
	void __cdecl PeaceFruitFunc(ChaoData* data, ObjectMaster* fruit)
	{
		if(fruit->Data1.Entity->Rotation.x == PeaceFruitID)
		{ 
			data->data.Emotion.Normal_Curiosity -= 10;
			if (data->data.Emotion.Normal_Curiosity < -100) 
				data->data.Emotion.Normal_Curiosity = -100;

			data->data.Emotion.CryBaby_Energetic -= 10;
			if (data->data.Emotion.CryBaby_Energetic < -100)
				data->data.Emotion.CryBaby_Energetic = -100;

			data->data.Emotion.Normal_Carefree += 10;
			if (data->data.Emotion.Normal_Carefree > 100)
				data->data.Emotion.Normal_Carefree = 100;

			data->data.Emotion.Naive_Normal += 10;
			if (data->data.Emotion.Naive_Normal > 100)
				data->data.Emotion.Naive_Normal = 100;
		}
	}
	struct BlackMarketItemAttributes
	{
		int PurchasePrice;
		int SalePrice;
		__int16 RequiredEmblems;
		__int16 Name;
		__int16 Descriptions;
		__int16 Unknown;
	};
	const char* EnergyFruitStr = "Energy Fruit";
	const char* EnergyFruitDesc = "Makes Chao active.";
	const char* PeaceFruitStr = "Peace Fruit";
	const char* PeaceFruitDesc = "Makes Chao relaxed.";
	BlackMarketItemAttributes EnergyFruit = { 3000, 1000, 0, -1, -1, 0 };
	BlackMarketItemAttributes PeaceFruit = { 3000, 1000, 0, -1, -1, 0 };
	void CWELoad()
	{
		EnergyFruitID = RegisterChaoFruit(&object_00103880, &AL_OBJECT_DC_TEXLIST, &GenericStats, &EnergyFruit, EnergyFruitFunc, EnergyFruitStr, EnergyFruitDesc);
		PeaceFruitID = RegisterChaoFruit(&object_00104BA8, &AL_OBJECT_DC_TEXLIST, &GenericStats, &PeaceFruit, PeaceFruitFunc, PeaceFruitStr, PeaceFruitDesc);
		ItemChance eFruitChance = { EnergyFruitID, 50 };
		ItemChance pFruitChance = { PeaceFruitID, 50 };
		RegisterBlackMarketRareFruit(&eFruitChance);
		RegisterBlackMarketRareFruit(&pFruitChance);
	}
	__declspec(dllexport) void Init()
	{
		WriteCall((void*)0x0052AC98, LoadTexturesHook);
		
		HMODULE h = GetModuleHandle(L"CWE");
		RegisterChaoFruit = (int (*)(NJS_OBJECT * model, NJS_TEXLIST * texlist, ChaoItemStats * stats, void* attrib, void*, const char*, const char*))GetProcAddress(h, "RegisterChaoFruit");
		RegisterDataFunc = (void (*)(void* ptr))GetProcAddress(h, "RegisterDataFunc");
		RegisterBlackMarketRareFruit = (void (*)(ItemChance * chance))GetProcAddress(h, "RegisterBlackMarketRareFruit");
		RegisterDataFunc(CWELoad);
	}
	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}