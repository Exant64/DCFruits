#include "pch.h"
#include "SA2ModLoader.h"
#include "alo_energyfruit.h"
#include "alo_peacefruit.h"
extern "C"
{
	NJS_TEXNAME  AL_OBJECT_DC_TEXNAME[88];
	NJS_TEXLIST AL_OBJECT_DC_TEXLIST = { arrayptrandlength(AL_OBJECT_DC_TEXNAME) };

	ChaoItemStats GenericStats = { 0,0,0,0,0,0,40,0 };

	int EnergyFruitID;
	int PeaceFruitID;

	int (*RegisterChaoFruit)(NJS_OBJECT* model, NJS_TEXLIST* texlist, ChaoItemStats* stats, void* attrib, void* Func, const char* name, const char* description);
	void (*RegisterDataFunc)(void* ptr);
	void (*RegisterBlackMarketRareFruit)(int ID, int chance);
	void (*RegisterChaoTexlistLoad)(const char* name, NJS_TEXLIST* load);

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
		RegisterChaoTexlistLoad("AL_OBJECT_DC", &AL_OBJECT_DC_TEXLIST);

		EnergyFruitID = RegisterChaoFruit(&object_00103880, &AL_OBJECT_DC_TEXLIST, &GenericStats, &EnergyFruit, EnergyFruitFunc, EnergyFruitStr, EnergyFruitDesc);
		PeaceFruitID = RegisterChaoFruit(&object_00104BA8, &AL_OBJECT_DC_TEXLIST, &GenericStats, &PeaceFruit, PeaceFruitFunc, PeaceFruitStr, PeaceFruitDesc);

		RegisterBlackMarketRareFruit(EnergyFruitID, 50);
		RegisterBlackMarketRareFruit(PeaceFruitID,50);
	}
	__declspec(dllexport) void Init()
	{
		HMODULE h = GetModuleHandle(L"CWE");
		RegisterChaoFruit = (int (*)(NJS_OBJECT * model, NJS_TEXLIST * texlist, ChaoItemStats * stats, void* attrib, void*, const char*, const char*))GetProcAddress(h, "RegisterChaoFruit");
		RegisterDataFunc = (void (*)(void* ptr))GetProcAddress(h, "RegisterDataFunc");
		RegisterBlackMarketRareFruit = (void (*)(int ID, int chance))GetProcAddress(h, "RegisterBlackMarketRareFruit");
		RegisterChaoTexlistLoad = (void (*)(const char* name, NJS_TEXLIST * load))GetProcAddress(h, "RegisterChaoTexlistLoad");
			
		RegisterDataFunc(CWELoad);
	}
	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}